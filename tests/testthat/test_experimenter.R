context("Basic IOHexperimenter functionality")

test_that("Initialization of experimenter",{
  exper <- IOHexperimenter(data.dir = NULL)
  expect_equal(exper$dims,c(100, 500, 1000, 2000, 3000))
  problem <- next_problem(exper)
  expect_equal(problem$dimension, 100)
  expect_equal(problem$fopt, 100)
  expect_equal(problem$obj_func(seq(1,1,length.out = 100)), 100)
  expect_equal(problem$obj_func(seq(0,0,length.out = 100)), 0)
})
