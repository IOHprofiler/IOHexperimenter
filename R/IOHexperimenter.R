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
  functions = seq(23), instances = seq(100), algorithm_info = '', algorithm_name = '',
  data.dir = './data', cdat = FALSE, idat = 0, tdat = 3, param.track = '') {
  
  assert_that(is.numeric(dims))
  assert_that(is.numeric(functions))
  assert_that(is.numeric(instances))
  base_evaluation_triggers <- 3
  
  # intialize the backend C code
  c_init_suite(
    paste0(functions, collapse = '-'), 
    paste0(dims, collapse = '-'),
    paste0(instances, collapse = '-')
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
      obj_func = function(x) {
        if (is.null(dim(x))) x <- t(x)
        if (ncol(x) != exp$curr_dim) x <- t(x)
        
        stopifnot(ncol(x) == exp$curr_dim)
        apply(x, 1, c_eval)
      },
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

next_function <- function(exp) {
  ans <- c_get_next_problem()
  if (is.null(ans)) return(NULL)
  
  exp$fopt <- c_get_fopt()
  exp$xopt <- c_get_xopt()
  exp$curr_dim <- ans$dimension 
  exp$curr_function_id <- ans$problem 
  exp$curr_instance <- exp$instance
  exp
}

get_evaluations <- function() {
  c_get_evaluations()
}

is_target_hit <- function() {
  c_is_target_hit()
}


# exmaple testing case
exp <- IOHexperimenter()
while (T) {
  exp <- next_function(exp)
  if (is.null(exp)) break

  dim <- exp$curr_dim
  exp$obj_func(runif(dim))
}


 



