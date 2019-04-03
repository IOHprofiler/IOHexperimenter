#' @importFrom assertthat assert_that
#' @import Rcpp
#' @useDynLib IOHexperimenter
NULL

#' Base procedure for benchmarking a custom algorithm
#'
#' @param user_alg Function defining the custom algorithm. Needs to accept two paramters:
#' The objective function (see \code{obj_func}) and an IOHproblem object, which has the following
#' properties:
#' \itemize{
#' \item dimension
#' \item function_id
#' \item instance
#' \item fopt (if known)
#' \item xopt (if known)
#' }
#' @param dimensions Which dimensions to test on
#' @param functions Which function to test on
#' @param instances Which instances to test on
#' @param algorithm.info Additional information about the algorithm you plan on running
#' @param algorithm.name The name of the algorithm you plan on running
#' @param data.dir Where the data should be stored (defaults to "./data" when not provided)
#' @param cdat Whether or not to generate a .cdat-file
#' @param idat Integer
#' @param tdat What frequency to use in a .tdat-file
#' @param params.track Which parameters to track
#'
#' @examples
#' \dontrun{
#' random_search <- function(objective, problem_info) {
#' iter <- 0
#' fopt <- -Inf
#' max_iter <- problem_info$dimension
#' while (iter < max_iter && !is_target_hit(problem_info)){
#'   candidate <- sample(c(0, 1), problem_info$dimension, TRUE)
#'   fopt <- max(objective(problem_info, candidate), fopt)
#'   iter <- iter+1
#' }
#' print(fopt)
#' }
#'
#' benchmark_algorithm(random_search)
#' }
#' @export
benchmark_algorithm <- function(user_alg, functions = NULL, instances = NULL, dimensions = NULL,
                                data.dir = NULL, algorithm.info = '', algorithm.name = '',
                                cdat = FALSE, idat = 0, tdat = 3, params.track = '') {
  # Setting default parameters if needed and verifying parameter integrity
  if (is.null(functions)) functions <- seq(2)
  else {
    assert_that( is.numeric(functions) )
    stopifnot( all( functions %in% get_problem_list() ) )
  }
  if (is.null(instances)) instances <- seq(2)
  else {
    assert_that( is.numeric(instances) )
    stopifnot( all( instances %in% seq(100) ) )
  }
  if (is.null(dimensions)) dimensions <- c(100, 300)
  else {
    assert_that( is.numeric(dimensions) )
    stopifnot( all( dimensions %in% get_dimension_list() ) )
  }
  if (is.null(data.dir)) data.dir <- './data'

  experimenter <- IOHexperimenter(dims = dimensions, functions = functions, instances = instances,
                                  algorithm_info = algorithm.info, algorithm_name = algorithm.name,
                                  data.dir = data.dir, cdat = cdat, idat = idat, tdat = tdat,
                                  param.track = params.track)
  problem_info <- next_problem(experimenter)
  print("initialized experimenter")
  while ( !is.null(problem_info) ) {
    user_alg(obj_func, problem_info)
    problem_info <- next_problem(experimenter)
  }

}

#' S3 class 'IOHexperimenter'
#'
#' @param dims Numerical
#' @param functions Numerical
#' @param instances Numerical Whether the underlying optimization algorithm performs a maximization?
#' @param algorithm_info Additional information about the algorithm you plan on running
#' @param algorithm_name The name of the algorithm you plan on running
#' @param data.dir Where the data should be stored
#' @param cdat Whether or not to generate a .cdat-file
#' @param idat Integer
#' @param tdat What frequency to use in a .tdat-file
#' @param param.track Which parameters to track
#'
#' @return A S3 object 'DataSet'
#' @export
#'
IOHexperimenter <- function(dims = c(100, 500, 1000, 2000, 3000),
  functions = seq(23), instances = seq(100), algorithm_info = '', algorithm_name = '',
  data.dir = './data', cdat = FALSE, idat = 0, tdat = 3, param.track = '') {

  assert_that(is.numeric(dims))
  assert_that(is.numeric(functions))
  assert_that(is.numeric(instances))
  base_evaluation_triggers <- 3

  # intialize the backend C code
  c_init_suite(
    paste0(functions, collapse = ','),
    paste0(dims, collapse = ','),
    paste0(instances, collapse = ',')
  )

  # intialize the observer
  c_init_observer(
    data.dir, algorithm_name, algorithm_info,
    complete_triggers = ifelse(cdat, 'true', 'false'), # @Furong: why this is a string in C?
    number_interval_triggers = idat,
		base_evaluation_triggers = "1,2,3", # @Diederick: this seems Furong's default value... no idea
		number_target_triggers = tdat,
		param.track   # @Diederick: I'm not sure about Furong's default value on this
  )

  structure(
    list(
      dims = dims,
      functions = functions,
      instances = instances
    ),
    class = c('IOHexperimenter', 'list'),
    cdat =  cdat, idat = 0, tdat = 3, param.track = param.track,
    data.dir = data.dir, C.state = T
  )
}

#' #' Constructor for S3 class 'IOHproblem'
#' #'
#' #' @param epxerimenter The current IOHexperimenter object
#' #'
#' #' @return A S3 object 'IOHproblem'
#' #' @export
#' #'
#' IOHproblem <- function(){
#'   structure(
#'     list(
#'       dimension = dims[1],
#'       function_id = functions[1],
#'       instance = instances[1],
#'       fopt = NULL,
#'       xopt = NULL,
#'       objective_function = obj_func
#'     ),
#'     class = c('IOHexperimenter', 'list'),
#'     dims = dims, functions = functions, instances = instances,
#'     cdat =  cdat, idat = 0, tdat = 3, param.track = param.track,
#'     data.dir = data.dir, C.state = T,
#'   )
#' }


print.IOHexperimenter <- function() {

}

cat.IOHexperimenter <- function() {

}


#' Evaluate the current function
#'
#' @param x point to evaluate
#' @param prob the current IOHproblem
#' @return The value of f(x) in the current problem instance
#'
#' @export
obj_func <- function(prob, x) {
  if (is.null(dim(x))) x <- t(x)
  if (ncol(x) != prob$dimension) x <- t(x)

  stopifnot(ncol(x) == prob$dimension)
  apply(x, 1, c_eval)
}

#' S3 generic summary operator for IOHexperimenter
#'
#' @param object A IOHexperimenter object
#' @param ... Arguments passed to other methods
#'
#' @return A summary of the DataSet containing both function-value and runtime based statistics.
#' @export
summary.IOHexperimenter <- function(object, ...) {
  cat('IOHexperimenter:\n')
  cat(paste('dimensions: ', paste(object$dims, collapse = ',')))
  cat(paste('functions: ', paste(object$functions, collapse = ',')))
  cat(paste('instances: ', paste(object$instances, collapse = ',')))
  cat(paste('current dimension: ', paste(object$curr_dim, collapse = ',')))
  cat(paste('current function ID: ', paste(object$curr_function_id, collapse = ',')))
  cat(paste('current instancee: ', paste(object$curr_instance, collapse = ',')))
}

#' Get the next function of the currently initialized suite
#'
#' @param experimenter The IOHexperimenter object
#'
#' @return An IOHproblem object
#' @export
next_problem <- function(experimenter) {
  ans <- c_get_next_problem()
  if (is.null(ans) || is.null(ans$problem)) return(NULL)

  return(structure(
    list(
      dimension = ans$dimension,
      function_id = ans$problem,
      instance = ans$instance,
      fopt = c_get_fopt(),
      xopt = c_get_xopt()
    ),
    class = c('IOHproblem', 'list'),
    params.track = experimenter$param.track
  ))
}

# #' Get eveluations of the current function / suite?
# #'
# #' @return something
# #' @export
# get_evaluations <- function() {
#   c_get_evaluations()
# }

#' Is the target of the current function hit
#'
#' @param problem The current IOHproblem
#'
#' @return something
#' @export
is_target_hit <- function(problem) {
  c_is_target_hit()
}






