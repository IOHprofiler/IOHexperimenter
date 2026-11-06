#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "ioh/common/format.hpp"
#include "ioh/problem/single.hpp"
#include "network.hpp"

namespace ioh::problem
{
    namespace adversarial
    {
        /**
         * @brief Numerically stable log(softmax(logits)[label]) = logits[label] - logsumexp(logits).
         */
        inline double log_softmax_at(const std::vector<float> &logits, const int label)
        {
            const float max_logit = *std::max_element(logits.begin(), logits.end());
            double sum_exp = 0.0;
            for (const auto v : logits)
                sum_exp += std::exp(static_cast<double>(v) - static_cast<double>(max_logit));
            const double log_sum_exp = static_cast<double>(max_logit) + std::log(sum_exp);
            return static_cast<double>(logits[static_cast<size_t>(label)]) - log_sum_exp;
        }

        /**
         * @brief Black-box adversarial-attack objective on a fixed image classifier,
         * proposed as a many-dimensional global optimization benchmark (amhe-attack,
         * https://github.com/wz7475/amhe-attack). For a fixed base image `x` (from a
         * bundled 100-image bank per dataset) with true label `y`, and a search vector
         * `delta` (the perturbation) bounded in [-eps, eps]^d:
         *
         *   f(delta) = -log softmax(model(x + delta))[y]  -  alpha * ||delta||_2
         *
         * maximized. `x + delta` is NOT clamped to [0,1] by default (matching the
         * reference implementation's main experimental path in amheattack/attack.py);
         * pass `clamp_input=true` for the amheattack/stochastic_attack.py semantics.
         *
         * Unlike most IOH problems, `instance` selects which of the 100 bundled base
         * images is attacked -- it changes the entire landscape, not just an affine
         * transformation of one fixed function. There is no known optimum, so
         * `optimum()` is left at its default (NaN).
         */
        class AdversarialAttack : public RealSingleObjective
        {
        public:
            //! The registry entries (see registry_entries_of): 12 (dataset, eps, alpha) presets with ids 5001-5006
            //! (MNIST) and 5011-5016 (CIFAR-10). Listing them loads no data: the network and the image bank are
            //! loaded when the first problem of a dataset is created.
            static std::vector<RegistryEntry<AdversarialAttack>> registry_entries();

        private:
            std::shared_ptr<const Network> net_;
            std::shared_ptr<const ImageBank> bank_;
            std::string dataset_;
            double eps_;
            double alpha_;
            int image_index_;
            int label_;
            bool clamp_input_;

            // Per-object scratch buffers: NOT shared between instances, so one
            // AdversarialAttack object must not be evaluated concurrently from
            // multiple threads (see tests/cpp/problem/test_threaded.cpp).
            mutable std::vector<float> input_;
            mutable std::vector<float> logits_;
            mutable std::vector<float> scratch_a_;
            mutable std::vector<float> scratch_b_;

            [[nodiscard]] static int dimension_for(const std::string &dataset)
            {
                return static_cast<int>(get_network(dataset)->input_size());
            }

        protected:
            double evaluate(const std::vector<double> &x) override
            {
                const auto &base = bank_->images[static_cast<size_t>(image_index_)];
                double sq = 0.0;
                for (size_t i = 0; i < x.size(); ++i)
                {
                    const float v = base[i] + static_cast<float>(x[i]);
                    input_[i] = clamp_input_ ? std::min(1.0f, std::max(0.0f, v)) : v;
                    sq += x[i] * x[i];
                }

                net_->forward(input_, logits_, scratch_a_, scratch_b_);
                const double log_p_true = log_softmax_at(logits_, label_);
                return -log_p_true - alpha_ * std::sqrt(sq);
            }

        public:
            /**
             * @brief Construct a new AdversarialAttack object.
             *
             * @param problem_id the id of the problem (one per (dataset, eps, alpha) preset)
             * @param instance the instance id: selects the base image, 1-indexed into the
             *                 100-image bank as `(instance - 1) mod 100`
             * @param n_variables ignored: dimension is fixed by `dataset` (784 for mnist,
             *                    3072 for cifar10) and derived from the loaded network
             * @param name the name of the problem
             * @param dataset "mnist" or "cifar10"
             * @param eps the perturbation bound: search space is [-eps, eps]^d
             * @param alpha weight of the L2 perturbation-size penalty
             * @param clamp_input whether to clamp x + delta to [0, 1] before classification
             */
            AdversarialAttack(const int problem_id, const int instance, const int n_variables,
                              const std::string &name, const std::string &dataset, const double eps,
                              const double alpha, const bool clamp_input = false) :
                RealSingleObjective(MetaData(problem_id, instance, name, dimension_for(dataset),
                                             common::OptimizationType::MAX),
                                    Bounds<double>(dimension_for(dataset), -eps, eps)),
                net_(get_network(dataset)), bank_(get_images(dataset)), dataset_(dataset), eps_(eps), alpha_(alpha),
                image_index_(static_cast<int>((static_cast<size_t>(instance - 1)) % bank_->images.size())),
                label_(bank_->labels[static_cast<size_t>(image_index_)]), clamp_input_(clamp_input),
                input_(net_->input_size()), scratch_a_(net_->scratch_size()), scratch_b_(net_->scratch_size())
            {
                static_cast<void>(n_variables); // dimension is derived from `dataset`, not user-supplied
                // Negative weight, matching star_discrepancy: this is a MAX problem, so a hard
                // bounds violation must push y towards -infinity, not +infinity.
                enforce_bounds(-std::numeric_limits<double>::infinity(), constraint::Enforced::HARD);
            }

            [[nodiscard]] const std::string &dataset() const { return dataset_; }
            [[nodiscard]] double eps() const { return eps_; }
            [[nodiscard]] double alpha() const { return alpha_; }
            [[nodiscard]] int image_index() const { return image_index_; }
            [[nodiscard]] int true_label() const { return label_; }
            [[nodiscard]] bool clamp_input() const { return clamp_input_; }

            //! Softmax probabilities from the most recent `evaluate` call.
            [[nodiscard]] std::vector<double> probabilities() const
            {
                const float max_logit = *std::max_element(logits_.begin(), logits_.end());
                std::vector<double> probs(logits_.size());
                double sum = 0.0;
                for (size_t i = 0; i < logits_.size(); ++i)
                {
                    probs[i] = std::exp(static_cast<double>(logits_[i] - max_logit));
                    sum += probs[i];
                }
                for (auto &p : probs)
                    p /= sum;
                return probs;
            }

            //! argmax of the most recent `evaluate` call's logits; != true_label() means the attack succeeded.
            [[nodiscard]] int predicted_label() const
            {
                return static_cast<int>(std::max_element(logits_.begin(), logits_.end()) - logits_.begin());
            }
        };
    } // namespace adversarial

    inline std::vector<RegistryEntry<adversarial::AdversarialAttack>>
    adversarial::AdversarialAttack::registry_entries()
    {
        struct Preset
        {
            int id;
            const char *dataset;
            double eps;
            double alpha;
        };

        const std::vector<Preset> presets = {
            {5001, "mnist", 0.1, 0.0},   {5002, "mnist", 0.3, 0.0},   {5003, "mnist", 1.0, 0.0},
            {5004, "mnist", 0.1, 0.05},  {5005, "mnist", 0.3, 0.05},  {5006, "mnist", 1.0, 0.05},
            {5011, "cifar10", 0.1, 0.0}, {5012, "cifar10", 0.3, 0.0}, {5013, "cifar10", 1.0, 0.0},
            {5014, "cifar10", 0.1, 0.05}, {5015, "cifar10", 0.3, 0.05}, {5016, "cifar10", 1.0, 0.05},
        };

        std::vector<RegistryEntry<AdversarialAttack>> entries;
        for (const auto &preset : presets)
        {
            const std::string dataset = preset.dataset;
            const auto name = fmt::format("Adversarial{}{}", dataset == "mnist" ? "MNIST" : "CIFAR10", preset.id);
            entries.push_back({preset.id, name,
                               [preset, dataset, name](const int instance, const int n_variables) {
                                   return std::make_shared<AdversarialAttack>(preset.id, instance, n_variables, name,
                                                                              dataset, preset.eps, preset.alpha);
                               }});
        }
        return entries;
    }
} // namespace ioh::problem
