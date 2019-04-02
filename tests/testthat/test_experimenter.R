context("Basic IOHexperimenter functionality")

test_that("Initialization of experimenter",{
  exp <- IOHexperimenter()
  expect_equal(attr(exp,"dims"),c(100, 500, 1000, 2000, 3000))
  exp <- next_function(exp)
  expect_equal(exp$curr_dim, 100)
  expect_equal(exp$fopt, 100)
  expect_equal(obj_func(exp, seq(1,1,length.out = 100)), 100)
  expect_equal(obj_func(exp, seq(0,0,length.out = 100)), 0)
  unlink(attr(exp,"data.dir"))
})
