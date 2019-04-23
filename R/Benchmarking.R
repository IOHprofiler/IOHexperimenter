#' Base procedure for benchmarking a custom algorithm
#'
#' @param user_alg Function defining the custom algorithm. Needs to accept one paramter:
#' an IOHproblem object, which has the following properties:
#' \itemize{
#' \item dimension
#' \item function_id
#' \item instance
#' \item fopt (if known)
#' \item xopt (if known)
#' }
#' And the following functions:
#' \itemize{
#' \item obj_func
#' \item target_hit
#' \item set_parameters
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
#' @param params.track Which parameters to track. Should be a vector of strings, containing no spaces or commas
#' @param repetitions How many independent runs of the algorithm to do for each problem
#'
#' @examples
#' \donttest{
#'
#' benchmark_algorithm(random_search, params.track = 'Test_param')
#' }
#' @export
benchmark_algorithm <- function(user_alg, functions = NULL, instances = NULL, dimensions = NULL,
                                data.dir = NULL, algorithm.info = '', algorithm.name = '',
                                cdat = FALSE, idat = 0, tdat = 3, params.track = NULL,
                                repetitions = 5) {
  # Setting default parameters if needed and verifying parameter integrity
  if (is.null(functions)) functions <- seq(2)
  else {
    #assert_that( is.numeric(functions) )
    #TODO: make a funcion which returns all possible functions / dimensions
    stopifnot( all( functions %in% 1:23 ) )

  }
  if (is.null(instances)) instances <- seq(2)
  else {
    # assert_that( is.numeric(instances) )
    stopifnot( all( instances %in% seq(100) ) )
  }
  if (is.null(dimensions)) dimensions <- c(100, 300)
  else {

    #assert_that( is.numeric(dimensions) )
    stopifnot( all( dimensions %in% 1:5000 ) )
  }
  if (is.null(data.dir)) data.dir <- './data'

  experimenter <- IOHexperimenter(dims = dimensions, functions = functions, instances = instances,
                                  algorithm.info = algorithm.info, algorithm.name = algorithm.name,
                                  data.dir = data.dir, cdat = cdat, idat = idat, tdat = tdat,
                                  param.track = params.track)
  IOHproblem <- next_problem(experimenter)
  while ( !is.null(IOHproblem) ) {
    for (rep in 1:repetitions){
      user_alg(IOHproblem)
      reset_problem(IOHproblem)
    }
    IOHproblem <- next_problem(experimenter)
  }

}
