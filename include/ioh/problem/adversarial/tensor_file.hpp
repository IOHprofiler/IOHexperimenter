#pragma once

#include <cstdint>
#include <cstring>
#include <fstream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

#include "ioh/common/data.hpp"
#include "ioh/common/format.hpp"

namespace ioh::problem::adversarial
{
    /**
     * @brief A dense, row-major float32 tensor, dequantized on load.
     */
    struct Tensor
    {
        std::vector<size_t> shape;
        std::vector<float> data;

        [[nodiscard]] size_t numel() const
        {
            return std::accumulate(shape.begin(), shape.end(), static_cast<size_t>(1), std::multiplies<>());
        }
    };

    //! name -> Tensor
    using TensorMap = std::map<std::string, Tensor>;

    namespace detail
    {
        //! IEEE-754 binary16 -> binary32, handling subnormals, inf and nan.
        inline float half_to_float(const uint16_t h)
        {
            const uint32_t sign = static_cast<uint32_t>(h & 0x8000u) << 16;
            uint32_t exponent = (h >> 10) & 0x1F;
            uint32_t mantissa = h & 0x3FF;
            uint32_t bits;

            if (exponent == 0)
            {
                if (mantissa == 0)
                {
                    // signed zero
                    bits = sign;
                }
                else
                {
                    // subnormal half -> normalize into a float32
                    exponent = 127 - 15 + 1;
                    while ((mantissa & 0x400) == 0)
                    {
                        mantissa <<= 1;
                        --exponent;
                    }
                    mantissa &= 0x3FF;
                    bits = sign | (exponent << 23) | (mantissa << 13);
                }
            }
            else if (exponent == 0x1F)
            {
                // inf / nan
                bits = sign | 0x7F800000u | (mantissa << 13);
            }
            else
            {
                bits = sign | ((exponent - 15 + 127) << 23) | (mantissa << 13);
            }

            float result;
            std::memcpy(&result, &bits, sizeof(float));
            return result;
        }

        inline bool is_little_endian()
        {
            const uint32_t probe = 1;
            uint8_t first_byte;
            std::memcpy(&first_byte, &probe, 1);
            return first_byte == 1;
        }

        template <typename T>
        inline T read_pod(std::istream &is)
        {
            T value{};
            is.read(reinterpret_cast<char *>(&value), sizeof(T));
            if (!is)
                throw std::runtime_error("adversarial::read_tensor_file: unexpected end of file");
            return value;
        }
    } // namespace detail

    /**
     * @brief Reads the little-endian "IOHNET01" tensor container used by the
     * adversarial-attack suite (weights and image banks), dequantizing
     * float16/uint8 payloads to float32 in the returned tensors.
     *
     * Container layout (no padding):
     *   magic       char[8]  "IOHNET01"
     *   n_tensors   uint32
     *   per tensor: name_len uint32, name char[name_len],
     *               ndim uint32, dims uint32[ndim],
     *               dtype uint32 (0=f16, 1=f32, 2=u8),
     *               data (row-major)
     */
    inline TensorMap read_tensor_file(const fs::path &path)
    {
        if (!detail::is_little_endian())
            throw std::runtime_error("adversarial::read_tensor_file: big-endian platforms are not supported");

        std::ifstream is(path, std::ios::binary);
        if (!is)
            throw std::runtime_error(fmt::format("adversarial::read_tensor_file: could not open {}", path.string()));

        char magic[8];
        is.read(magic, 8);
        if (!is || std::memcmp(magic, "IOHNET01", 8) != 0)
            throw std::runtime_error(fmt::format("adversarial::read_tensor_file: bad magic in {}", path.string()));

        const auto n_tensors = detail::read_pod<uint32_t>(is);
        TensorMap result;

        for (uint32_t t = 0; t < n_tensors; ++t)
        {
            const auto name_len = detail::read_pod<uint32_t>(is);
            std::string name(name_len, '\0');
            is.read(name.data(), static_cast<std::streamsize>(name_len));

            const auto ndim = detail::read_pod<uint32_t>(is);
            Tensor tensor;
            tensor.shape.resize(ndim);
            for (uint32_t d = 0; d < ndim; ++d)
                tensor.shape[d] = detail::read_pod<uint32_t>(is);

            const auto dtype = detail::read_pod<uint32_t>(is);
            const auto n = tensor.numel();
            tensor.data.resize(n);

            if (dtype == 0) // float16
            {
                std::vector<uint16_t> raw(n);
                is.read(reinterpret_cast<char *>(raw.data()), static_cast<std::streamsize>(n * sizeof(uint16_t)));
                for (size_t i = 0; i < n; ++i)
                    tensor.data[i] = detail::half_to_float(raw[i]);
            }
            else if (dtype == 1) // float32
            {
                is.read(reinterpret_cast<char *>(tensor.data.data()), static_cast<std::streamsize>(n * sizeof(float)));
            }
            else if (dtype == 2) // uint8
            {
                std::vector<uint8_t> raw(n);
                is.read(reinterpret_cast<char *>(raw.data()), static_cast<std::streamsize>(n));
                for (size_t i = 0; i < n; ++i)
                    tensor.data[i] = static_cast<float>(raw[i]);
            }
            else
            {
                throw std::runtime_error(
                    fmt::format("adversarial::read_tensor_file: unknown dtype {} in {}", dtype, path.string()));
            }

            if (!is)
                throw std::runtime_error(
                    fmt::format("adversarial::read_tensor_file: truncated tensor '{}' in {}", name, path.string()));

            result.emplace(std::move(name), std::move(tensor));
        }

        return result;
    }
} // namespace ioh::problem::adversarial
