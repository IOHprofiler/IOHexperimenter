context("Basic IOHexperimenter functionality")

test_that("Initialization of experimenter",{
  exper <- IOHexperimenter(data.dir = NULL)
  expect_equal(exper$dims,c(16, 100, 625, 2500))
  problem <- next_problem(exper)
  expect_equal(problem$dimension, 16)
  expect_equal(problem$fopt, 16)
  expect_equal(problem$obj_func(seq(1,1,length.out = 16)), 16)
  expect_equal(problem$obj_func(seq(0,0,length.out = 16)), 0)
})
