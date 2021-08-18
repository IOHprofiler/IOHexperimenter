#include "../utils.hpp" 


// void bbob_random_search(const std::shared_ptr<ioh::problem::Real>& p)
// {
// 	using namespace ioh::common;
// 	std::vector<double> x(p->meta_data().n_variables);
// 	auto count = 0;
// 	while (count++ < 10)
// 	{
//         (*p)(random::pbo::uniform(p->meta_data().n_variables, random::integer()));
// 	}
// }

// void pbo_random_search(const std::shared_ptr<ioh::problem::Integer>& p)
// {
// 	using namespace ioh::common;
// 	auto count = 0;
// 	while (count++ < 10)
// 	{
// 		const auto x = random::integers(p->meta_data().n_variables, 0, 5);
// 		(*p)(x);
// 	}
// }



TEST_F(BaseTest, experiment_bbob)
{
	// using namespace ioh;

	// std::vector<int> pbs = {1, 2};
	// std::vector<int> ins = {1, 2};
	// std::vector<int> dims = {2, 10};
	// const auto suite = std::make_shared<suite::BBOB>(pbs, ins, dims);
	// const auto logger = std::make_shared<logger::Default>(fs::current_path(),
	// 	std::string("logger-experimenter"), "random-search", "10iterations", common::OptimizationType::Minimization, true);
	// auto experiment = Experimenter<problem::Real>(
	// 	suite, logger, bbob_random_search, 10);

	// EXPECT_EQ(experiment.independent_runs(), 10);
	// testing::internal::CaptureStdout();
	// experiment.run();
	// const auto output = testing::internal::GetCapturedStdout();
	// EXPECT_GE(count_newlines(output), 8);
	// // TODO: check that files are generated properly
	// experiment.logger()->flush();
	// try
	// {
	// 	fs::remove_all(dynamic_cast<logger::Default*>(&*experiment.logger())->experiment_folder().path()); // Cleanup	
	// }
	// catch (const std::exception& e)
	// {
	// 	IOH_DBG(warning,"Cannot remove directory: "
	// 		<< static_cast<std::string>(e.what()));
	// }
}
