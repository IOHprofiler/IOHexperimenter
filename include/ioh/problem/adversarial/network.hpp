#pragma once

#include <cmath>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "tensor_file.hpp"

namespace ioh::problem::adversarial
{
    //! A single 2D convolution layer (stride 1, symmetric padding), weight shape [out,in,kh,kw].
    struct ConvLayer
    {
        Tensor weight;
        Tensor bias;
        size_t padding{};
    };

    //! A single dense (fully-connected) layer, weight shape [out,in].
    struct DenseLayer
    {
        Tensor weight;
        Tensor bias;
    };

    /**
     * @brief A small, fixed, dependency-free feed-forward CNN: alternating
     * Conv2d+ReLU (some followed by 2x2 max pooling), flatten, then
     * Linear+ReLU, Linear. Reproduces the eval-mode forward pass of the
     * torch reference models in amheattack/models.py (dropout is the
     * identity in eval mode, so it is omitted here entirely).
     *
     * Immutable and safe to share (via shared_ptr) between AdversarialAttack
     * instances of the same dataset -- only the caller-provided scratch
     * buffers in `forward` carry per-evaluation state.
     */
    struct Network
    {
        std::vector<ConvLayer> convs;
        //! Indices (into `convs`) of conv layers immediately followed by a 2x2 max pool.
        std::vector<bool> pool_after;
        std::vector<DenseLayer> dense;
        size_t c{}, h{}, w{};

        [[nodiscard]] size_t input_size() const { return c * h * w; }
        [[nodiscard]] size_t n_classes() const { return dense.empty() ? 0 : dense.back().bias.numel(); }

        //! Upper bound on the largest activation buffer needed anywhere in the forward pass.
        [[nodiscard]] size_t scratch_size() const
        {
            size_t cur_c = c, cur_h = h, cur_w = w;
            size_t max_elems = cur_c * cur_h * cur_w;
            for (size_t i = 0; i < convs.size(); ++i)
            {
                cur_c = convs[i].weight.shape[0];
                max_elems = std::max(max_elems, cur_c * cur_h * cur_w);
                if (pool_after[i])
                {
                    cur_h /= 2;
                    cur_w /= 2;
                }
            }
            for (const auto &d : dense)
                max_elems = std::max(max_elems, d.weight.shape[0]);
            return max_elems;
        }

        /**
         * @brief Runs the forward pass. `input` must have `input_size()` elements
         * in NCHW (C-outer, W-innermost) layout, un-normalized in [0, 1]-ish range
         * (no clamping is performed here). `logits` is resized to `n_classes()`.
         * `scratch_a`/`scratch_b` are ping-ponged activation buffers, resized to
         * `scratch_size()` on first use by the caller.
         */
        void forward(const std::vector<float> &input, std::vector<float> &logits, std::vector<float> &scratch_a,
                     std::vector<float> &scratch_b) const
        {
            const float *cur = input.data();
            size_t cur_c = c, cur_h = h, cur_w = w;
            float *buf_a = scratch_a.data();
            float *buf_b = scratch_b.data();

            for (size_t i = 0; i < convs.size(); ++i)
            {
                const auto &layer = convs[i];
                const size_t out_c = layer.weight.shape[0];
                const size_t in_c = layer.weight.shape[1];
                const size_t kh = layer.weight.shape[2];
                const size_t kw = layer.weight.shape[3];
                if (in_c != cur_c)
                    throw std::runtime_error("adversarial::Network: channel mismatch in conv layer");

                float *out = buf_a;
                conv2d_relu(cur, cur_c, cur_h, cur_w, layer.weight.data.data(), layer.bias.data.data(), out_c, kh, kw,
                            layer.padding, out);
                cur = out;
                cur_c = out_c;
                std::swap(buf_a, buf_b);

                if (pool_after[i])
                {
                    float *pooled = buf_a;
                    maxpool2(cur, cur_c, cur_h, cur_w, pooled);
                    cur = pooled;
                    cur_h /= 2;
                    cur_w /= 2;
                    std::swap(buf_a, buf_b);
                }
            }

            const size_t flat = cur_c * cur_h * cur_w;
            std::vector<float> flat_copy;
            const float *dense_in = cur;
            if (dense_in == input.data())
            {
                // No conv layers at all: copy input so we never read/write the same buffer.
                flat_copy.assign(input.begin(), input.end());
                dense_in = flat_copy.data();
            }

            for (size_t i = 0; i < dense.size(); ++i)
            {
                const bool is_last = (i + 1 == dense.size());
                const auto &layer = dense[i];
                const size_t out_n = layer.weight.shape[0];
                const size_t in_n = layer.weight.shape[1];
                if (in_n != (i == 0 ? flat : dense[i - 1].weight.shape[0]))
                    throw std::runtime_error("adversarial::Network: dimension mismatch in dense layer");

                float *out = is_last ? nullptr : buf_a;
                if (is_last)
                {
                    logits.resize(out_n);
                    out = logits.data();
                }
                linear(dense_in, in_n, layer.weight.data.data(), layer.bias.data.data(), out_n, !is_last, out);
                dense_in = out;
                if (!is_last)
                    std::swap(buf_a, buf_b);
            }
        }

    private:
        //! Direct (non-im2col) convolution with fused ReLU, symmetric zero padding, stride 1.
        static void conv2d_relu(const float *in, size_t in_c, size_t in_h, size_t in_w, const float *weight,
                                const float *bias, size_t out_c, size_t kh, size_t kw, size_t pad, float *out)
        {
            for (size_t oc = 0; oc < out_c; ++oc)
            {
                const float *w_oc = weight + oc * in_c * kh * kw;
                float *out_oc = out + oc * in_h * in_w;
                for (size_t oy = 0; oy < in_h; ++oy)
                {
                    for (size_t ox = 0; ox < in_w; ++ox)
                    {
                        float acc = bias[oc];
                        for (size_t ic = 0; ic < in_c; ++ic)
                        {
                            const float *in_ic = in + ic * in_h * in_w;
                            const float *w_ic = w_oc + ic * kh * kw;
                            for (size_t ky = 0; ky < kh; ++ky)
                            {
                                const long iy = static_cast<long>(oy + ky) - static_cast<long>(pad);
                                if (iy < 0 || iy >= static_cast<long>(in_h))
                                    continue;
                                const float *in_row = in_ic + static_cast<size_t>(iy) * in_w;
                                const float *w_row = w_ic + ky * kw;
                                for (size_t kx = 0; kx < kw; ++kx)
                                {
                                    const long ix = static_cast<long>(ox + kx) - static_cast<long>(pad);
                                    if (ix < 0 || ix >= static_cast<long>(in_w))
                                        continue;
                                    acc += in_row[static_cast<size_t>(ix)] * w_row[kx];
                                }
                            }
                        }
                        out_oc[oy * in_w + ox] = std::max(0.0f, acc);
                    }
                }
            }
        }

        //! 2x2 max pooling, stride 2, no padding (input dims assumed even).
        static void maxpool2(const float *in, size_t c, size_t h, size_t w, float *out)
        {
            const size_t oh = h / 2, ow = w / 2;
            for (size_t ch = 0; ch < c; ++ch)
            {
                const float *in_ch = in + ch * h * w;
                float *out_ch = out + ch * oh * ow;
                for (size_t oy = 0; oy < oh; ++oy)
                {
                    for (size_t ox = 0; ox < ow; ++ox)
                    {
                        const size_t iy = oy * 2, ix = ox * 2;
                        const float v00 = in_ch[iy * w + ix];
                        const float v01 = in_ch[iy * w + ix + 1];
                        const float v10 = in_ch[(iy + 1) * w + ix];
                        const float v11 = in_ch[(iy + 1) * w + ix + 1];
                        out_ch[oy * ow + ox] = std::max(std::max(v00, v01), std::max(v10, v11));
                    }
                }
            }
        }

        //! Dense layer: out = weight @ in + bias, optionally fused with ReLU.
        static void linear(const float *in, size_t in_n, const float *weight, const float *bias, size_t out_n,
                            bool relu, float *out)
        {
            for (size_t o = 0; o < out_n; ++o)
            {
                const float *w_row = weight + o * in_n;
                float acc = bias[o];
                for (size_t i = 0; i < in_n; ++i)
                    acc += w_row[i] * in[i];
                out[o] = relu ? std::max(0.0f, acc) : acc;
            }
        }
    };

    //! A bank of fixed base images (in [0, 1], NCHW layout) and their ground-truth labels.
    struct ImageBank
    {
        std::vector<std::vector<float>> images;
        std::vector<int> labels;
    };

    /**
     * @brief Builds the MNIST or CIFAR-10 network from its exported weight file.
     * Architecture is implied by the tensor names/shapes found in the file.
     */
    inline Network load_network(const std::string &dataset)
    {
        const auto file = dataset == "mnist" ? "adversarial/mnist_cnn.ioh" : "adversarial/cifar_cnn.ioh";
        const auto tensors = read_tensor_file(ioh::data::file(file));

        Network net;
        std::vector<std::string> conv_names;
        std::vector<std::string> dense_names;
        std::vector<bool> pool_after;

        if (dataset == "mnist")
        {
            net.c = 1;
            net.h = 28;
            net.w = 28;
            conv_names = {"conv1", "conv2"};
            pool_after = {true, true};
            dense_names = {"fc1", "fc2"};
        }
        else if (dataset == "cifar10")
        {
            net.c = 3;
            net.h = 32;
            net.w = 32;
            conv_names = {"conv1", "conv2", "conv3", "conv4"};
            pool_after = {false, true, false, true};
            dense_names = {"fc1", "fc2"};
        }
        else
        {
            throw std::runtime_error(fmt::format("adversarial::load_network: unknown dataset '{}'", dataset));
        }

        for (size_t i = 0; i < conv_names.size(); ++i)
        {
            ConvLayer layer;
            layer.weight = tensors.at(conv_names[i] + ".weight");
            layer.bias = tensors.at(conv_names[i] + ".bias");
            layer.padding = (layer.weight.shape[2] - 1) / 2; // "same" padding, matches models.py
            net.convs.push_back(std::move(layer));
        }
        net.pool_after = pool_after;

        for (const auto &name : dense_names)
        {
            DenseLayer layer;
            layer.weight = tensors.at(name + ".weight");
            layer.bias = tensors.at(name + ".bias");
            net.dense.push_back(std::move(layer));
        }

        return net;
    }

    //! Loads the fixed 100-image bank (uint8 -> [0,1] float) for `dataset`.
    inline ImageBank load_images(const std::string &dataset)
    {
        const auto file = dataset == "mnist" ? "adversarial/mnist_images.ioh" : "adversarial/cifar_images.ioh";
        const auto tensors = read_tensor_file(ioh::data::file(file));

        const auto &images_t = tensors.at("images"); // [n, c, h, w], values already dequantized 0..255
        const auto &labels_t = tensors.at("labels");  // [n]

        const size_t n = images_t.shape[0];
        const size_t per_image = images_t.numel() / n;

        ImageBank bank;
        bank.images.resize(n);
        bank.labels.resize(n);
        for (size_t i = 0; i < n; ++i)
        {
            bank.images[i].resize(per_image);
            for (size_t j = 0; j < per_image; ++j)
                bank.images[i][j] = images_t.data[i * per_image + j] / 255.0f;
            bank.labels[i] = static_cast<int>(labels_t.data[i]);
        }
        return bank;
    }

    //! The network for `dataset` ("mnist" or "cifar10"), loaded on first use and shared (see ioh::data::cached)
    inline std::shared_ptr<const Network> get_network(const std::string &dataset)
    {
        return ioh::data::cached<Network>("adversarial/network/" + dataset, [&] { return load_network(dataset); });
    }

    //! The image bank for `dataset`, loaded on first use and shared (see ioh::data::cached)
    inline std::shared_ptr<const ImageBank> get_images(const std::string &dataset)
    {
        return ioh::data::cached<ImageBank>("adversarial/images/" + dataset, [&] { return load_images(dataset); });
    }
} // namespace ioh::problem::adversarial
