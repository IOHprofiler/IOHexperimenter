# library(IOHexperimenter)

random_search <- function(objective, max_iter, exper) {
  iter <- 0
  fopt <- -Inf
  while (iter < max_iter && !is_target_hit()){
    candidate <- sample(c(0, 1), exper$curr_dim, TRUE)
    fopt <- max(objective(exper, candidate), fopt)
    iter <- iter+1
  }
  print(fopt)
}

run_example <- function(){
  print("Starting")
  exper <- IOHexperimenter(instances = c(1), functions = c(1,2), dims = c(100))
  exper <- next_function(exper)
  print("initialized experimenter")
  while ( !is.null(exper) ) {
    print("running search")
    random_search(obj_func, exper$curr_dim, exper)
    print("initialize new function")
    exper <- next_function(exper)

  }
}

run_example2 <- function(){
  print("Starting")
  exper <- IOHexperimenter(instances = c(1,2,3), functions = c(1,2,3), dims = c(100))
  exper <- next_function(exper)
  print("initialized experimenter")
  for (i in 1:8) {
    print("running search")
    random_search(obj_func, exper$curr_dim, exper)
    print("initialize new function")
    exper <- next_function(exper)

  }
}

