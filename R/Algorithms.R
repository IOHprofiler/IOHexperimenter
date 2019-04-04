#' A random_search implementation
#'
#' This implements a bare-bones random search algorithm.
#' This serves as an example of how to implement an optimization algorithm
#' for use with the IOHexperimenter.
#'
#'
#' @param IOHproblem An IOHproblem object
#'
#' @export
random_search <- function(IOHproblem) {
  iter <- 0
  max_iter <- IOHproblem$dimension
  while ( iter < max_iter && !IOHproblem$target_hit() ){
    candidate <- sample(c(0, 1), IOHproblem$dimension, TRUE)
    IOHproblem$store_parameters(0)
    IOHproblem$obj_func(candidate)
    iter <- iter+1
  }
}

#' Mutation operator for 1+lambda EA
#'
#'
#' @param ind The individual to mutate
#' @param mutation_rate The mutation rate
#'
mutate <- function(ind, mutation_rate){
  dim <- length(ind)
  mutations <- seq(0, 0, length.out = dim)
  while (sum(mutations) == 0){
    mutations <- sample(c(0, 1), dim, prob = c(1-mutation_rate, mutation_rate), replace = TRUE)
  }
  as.integer( xor(ind, mutations) )
}

#' A (1+lambda) EA implementation
#'
#' @param IOHproblem An IOHproblem object
#' @param lambda_ The size of the offspring
#' @param budget How many times the objective function can be evaluated
#'
#' @export
#' @examples
#' \donttest{
#' one_plus_two_EA <- function(IOHproblem) { one_plus_lambda_EA(IOHproblem, lambda_=2) }
#'
#' benchmark_algorithm(one_plut_two_EA, params.track = "Mutation rate",
#' algorithm.name = "one_plus_two_EA",
#' algorithm.info = "Using one_plus_lambda_EA with specific parameter" )
#' }
one_plus_lambda_EA <- function(IOHproblem, lambda_ = 1, budget = NULL) {
  dim = IOHproblem$dimension
  if ( is.null(budget) ) budget <- 10*dim
  obj <- IOHproblem$obj_func

  parent <- sample(c(0, 1), dim, TRUE)
  best <- parent
  mutation_rate <- 1.0/dim
  IOHproblem$set_parameters(mutation_rate)
  best_value <- obj(parent)
  budget <- budget-1

  while ( budget > 0 && !IOHproblem$taget_hit() ){
    for (i in 1:lambda_){
      offspring <- parent
      offspring <- mutate(offspring, mutation_rate)
      v <- obj(offspring)
      if(v > best_value){
        best_value <- v
        best <- offspring
      }
      budget <- budget - 1
      if (budget == 0 ) break
    }
    parent <- best
    mutation_rate = 1.0 / (1 + (1 - mutation_rate) / mutation_rate * exp(0.22 * rnorm(1)))
    mutation_rate = min(max(mutation_rate, 1.0/dim), 0.5)
    IOHproblem$set_parameters(mutation_rate)
  }
  return(best_value)
}
