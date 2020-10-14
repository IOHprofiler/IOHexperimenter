/// \file f_w_model_leading_ones.hpp
/// \brief cpp file for class f_w_model_leading_ones.hpp
///
/// \author Furong Ye
/// \date 2019-08-28
#pragma once
#include "wmodel_base.hpp"

namespace ioh
{
	namespace problem
	{
		namespace wmodel
		{
			class W_Model_LeadingOnes : public wmodel_base
			{
			public:

				std::vector<int> dummy_info;
				double dummy_para = 0;
				int epistasis_para = 0;
				int neutrality_para = 0;
				int ruggedness_para = 0;
				std::vector<int> ruggedness_info;
				int temp_dimension;

				W_Model_LeadingOnes(int instance_id = DEFAULT_INSTANCE, int dimension = DEFAULT_DIMENSION) :
					wmodel_base("W_Model_LeadingOnes", instance_id, dimension),
					temp_dimension(0)
				{
					set_number_of_variables(dimension);
				}

				void prepare_problem() override
				{
					this->temp_dimension = static_cast<int>(this->get_number_of_variables());

					if (this->dummy_para > 0)
					{
						this->dummy_info = utils::dummy(this->temp_dimension, dummy_para, 10000);
						assert(this->dummy_info.size() == static_cast<size_t>(this->temp_dimension * this->dummy_para));
						this->temp_dimension = static_cast<int>(this->dummy_info.size());
					}

					if (this->neutrality_para > 0)
					{
						this->temp_dimension = this->temp_dimension / this->neutrality_para;
					}
					if (this->ruggedness_para > 0)
					{
						this->ruggedness_info = utils::ruggedness_raw(
							utils::ruggedness_translate(this->ruggedness_para, this->temp_dimension),
							this->temp_dimension);
					}
				}

				void customize_optimal() override
				{
					auto dimension = get_number_of_variables();
					auto optimal_value = static_cast<int>(dimension * (this->dummy_para == 0 ? 1 : this->dummy_para)) / (
						this->neutrality_para == 0 ? 1 : this->neutrality_para);

					set_optimal(static_cast<double>(optimal_value));
				}

				/// \fn void set_w_setting(const double dummy_para, const int epistasis_para, const int neutrality_para, const int ruggedness_para)
				/// \brief A function to configure w-model layers.
				/// \param dummy_para percentage of valid bits.
				/// \param epistasis_para size of sub-string for epistasis
				/// \param neutrality_para size of sub-string for neutrality
				/// \param ruggedness_para gamma for ruggedness layper
				///
				/// For details of w-model, please read https://www.sciencedirect.com/science/article/pii/S1568494619308099
				/// and https://dl.acm.org/doi/pdf/10.1145/3205651.3208240
				void set_w_setting(const double dummy_para, const int epistasis_para, const int neutrality_para,
				                   const int ruggedness_para)
				{
					this->dummy_para = dummy_para;
					this->epistasis_para = epistasis_para;
					this->neutrality_para = neutrality_para;
					this->ruggedness_para = ruggedness_para;
				}

				double internal_evaluate(const std::vector<int>& x) override
				{
					std::vector<int> w_model_x;
					std::vector<int> tempX;
					size_t n;

					// Dummy Layer
					if (this->dummy_para > 0)
					{
						n = this->dummy_info.size();
						w_model_x.reserve(n);
						for (auto i = 0; i != n; ++i)
						{
							w_model_x.push_back(x[this->dummy_info[i]]);
						}
					}
					else
					{
						w_model_x = x;
					}

					// Neutrality layer
					if (this->neutrality_para > 0)
					{
						tempX = w_model_x;
						utils::layer_neutrality_compute(tempX, w_model_x, this->neutrality_para);
					}

					// Epistasis layer
					if (this->epistasis_para > 0)
					{
						tempX = w_model_x;
						utils::layer_epistasis_compute(tempX, w_model_x, this->epistasis_para);
					}

					// Base evaluate
					n = w_model_x.size();
					auto result = 0;
					for (auto i = 0; i != n; ++i)
					{
						if (w_model_x[i] == 1)
						{
							result = i + 1;
						}
						else
						{
							break;
						}
					}

					// Ruggedness layer
					if (this->ruggedness_para > 0)
					{
						result = this->ruggedness_info[result];
					}

					return static_cast<double>(result);
				}

				static W_Model_LeadingOnes* create(int instance_id = DEFAULT_INSTANCE,
				                                           int dimension = DEFAULT_DIMENSION)
				{
					return new W_Model_LeadingOnes(instance_id, dimension);
				}
			};
		}
	}
}
