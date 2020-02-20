#' S3 print function for IOHproblem
#'
#' @param x The IOHproblem to print
#' @param ... Arguments for underlying function
#' @return The printed object
#' @export
#' @examples 
#' exp <- IOHexperimenter()
#' p <- next_problem(exp)
#' print(p)
print.IOHproblem <- function(x, ...) {
  cat(as.character.IOHproblem(x, ...))
}

#' S3 generic as.character function for IOHproblem
#'
#' @param x The IOHproblem to print
#' @param ... Arguments for underlying function
#' @return The printed object
#' @export
#' @examples 
#' exp <- IOHexperimenter()
#' p <- next_problem(exp)
#' as.character(p)
as.character.IOHproblem <- function(x, ...) {
  sprintf('IOHproblem (Suite %s: Instance %d of function %s %dD)\n', x$suite, x$instance, 
          x$function_id, x$dimension)
}

#' get an IOHproblem objects
#' 
#' If an `experimenter`-argument is provided, this is the same function as `next_problem`
#' If not, this creates a suite consisting of a single function based on the other arguments
#' 
#' An IOHproblem-object has the following attributes:
#' \itemize{
#'  \item{"Dimension": }{The dimesion of the problem}
#'  \item{"function_id: }{The number of the function}
#'  \item{"instance: }{The number of the function-instance}
#'  \item{suite: }{The suite of the function. Either 'PBO' or 'BBOB'}
#'  \item{fopt: }{If known, the optimal value of the function}
#'  \item{lbound: }{The lower bound of the searchspace}
#'  \item{ubound: }{The upper bound of the searchspace}
#'  \item{maximization: }{Boolean indicating whether the function should be maximized or minimized}
#'  \item{params.track: }{The parameters which are being tracked on this function. Only available if initialized
#'  in the underlying IOHexperimenter-object (or when using the `benchmark_algorithm`-function)}
#' }
#' In addition to these attributes, there are three function-attributes available to use:
#' \itemize{
#'  \item{obj_function: }{The interface to evaluate the function}
#'  \item{target_hit: }{Boolean indicating if the optimal has been hit (if known)}
#'  \item{set_parameters: }{Interface to storing the current parameter values (if param.track is initialized)}
#' }
#' 
#' @param experimenter (optional) an IOHexperimenter object
#' @param suite The suite to use. Either 'PBO' or 'BBOB'
#' @param functionnr The number of the function to create
#' @param instance The instance of the function to create
#' @param dim The dimension of the function to create
#' 
#' @return An IOHproblem object
#' @export
#' @examples
#' p <- IOHproblem()
IOHproblem <- function(suite = "PBO", functionnr = 1, dim = 16, instance = 1, experimenter = NULL){
  if (is.null(experimenter)) {
    experimenter <- IOHexperimenter(suite, dim, functionnr, instance)
  }
  next_problem(experimenter)
}

#' Get the next function of the currently initialized IOHexperimenter object
#'
#' @param experimenter The IOHexperimenter object
#'
#' @return An IOHproblem object if available, NULL otherwise
#' @export
#' @examples 
#' exp <- IOHexperimenter()
#' p <- next_problem(exp)
next_problem <- function(experimenter) {
  if (!"IOHexperimenter" %in% class(experimenter))
    stop("Please ensure a valid IOHexperimenter object is provided!")
  
  ans <- cpp_get_next_problem()
  if (is.null(ans) || is.null(ans$problem)) return(NULL)

  if (experimenter$suite == "PBO") {
    internal_eval <- function(x) {
      stopifnot( all(x %in% c(0,1) ))
      f <- cpp_int_evaluate(x)
      if (experimenter$observed)
        cpp_write_line(cpp_loggerInfo())
      return(f)
    }
  }
  else{
    internal_eval <- function(x) {
      f <- cpp_double_evaluate(x)
      if (experimenter$observed)
        cpp_write_line(cpp_loggerCOCOInfo())
      return(f)
    }
  }
  return(structure(
    list(
      dimension = ans$dimension,
      function_id = ans$problem,
      instance = ans$instance,
      suite = experimenter$suite,
      fopt = cpp_get_optimal(),
      lbound = if (experimenter$suite == "PBO")
                  cpp_get_int_lower_bounds()
               else cpp_get_double_lower_bounds(),
      ubound = if (experimenter$suite == "PBO")
                  cpp_get_int_upper_bounds()
               else cpp_get_double_upper_bounds(),
      maximization = (cpp_get_optimization_type() == 1),
      params.track = experimenter$param.track,
      obj_func = function(x) {
        if (is.null(dim(x))) x <- t(x)
        if (ncol(x) != ans$dimension) x <- t(x)

        stopifnot(ncol(x) == ans$dimension)
        apply(x, 1, internal_eval)
      },
      target_hit = function() {
        cpp_is_target_hit()
      },
      set_parameters = 
        if (length(experimenter$param.track) > 0) function(param_vals){
          stopifnot( length(param_vals) == length(experimenter$param.track) )
          cpp_set_parameters_value(param_vals)
        }
        else
          NULL

    ),
    class = c('IOHproblem', 'list')
  ))
}

#' Reset the IOHproblem
#'
#' @param problem The IOHproblem object
#'
#' @return An IOHproblem object
#' @export
#' @examples 
#' exp <- IOHexperimenter()
#' p <- next_problem(exp)
#' IOH_random_search(p)
#' p <- reset_problem(p)
reset_problem <- function(problem) {
  ans <- cpp_reset_problem()
  if (is.null(ans) || is.null(ans$problem)) return(NULL)
  else return(problem)
}


