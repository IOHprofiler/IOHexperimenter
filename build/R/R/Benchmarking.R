#' Base procedure for benchmarking a custom algorithm
#'
#' @param user_alg Function defining the custom algorithm. Needs to accept one paramter:
#' an IOHproblem object, which has the following properties:
#' \itemize{
#' \item dimension
#' \item function_id
#' \item instance
#' \item suite (Currently 'BBOB' or 'PBO')
#' }
#' And the following functions:
#' \itemize{
#' \item obj_func
#' \item target_hit
#' \item set_parameters
#' }
#' @param suite Which suite to test on
#' @param dimensions Which dimensions to test on
#' @param functions Which function to test on
#' @param instances Which instances to test on
#' @param algorithm.info Additional information about the algorithm you plan on running
#' @param algorithm.name The name of the algorithm you plan on running
#' @param data.dir Where the data should be stored (defaults to "./data" when not provided)
#' @param params.track Which parameters to track. Should be a vector of strings, containing no spaces or commas
#' @param repetitions How many independent runs of the algorithm to do for each problem instance
#'
#' @return None
#' @examples
#' \donttest{
#'
#' benchmark_algorithm(IOH_two_rate_GA, params.track = 'Mutation_rate', data.dir = './data')
#' }
#' @export
benchmark_algorithm <- function(user_alg, suite = "PBO", functions = c(1,2), instances = c(1,2),
                                dimensions = 16, data.dir = NULL, algorithm.info = ' ', 
                                algorithm.name = ' ', params.track = NULL,
                                repetitions = 5) {

  experimenter <- IOHexperimenter(suite = suite, dims = dimensions, functions = functions, instances = instances,
                                  algorithm.info = algorithm.info, algorithm.name = algorithm.name,
                                  data.dir = data.dir, param.track = params.track)
  IOHproblem <- next_problem(experimenter)
  while (!is.null(IOHproblem) ) {
    for (rep in 1:(repetitions - 1) ) {
      user_alg(IOHproblem)
      IOHproblem <- reset_problem(IOHproblem)
    }
    user_alg(IOHproblem)
    
    IOHproblem <- next_problem(experimenter)
  }
  cpp_clear_logger()
  cpp_clear_suite()
}
