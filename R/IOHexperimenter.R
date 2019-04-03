#' @importFrom assertthat assert_that
#' @import Rcpp
#' @useDynLib IOHexperimenter
NULL

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
  IOHfree()

  if (dir.exists(data.dir)) {
    data.dir <- tempdir()
    print(data.dir)
  }

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
      curr_dim = dims[1],
      curr_function_id = functions[1],
      curr_instance = instances[1],
      fopt = NULL,
      xopt = NULL
    ),
    class = c('IOHexperimenter', 'list'),
    dims = dims, functions = functions, instances = instances,
    cdat =  cdat, idat = 0, tdat = 3, param.track = param.track,
    data.dir = data.dir, C.state = T
  )
}

print.IOHexperimenter <- function() {

}

cat.IOHexperimenter <- function() {

}


#' Evaluate the current function
#'
#' @param x point to evaluate
#' @param exp the current IOHexperimenter
#'
#' @export
obj_func <- function(exp, x) {
  if (is.null(dim(x))) x <- t(x)
  if (ncol(x) != exp$curr_dim) x <- t(x)

  stopifnot(ncol(x) == exp$curr_dim)
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

# we don't need this fuction for now...
# set_function <- function(exp, dim, function_id, instance) {
#   exp$curr_dim <- dim
#   exp$curr_function_id <- function_id
#   exp$curr_instance <- instance
#   c_set_function(function_id, dim, instance)
# }

#' Get the next function of the currently initialized suite
#'
#' @param exp Something?
#'
#' @return Something?
#' @export
next_function <- function(exp) {
  ans <- c_get_next_problem()
  if (is.null(ans) || is.null(ans$problem)) return(NULL)

  exp$fopt <- c_get_fopt()
  exp$xopt <- c_get_xopt()
  exp$curr_dim <- ans$dimension
  exp$curr_function_id <- ans$problem
  exp$curr_instance <- exp$instance
  exp
}

#' Get eveluations of the current function / suite?
#'
#' @return something
#' @export
get_evaluations <- function() {
  c_get_evaluations()
}

#' Is the target of the current function hit
#'
#'
#' @return something
#' @export
is_target_hit <- function() {
  c_is_target_hit()
}






