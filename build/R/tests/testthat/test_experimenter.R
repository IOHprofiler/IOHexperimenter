context("Basic IOHexperimenter functionality")

test_that("Initialization of experimenter (PBO)",{
  exper <- IOHexperimenter(data.dir = NULL)
  expect_equal(exper$dims,c(16, 100, 625))
  problem <- next_problem(exper)
  expect_equal(problem$dimension, 16)
  expect_equal(problem$obj_func(seq(1,1,length.out = 16)), 16)
  expect_equal(problem$obj_func(seq(0,0,length.out = 16)), 0)
})

test_that("Initialization of experimenter (BBOB)",{
  exper <- IOHexperimenter(data.dir = NULL, suite = "BBOB")
  expect_equal(exper$dims,c(5, 10))
  problem <- next_problem(exper)
  expect_equal(problem$dimension, 5)
  expect_type(problem$obj_func(seq(1,1,length.out = 5)), "double")
  expect_type(problem$obj_func(seq(-5,5,length.out = 5)), "double")
  expect_type(problem$obj_func(seq(0,0,length.out = 5)), "double")
})

test_that("Algorithms on PBO",{
  exper <- IOHexperimenter(data.dir = NULL)
  problem <- next_problem(exper)
  expect_named(IOH_random_local_search(problem), expected = c("xopt", "fopt"))
  problem <- next_problem(exper)
  expect_named(IOH_random_search(problem), expected = c("xopt", "fopt"))
  problem <- next_problem(exper)
  expect_named(IOH_self_adaptive_GA(problem), expected = c("xopt", "fopt"))
  problem <- next_problem(exper)
  expect_named(IOH_two_rate_GA(problem), expected = c("xopt", "fopt"))
})

test_that("Algorithms on BBOB",{
  exper <- IOHexperimenter(data.dir = NULL, suite = "BBOB")
  problem <- next_problem(exper)
  expect_named(IOH_random_search(problem), expected = c("xopt", "fopt"))
})

test_that("Disallow non-existing functions / dimensions / suites", {
  expect_error(IOHexperimenter(suite = "aefsdfa"))
  expect_error(IOHexperimenter(suite = "PBO", functions = c(2,23), dims = c(9,17)))
  expect_error(IOHexperimenter(suite = "PBO", functions = c(1,111)))
  expect_error(IOHexperimenter(suite = "BBOB", functions = c(123,1)))
})