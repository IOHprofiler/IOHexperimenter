#' @importFrom assertthat assert_that
#' @import Rcpp
#' @importFrom stats rnorm
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
#' @param param.track Which parameters to track. Should be a vector of strings, containing no spaces or commas
#'
#' @return A S3 object 'DataSet'
#' @export
#'
IOHexperimenter <- function(dims = c(100, 500, 1000, 2000, 3000),
  functions = seq(23), instances = seq(100), algorithm_info = '', algorithm_name = '',
  data.dir = './data', cdat = FALSE, idat = 0, tdat = 3, param.track = NULL) {

  assert_that(is.numeric(dims))
  assert_that(is.numeric(functions))
  assert_that(is.numeric(instances))
  base_evaluation_triggers <- 3
  if( !is.null(param.track) ) param_str <- paste0(param.track, collapse = ',')
  else param_str <- ''


  # intialize the backend C code
  c_init_suite(
    paste0(functions, collapse = ','),
    paste0(dims, collapse = ','),
    paste0(instances, collapse = ',')
  )

  # intialize the observer
  c_init_observer(
    data.dir, algorithm_name, algorithm_info,
    complete_triggers = ifelse(cdat, 'true', 'false'),
    number_interval_triggers = idat,
		base_evaluation_triggers = "1,2,5",
		number_target_triggers = tdat,
		param_str
  )

  structure(
    list(
      dims = dims,
      functions = functions,
      instances = instances,
      param.track = param.track
    ),
    class = c('IOHexperimenter', 'list'),
    cdat =  cdat, idat = 0, tdat = 3,
    data.dir = data.dir, C.state = T
  )
}

#' S3 print function for IOHexperimenter
#'
#' @param x The IOHexperimenter to print
#' @param ... Arguments for underlying function
#'
#' @export
print.IOHexperimenter <- function(x, ...) {
  cat(as.character.IOHexperimenter(x, ...))
}

#' S3 generic cat function for IOHexperimenter
#'
#' @param x The IOHexperimenter to print
#'
#' @export
cat.IOHexperimenter <- function(x) cat(as.character(x))

#' S3 generic as.character function for IOHexperimenter
#'
#' @param x The IOHexperimenter to print
#' @param ... Arguments for underlying function
#'
#' @export
as.character.IOHexperimenter <- function(x, ...) {
  sprintf('IOHexperimenter (Instances %d of functions %d %dD)', x$instances, x$functions, x$dims)
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
}









