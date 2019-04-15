#' Random Search
#'
#' Random walk in \eqn{{0, 1}^d} space
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


#' Random Local Search (RLS) Algorithm
#'
#' The simplest stochastic optimization algorithm for discrete problems. A randomly
#' chosen position in the solution vector is perturbated in each iteration. Only
#' improvements are accepted after perturbation.
#'
#'
#' @param IOHproblem An IOHproblem object
#' @param budget integer, maximal allowable number of function evaluations
#'
#' @export
random_local_search <- function(IOHproblem, budget = NULL) {
  if (is.null(budget)) budget <- 10*IOHproblem$dimension
  starting_point <- sample(c(0, 1), IOHproblem$dimension, TRUE)
  fopt <- IOHproblem$obj_func(starting_point)
  xopt <- starting_point
  iter <- 1
  while ( iter < budget && !IOHproblem$target_hit() ){
    candidate <- xopt
    switch_idx <- sample(1:IOHproblem$dimension, 1)
    candidate[switch_idx] <- 1 - candidate[switch_idx]
    fval <- IOHproblem$obj_func(candidate)
    if (fval >= fopt){
      fopt <- fval
      xopt <- candidate
    }
    iter <- iter+1
  }
  return(fopt)
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

  while ( budget > 0 && !IOHproblem$target_hit() ){
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


#' One-Comma-Lambda Self-Adapative Genetic Algorithm
#'
#' A genetic algorithm that controls the mutation rate (strength) using the so-called
#' self-adaptation mechanism: the mutation rate is firstly perturbated and then the
#' resulting value is taken to mutate Lambda solution vector. The best solution is
#' selected along with its mutation rate.
#'
#' @param IOHproblem An IOHproblem object
#' @param budget integer, maximal allowable number of function evaluations
#' @param lambda_ integer, the population size > 1
#'
#' @export
self_adaptive_GA <- function(IOHproblem, budget = NULL, lambda_ = 10) {
  dim <- IOHproblem$dimension
  obj_func <- IOHproblem$obj_func
  if (is.null(budget)) budget <- 10 * dim

  r <- 1.0 / dim
  IOHproblem$set_parameters(r)

  x <- sample(c(0, 1), dim, TRUE)
  xopt <- x
  fopt <- fx <- obj_func(x)
  budget <- budget - 1

  tau <- 0.22

  while (budget > 0 && !IOHproblem$target_hit()) {
    lambda_ <- min(lambda_, budget) #ensure budget is not exceeded
    x_ <- tcrossprod(rep(1, lambda_), x)
    r_ <- (1.0 / (1 + (1 - r) / r * exp(tau * rnorm(lambda_))))  %*% t(rep(1, dim))
    idx <- matrix(runif(lambda_ * dim), lambda_, dim) < r_
    x_[idx] <- 1 - x_[idx]

    IOHproblem$set_parameters(r)
    f <- obj_func(x_)
    budget <- budget - lambda_
    selected <- which(min(f) == f)[[1]]
    x <- x_[selected, ]
    r <- r_[selected, 1]
    if (f[selected] > fopt) {
      fopt <- f[selected]
      xopt <- x
    }
  }
  list(xopt = xopt, fopt = fopt)
}
