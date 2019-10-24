#' S3 print function for IOHproblem
#'
#' @param x The IOHproblem to print
#' @param ... Arguments for underlying function
#'
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
#'
#' @export
#' @examples 
#' exp <- IOHexperimenter()
#' p <- next_problem(exp)
#' as.character(p)
as.character.IOHproblem <- function(x, ...) {
  sprintf('IOHproblem (Suite %s: Instance %d of function %s %dD)\n', x$suite, x$instance, 
          x$function_id, x$dimension)
}

#' Get the next function of the currently initialized suite
#'
#' @param experimenter The IOHexperimenter object
#'
#' @return An IOHproblem object
#' @export
#' @examples 
#' exp <- IOHexperimenter()
#' p <- next_problem(exp)
next_problem <- function(experimenter) {
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
      # fopt = cpp_get_fopt(),
      # xopt = cpp_get_xopt(),
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


