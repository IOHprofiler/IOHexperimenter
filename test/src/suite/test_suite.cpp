#include <cmath>
#include <list>

#include <gtest/gtest.h>
#include "ioh.hpp"

TEST(suite, pbo)
{
	std::vector<int> problem_id; 
	std::vector<int> instance_id;
	std::vector<int> dimension = {16,64};

	for (auto i = 1; i != 26; ++i) {
		problem_id.push_back(i);
	}

	for (auto i = 1; i != 6; ++i) {
		instance_id.push_back(i);
	}
	ioh::suite::pbo pbo(problem_id,instance_id,dimension);

	ASSERT_EQ(pbo.get_number_of_problems(),25);
	ASSERT_EQ(pbo.get_number_of_instances(),5);
	ASSERT_EQ(pbo.get_number_of_dimensions(),2);
	ASSERT_EQ(pbo.get_suite_name(),"PBO");

	auto tmp_p_index = 0;
	auto tmp_i_index = 0;
	auto tmp_d_index = 0;

	std::shared_ptr<ioh::problem::pbo::pbo_base> problem;	
	while ( (problem = pbo.get_next_problem()) != nullptr) {
		ASSERT_LT(tmp_p_index, 25);
		ASSERT_EQ(problem->get_problem_id(),problem_id[tmp_p_index]);
		ASSERT_EQ(problem->get_instance_id(),instance_id[tmp_i_index]);
		ASSERT_EQ(problem->get_number_of_variables(),dimension[tmp_d_index]);
		problem = pbo.get_current_problem();
		ASSERT_EQ(problem->get_problem_id(),problem_id[tmp_p_index]);
		ASSERT_EQ(problem->get_instance_id(),instance_id[tmp_i_index]);
		ASSERT_EQ(problem->get_number_of_variables(),dimension[tmp_d_index]);
		
		tmp_i_index++;
		if (tmp_i_index == 5) {
			tmp_i_index = 0;
			tmp_d_index++;
			if (tmp_d_index == 2) {
				tmp_d_index = 0;
				tmp_p_index++;
			}
		}
	}
}

TEST(suite, bbob)
{
	std::vector<int> problem_id;
	std::vector<int> instance_id;
	std::vector<int> dimension = {2,10};

	for (auto i = 1; i != 25; ++i) {
		problem_id.push_back(i);
	}

	for (auto i = 1; i != 6; ++i) {
		instance_id.push_back(i);
	}
	ioh::suite::bbob bbob(problem_id,instance_id,dimension);

	ASSERT_EQ(bbob.get_number_of_problems(),24);
	ASSERT_EQ(bbob.get_number_of_instances(),5);
	ASSERT_EQ(bbob.get_number_of_dimensions(),2);
	ASSERT_EQ(bbob.get_suite_name(),"BBOB");

	auto tmp_p_index = 0;
	auto tmp_i_index = 0;
	auto tmp_d_index = 0;

	std::shared_ptr<ioh::problem::bbob::bbob_base> problem;	
	while( (problem = bbob.get_next_problem()) != nullptr) {
		ASSERT_LT(tmp_p_index, 25);
		ASSERT_EQ(problem->get_problem_id(),problem_id[tmp_p_index]);
		ASSERT_EQ(problem->get_instance_id(),instance_id[tmp_i_index]);
		ASSERT_EQ(problem->get_number_of_variables(),dimension[tmp_d_index]);
		problem = bbob.get_current_problem();
		ASSERT_EQ(problem->get_problem_id(),problem_id[tmp_p_index]);
		ASSERT_EQ(problem->get_instance_id(),instance_id[tmp_i_index]);
		ASSERT_EQ(problem->get_number_of_variables(),dimension[tmp_d_index]);
		
		tmp_i_index++;
		if (tmp_i_index == 5) {
			tmp_i_index = 0;
			tmp_d_index++;
			if (tmp_d_index == 2) {
				tmp_d_index = 0;
				tmp_p_index++;
			}
		}
	}
}