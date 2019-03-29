#' S3 class 'IOHexperimenter'
#'
#' @param dims Numerical 
#' @param functions Numerical
#' @param instances Numerical Whether the underlying optimization algorithm performs a maximization?
#'
#' @return A S3 object 'DataSet'
#' @export
#'
IOHexperimenter <- function(dims = c(100, 500, 1000, 2000, 3000), 
  functions = seq(24), instances = seq(100), algorithm_info = '', algorithm_name = '',
  data.dir = './data', cdat = FALSE, idat = 0, tdat = 3, param.track = NULL) {
  
  assert_that(is.numeric(dims))
  assert_that(is.numeric(functions))
  assert_that(is.numeric(instances))
  base_evaluation_triggers <- 3
  
  # intialize the backend C code
  init_suite(paste0(functions, collapse = '-'), 
             paste0(dims, collapse = '-'),
             paste0(instances, collapse = '-'))
  
  # add parameters here
  init_observer()
  
  structure(
    list(
      curr_dim = dims[1],
      curr_function_id = functions[1],
      curr_instance = instances[1]
    ), 
    class = c('IOHexperimenter', 'list'),
    dims = dims, functions = functions, instances = instances,
    cdat =  cdat, idat = 0, tdat = 3, param.track = param.track,
    data.dir = data.dir
  )
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

get_function.IOHexperimenter <- function(exp, dim, function_id, instance) {
  exp$curr_dim <- dim
  exp$curr_function_id <- function_id
  exp$curr_instance <- instance
  c_set_function(function_id, dim, instance)
  
  function (x) {
    if (is.null(dim(x))) x <- t(x)
    if (ncol(x) != exp$curr_dim) x <- t(x)
    
    stopifnot(ncol(x) == exp$curr_dim)
    apply(x, 1, c_eval)
  }
}

get_next_function.IOHexperimenter <- function(exp) {
  curr_dim <- exp$curr_dim 
  curr_function_id <- exp$curr_function_id 
  curr_instance <- exp$curr_instance
  # TODO: check how to get the next function
  c_set_function(function_id, dim, instance)
  
  function (x) {
    if (is.null(dim(x))) x <- t(x)
    if (ncol(x) != exp$curr_dim) x <- t(x)
    
    stopifnot(ncol(x) == exp$curr_dim)
    apply(x, 1, c_eval)
  }
}

evaluate.IOHexperimenter <- function(exp, x) {
  function (x) {
    if (is.null(dim(x))) x <- t(x)
    if (ncol(x) != exp$curr_dim) x <- t(x)
    
    stopifnot(ncol(x) == exp$curr_dim)
    apply(x, 1, c_eval)
  }
}

 



