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
  sprintf('IOHproblem (Instance %d of function %d %dD)\n', x$instance, x$function_id, 
          x$dimension)
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
  ans <- c_get_next_problem()
  if (is.null(ans) || is.null(ans$problem)) return(NULL)

  return(structure(
    list(
      dimension = ans$dimension,
      function_id = ans$problem,
      instance = ans$instance,
      fopt = c_get_fopt(),
      xopt = c_get_xopt(),
      params.track = experimenter$param.track,
      obj_func = function(x) {
        stopifnot( all(x %in% c(0,1) ))
        if (is.null(dim(x))) x <- t(x)
        if (ncol(x) != ans$dimension) x <- t(x)

        stopifnot(ncol(x) == ans$dimension)
        apply(x, 1, c_eval)
      },
      target_hit = function() {
        c_is_target_hit()
      },
      set_parameters = function(param_vals){
        stopifnot( length(param_vals) == length(experimenter$param.track) )
        c_set_parameters(param_vals)
      }

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
  ans <- c_reset_problem()
  if (is.null(ans) || is.null(ans$problem)) return(NULL)
  else return(problem)
}


