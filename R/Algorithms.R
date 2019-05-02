#' IOHexperimenter-based wrapper 
#' 
#' For easier use with the IOHexperimenter
#'
#' @rdname random_search
#' @param IOHproblem An IOHproblem object
#'
#' @export
#' @examples 
#' \donttest{
#' benchmark_algorithm(IOH_random_search, data.dir = NULL)
#' }
IOH_random_search <- function(IOHproblem, budget = NULL) {
  random_search(IOHproblem$dimension, IOHproblem$obj_func, IOHproblem$fopt, budget)
}

#' IOHexperimenter-based wrapper 
#' 
#' For easier use with the IOHexperimenter
#'
#' @param IOHproblem An IOHproblem object
#' 
#' @rdname random_local_search
#' @export
#' @examples 
#' \donttest{
#' benchmark_algorithm(IOH_random_local_search, data.dir = NULL)
#' }
IOH_random_local_search <- function(IOHproblem, budget = NULL) {
  random_local_search(IOHproblem$dimension, IOHproblem$obj_func, IOHproblem$fopt, budget) 
}

#' IOHexperimenter-based wrapper 
#' 
#' For easier use with the IOHexperimenter
#'
#' @param IOHproblem An IOHproblem object
#' 
#' @rdname self_adaptive_GA
#' @export
#' @examples 
#' \donttest{
#' one_comma_two_EA <- function(IOHproblem) { IOH_self_adaptive_GA(IOHproblem, lambda_=2) }
#'
#' benchmark_algorithm(one_comma_two_EA, params.track = "Mutation rate",
#' algorithm.name = "one_comma_two_EA", data.dir = NULL,
#' algorithm.info = "Using one_comma_two_EA with specific parameter" )
#' }
IOH_self_adaptive_GA <- function(IOHproblem, lambda_ = 1, budget = NULL) {
  self_adaptive_GA(IOHproblem$dimension, IOHproblem$obj_func, 
                     target = IOHproblem$fopt, budget = budget,
                     lambda_ = lambda_, set_parameters = IOHproblem$set_parameters) 
}

#' IOHexperimenter-based wrapper 
#' 
#' For easier use with the IOHexperimenter
#'
#' @param IOHproblem An IOHproblem object
#' 
#' @rdname two_rate_GA
#' @export
#' @examples 
#' \donttest{
#' bechmark_algorithm(IOH_two_rate_GA)
#' }
IOH_two_rate_GA <- function(IOHproblem, lambda_ = 1, budget = NULL) {
  two_rate_GA(IOHproblem$dimension, IOHproblem$obj_func, 
                   target = IOHproblem$fopt, budget = budget,
                   lambda_ = lambda_, set_parameters = IOHproblem$set_parameters) 
}

#' Random Search
#'
#' Random walk in \eqn{{0, 1}^d} space
#' @param dim Dimension of search space
#' @param obj_func The evaluation function
#' @param target Optional, enables early stopping if this value is reached
#' @param budget integer, maximal allowable number of function evaluations
#' 
#' @export
random_search <- function(dim, obj_func, target = NULL, budget = NULL) {
  if (is.null(budget)) budget <- 10 * dim
  fopt <- -Inf
  xopt <- NULL
  
  target_hit <- function() {
    if (is.null(target)) return(FALSE)
    else return (target<=fopt)
  }
  
  while (budget > 0 && !target_hit()) {
    x <- sample(c(0, 1), dim, TRUE)
    f <- obj_func(x)
    budget <- budget - 1
    
    if (f > fopt) {
      xopt <- x
      fopt <- f
    }
  }
  list(xopt = xopt, fopt = fopt)
}


#' Random Local Search (RLS) Algorithm
#'
#' The simplest stochastic optimization algorithm for discrete problems. A randomly
#' chosen position in the solution vector is perturbated in each iteration. Only
#' improvements are accepted after perturbation.
#'
#'
#' @param dimension Dimension of search space
#' @param obj_func The evaluation function
#' @param target Optional, enables early stopping if this value is reached
#' @param budget integer, maximal allowable number of function evaluations
#' 
#' @export
random_local_search <- function(dimension, obj_func, target = NULL, budget = NULL) {
  if (is.null(budget)) budget <- 10*dimension
  starting_point <- sample(c(0, 1), dimension, TRUE)
  fopt <- obj_func(starting_point)
  xopt <- starting_point
  iter <- 1
  
  target_hit <- function() {
    if (is.null(target)) return(FALSE)
    else return (target<=fopt)
  }
  
  while ( iter < budget && !target_hit() ){
    candidate <- xopt
    switch_idx <- sample(1:dimension, 1)
    candidate[switch_idx] <- 1 - candidate[switch_idx]
    fval <- obj_func(candidate)
    if (fval >= fopt){
      fopt <- fval
      xopt <- candidate
    }
    iter <- iter+1
  }
  list(xopt = xopt, fopt = fopt)
}


#' Mutation operator for 1+lambda EA
#'
#'
#' @param ind The individual to mutate
#' @param mutation_rate The mutation rate
#' @noRd
mutate <- function(ind, mutation_rate){
  dim <- length(ind)
  mutations <- seq(0, 0, length.out = dim)
  while (sum(mutations) == 0){
    mutations <- sample(c(0, 1), dim, prob = c(1-mutation_rate, mutation_rate), replace = TRUE)
  }
  as.integer( xor(ind, mutations) )
}


#' One-Comma-Lambda Self-Adapative Genetic Algorithm
#'
#' A genetic algorithm that controls the mutation rate (strength) using the so-called
#' self-adaptation mechanism: the mutation rate is firstly perturbated and then the
#' resulting value is taken to mutate Lambda solution vector. The best solution is
#' selected along with its mutation rate.
#'
#' @param lambda_ The size of the offspring
#' @param budget How many times the objective function can be evaluated
#' @param dimension Dimension of search space
#' @param obj_func The evaluation function
#' @param target Optional, enables early stopping if this value is reached
#' @param set_parameters Function to call to store the value of the registered parameters
#' 
#' @export
self_adaptive_GA <- function(dimension, obj_func, target = NULL, lambda_ = 10, budget = NULL,
                             set_parameters = NULL) {
  obj_func <- obj_func
  if (is.null(budget)) budget <- 10 * dimension

  r <- 1.0 / dim
  if (is.function(set_parameters)) set_parameters(r)

  x <- sample(c(0, 1), dimension, TRUE)
  xopt <- x
  fopt <- fx <- obj_func(x)
  budget <- budget - 1

  tau <- 0.22

  target_hit <- function() {
    if (is.null(target)) return(FALSE)
    else return (target<=fopt)
  }
  
  while (budget > 0 && !target_hit()) {
    lambda_ <- min(lambda_, budget) #ensure budget is not exceeded
    x_ <- tcrossprod(rep(1, lambda_), x)
    r_ <- (1.0 / (1 + (1 - r) / r * exp(tau * rnorm(lambda_))))  %*% t(rep(1, dimension))
    idx <- matrix(runif(lambda_ * dimension), lambda_, dimension) < r_
    x_[idx] <- 1 - x_[idx]

    if (is.function(set_parameters)) set_parameters(r)
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

#' One-Comma-Lambda Genetic Algorithm with 2-rate self-adaptive mutation rate
#' 
#' A genetic algorithm that controls the mutation rate (strength) using the so-called 
#' 2-rate self-adaptation mechanism: the mutation rate is based on a parameter r. For
#' each generation, half offspring are generated by mutation rate 2r/dim, and half by 
#' r/2dim. r that the best offspring has been created with will be inherited by 
#' probability 3/4, the other by 1/4.
#'
#' @param lambda_ The size of the offspring
#' @param budget How many times the objective function can be evaluated
#' @param dimension Dimension of search space
#' @param obj_func The evaluation function
#' @param target Optional, enables early stopping if this value is reached
#' @param set_parameters Function to call to store the value of the registered parameters
#' 
#' @export
two_rate_GA <- function(dimension, obj_func, target = NULL, lambda_ = 2, budget = NULL, set_parameters = NULL){
  if (is.null(budget)) budget <- 100*dimension

  parent <- sample(c(0, 1), dimension, TRUE)
  best <- parent
  r <- 2.0
  fopt <- obj_func(parent)
  budget <- budget-1
  if (is.function(set_parameters)) set_parameters(r)
  
  target_hit <- function() {
    if (is.null(target)) return(FALSE)
    else return (target<=fopt)
  }
  
  while (budget > 0 && !target_hit()) {
    selected_r <- r;
    selected_obj <- -Inf

    for (i in 1:lambda_){
      offspring <- parent

      if(i <= lambda_/2){
        mutation_rate = r / 2.0 / dimension;
      } else{
        mutation_rate = 2.0 * r / dimension;
      }
      offspring <- mutate(offspring, mutation_rate)

      v <- obj_func(offspring)
      if(v >= fopt){
        fopt <- v
        best <- offspring
      }
      if(v >= selected_obj){
        selected_obj = v
        selected_r = mutation_rate * dimension;
      }
      budget <- budget - 1
      if (budget == 0 ) break
    }
    parent <- best
    if(runif(1) > 0.5){
      r = selected_r
    }
    else{
      if(runif(1) > 0.5){
        r = r / 2.0
      } else{
        r = 2.0 * r
      }
    }

    if(r < 2.0) r = 2.0
    if(r > dimension / 4.0) r = dimension / 4.0
    if (is.function(set_parameters)) set_parameters(r)
    
  }
  list(xopt = best, fopt = fopt)
}
