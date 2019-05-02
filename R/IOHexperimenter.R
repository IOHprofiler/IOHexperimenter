#' @importFrom assertthat assert_that
#' @import Rcpp
#' @importFrom stats rnorm runif
#' @importFrom magrittr %>%
#' @useDynLib IOHexperimenter
NULL

utils::globalVariables(c("."))

#' S3 class 'IOHexperimenter'
#'
#' @param dims Numerical
#' @param functions Numerical
#' @param instances Numerical Whether the underlying optimization algorithm performs a maximization?
#' @param algorithm.info Additional information about the algorithm you plan on running
#' @param algorithm.name The name of the algorithm you plan on running
#' @param data.dir Where the data should be stored
#' @param cdat Whether or not to generate a .cdat-file
#' @param idat Integer
#' @param tdat What frequency to use in a .tdat-file
#' @param param.track Which parameters to track. Should be a vector of strings, containing no spaces or commas
#'
#' @return A S3 object 'DataSet'
#' @export
#' @examples 
#' exp <- IOHexperimenter()
IOHexperimenter <- function(dims = c(16, 100, 625, 2500),
  functions = seq(23), instances = seq(100), algorithm.info = ' ', algorithm.name = ' ',
  data.dir = './data', cdat = FALSE, idat = 0, tdat = 3, param.track = NULL) {

  stopifnot({
    instances %in% seq(100) %>%
      c(., functions %in% seq(23)) %>%
      c(., dims %in% if(any(functions == 23)) (seq(50)^2) else seq(50^2)) %>% 
      all
  })
  
  if (is.null(data.dir)) {
    observer_name <- "no_observer"
    data.dir <- "N/A"
  }
  else observer_name <- "PBO"
  if (!is.null(param.track)) param_str <- paste0(param.track, collapse = ',')
  else param_str <- ''

  # intialize the backend C code
  c_init_suite(
    paste0(functions, collapse = ','),
    paste0(dims, collapse = ','),
    paste0(instances, collapse = ',')
  )
  
  if (algorithm.info == '') algorithm.info <- "N/A"
  else algorithm.info <- gsub(" ", "_", algorithm.info)
  if (algorithm.name == '') algorithm.name <- "N/A"
  else algorithm.name <- gsub(" ", "_", algorithm.name)
  
  # intialize the observer
  c_init_observer(
    data.dir, algorithm.name, algorithm.info,
    complete_triggers = ifelse(cdat, 'true', 'false'),
    number_interval_triggers = idat,
		base_evaluation_triggers = "1,2,5",
		number_target_triggers = tdat,
		parameters_name = param_str,
		observer_name = observer_name
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
#' @examples 
#' print(IOHexperimenter())
print.IOHexperimenter <- function(x, ...) {
  cat(as.character.IOHexperimenter(x, ...))
}

#' S3 generic as.character function for IOHexperimenter
#'
#' @param x The IOHexperimenter to print
#' @param ... Arguments for underlying function
#'
#' @export
#' @examples 
#' as.character(IOHexperimenter())
as.character.IOHexperimenter <- function(x, ...) {
  paste0(
    sprintf('IOHexperimenter (%d instances, %d functions, %d dimensions)\n', 
              length(x$instances), length(x$functions), length(x$dims)),
    sprintf('Dimensions: %s\n', paste0(x$dims, collapse = ', ')),
    sprintf('Functions: %s\n', paste0(x$functions, collapse = ', ')),
    sprintf('Instances: %s\n', paste0(x$instances, collapse = ', '))
  )
}

#' S3 generic summary operator for IOHexperimenter
#'
#' @param object A IOHexperimenter object
#' @param ... Arguments passed to other methods
#'
#' @return A summary of the DataSet containing both function-value and runtime based statistics.
#' @export
#' @examples 
#' summary(IOHexperimenter())
summary.IOHexperimenter <- function(object, ...) {
  cat('IOHexperimenter:\n')
  cat(paste('dimensions: ', paste(object$dims, collapse = ',')))
  cat(paste('functions: ', paste(object$functions, collapse = ',')))
  cat(paste('instances: ', paste(object$instances, collapse = ',')))
}
