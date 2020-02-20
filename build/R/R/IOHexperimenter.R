#' @importFrom assertthat assert_that
#' @import Rcpp
#' @importFrom stats rnorm runif
#' @importFrom magrittr %>%
#' @useDynLib IOHexperimenter
NULL

utils::globalVariables(c("."))


# Options being set upon loading of library
.onLoad <- function(libname, pkgname) {
  op <- options()
  op.IOHexperimenter <- list(
    IOHexperimenter.idat = 0,
    IOHexperimenter.tdat = 0,
    IOHexperimenter.cdat = FALSE,
    IOHexperimenter.dat = TRUE
  )
  toset <- !(names(op.IOHexperimenter) %in% names(op))
  if (any(toset)) options(op.IOHexperimenter[toset])
  
  invisible()
}

#' S3 class 'IOHexperimenter'
#'
#' @param suite Which suite to use. Available: 'PBO', 'BBOB'
#' @param dims Numerical Which dimensions to use
#' @param functions Numerical Which functions from the selected suite to use
#' @param instances Numerical Which problem instances to use
#' @param algorithm.info Additional information about the algorithm you plan on running
#' @param algorithm.name The name of the algorithm you plan on running
#' @param data.dir Where the data should be stored. Defaults to NULL, meaning no data is stored.
#' @param param.track Which parameters to track. Should be a vector of strings, containing no spaces or commas
#'
#' @return A S3 object 'IOHexperimenter'
#' @export
#' @examples 
#' exp <- IOHexperimenter()
IOHexperimenter <- function(suite = "PBO", dims = NULL, functions = NULL, instances = NULL,
                            algorithm.info = ' ', algorithm.name = ' ',
                            data.dir = NULL, param.track = NULL) {
  
  if (suite == "PBO") {
    #set default values
    if (is.null(dims)) dims <- c(16, 100, 625)
    if (is.null(functions)) functions <- seq(23)
    if (is.null(instances)) instances <- seq(5)
    
    #check validity of parameters
    stopifnot({
      instances %in% seq(100) %>%
        c(., functions %in% seq(23)) %>%
        c(., dims %in% if (any(functions == 23)) (seq(50)^2) else seq(50^2)) %>% 
        all
    })
  }
  else if (suite == "BBOB") {
    #set default values
    if (is.null(dims)) dims <- c(5, 10)
    if (is.null(functions)) functions <- seq(24)
    if (is.null(instances)) instances <- seq(5)
    
    #check validity of parameters
    stopifnot({
      instances %in% seq(100) %>%
        c(., functions %in% seq(24)) %>%
        c(., dims %in% seq(40)) %>% 
        all
    })
  }
  else stop("Requested suite is not implemented")
  
  # intialize the backend C code
  cpp_init_suite(
    suite, functions, instances, dims
  )
  
  if (algorithm.info == '') algorithm.info <- "N/A"
  else algorithm.info <- gsub(" ", "_", algorithm.info)
  if (algorithm.name == '') algorithm.name <- "N/A"
  else algorithm.name <- gsub(" ", "_", algorithm.name)
  
  if (is.null(data.dir)) {
    observed <- FALSE
  }
  else {
    if (!dir.exists(dirname(data.dir)))
      dir.create(dirname(data.dir), recursive = TRUE)
    # intialize the observer
    cpp_init_logger(
      dirname(data.dir), basename(data.dir), algorithm.name, algorithm.info,
      getOption('IOHexperimenter.dat', default = TRUE), 
      getOption('IOHexperimenter.cdat', default = FALSE),
      getOption('IOHexperimenter.tdat', default = 0),
      getOption('IOHexperimenter.idat', default = 0)
    )
    
    # cpp_logger_set_state(getOption(IOHexperimenter.dat, default = TRUE), 
    #                      getOption(IOHexperimenter.cdat, default = FALSE),
    #                      getOption(IOHexperimenter.tdat, default = 0),
    #                      getOption(IOHexperimenter.idat, default = 0))
    
    cpp_logger_target_suite()
    
    if (!is.null(param.track))
      cpp_set_parameters_name(param.track)
    
    observed <- TRUE
  }

  
  structure(
    list(
      dims = dims,
      functions = functions,
      instances = instances,
      suite = suite,
      param.track = param.track,
      observed = observed
    ),
    class = c('IOHexperimenter', 'list'),
    data.dir = data.dir
  )
}

#' S3 print function for IOHexperimenter
#'
#' @param x The IOHexperimenter to print
#' @param ... Arguments for underlying function
#' @return The printed object
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
#' @return The printed object
#' @export
#' @examples 
#' as.character(IOHexperimenter())
as.character.IOHexperimenter <- function(x, ...) {
  paste0(
    sprintf('IOHexperimenter (Suite %s: %d instances, %d functions, %d dimensions)\n', 
              x$suite, length(x$instances), length(x$functions), length(x$dims)),
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
#' @return A summary of the IOHexperimenter object.
#' @export
#' @examples 
#' summary(IOHexperimenter())
summary.IOHexperimenter <- function(object, ...) {
  cat('IOHexperimenter:\n')
  cat(paste('dimensions: ', paste(object$dims, collapse = ',')))
  cat(paste('functions: ', paste(object$functions, collapse = ',')))
  cat(paste('instances: ', paste(object$instances, collapse = ',')))
}
