#include "../utils.hpp" 

#include "ioh.hpp"

void real_random_search(const std::shared_ptr<ioh::problem::Real>& p)
{
	for (int i = 0; i < 10; i++)
		(*p)(ioh::common::random::doubles(p->meta_data().n_variables));
}

void integer_random_search(const std::shared_ptr<ioh::problem::Integer>& p)
{
	for (int i = 0; i < 10; i++)
		(*p)(ioh::common::random::integers(p->meta_data().n_variables));
}


TEST_F(BaseTest, experiment_bbob)
{
	using namespace ioh;

	const std::vector<int> pbs = {1, 2};
	const std::vector<int> ins = {1, 2};
	const std::vector<int> dims = {2, 10};
	
	const auto suite  = std::make_shared<suite::BBOB>(pbs, ins, dims);
	const auto logger = std::make_shared<logger::Store>(
		logger::Triggers{trigger::always}, logger::Properties{watch::raw_y_best});
	auto experiment   = Experimenter<problem::Real>(suite, logger, real_random_search, 10);

	EXPECT_EQ(experiment.independent_runs(), 10);
	experiment.run();

	auto data = logger->data();
	std::set<int> pr;
	std::set<int> di;
	std::set<int> ii;

	for (const auto& [problem, dimensions]: data.at("BBOB")){
		pr.insert(problem);
		for (const auto& [dimension, instances]: dimensions){
			di.insert(dimension);
			for (const auto& [instance, runs]: instances){
				ii.insert(instance);
				EXPECT_EQ(runs.size(), 10);
			}
		}
	}
	EXPECT_EQ(pr, (std::set<int>{1, 2}));
	EXPECT_EQ(ii, (std::set<int>{1, 2}));
	EXPECT_EQ(di, (std::set<int>{2, 10}));
}
