#include <Rcpp.h>
#include <string>
#include <stdlib.h>

// @Furong: what are those used for?
#undef Realloc
#undef Free

#include "IOHprofiler.h"

using namespace Rcpp;

IOHprofiler_observer_t * current_observer;
IOHprofiler_suite_t* current_suite;
IOHprofiler_problem_t* current_problem;

// Create a suite with specific arguments.

//[[Rcpp::export]]
void c_init_suite(String functions, String dimensions, String instances){
	if(current_suite != NULL){
		IOHprofiler_suite_free(current_suite);
		Rcout << "Current suite has been erased.\n";
	}
	String ins = " instances: ";
	String func = " function_indices: ";
	String dim =  " dimensions: ";
	ins += instances;
	func += functions;
	dim += dimensions;
	func += dim;
	current_suite = IOHprofiler_suite("PBO", ins.get_cstring() ,func.get_cstring());

	Rcout << "Suite PBO has been initialized.\n";
}



// Create an  All options are with default values. To config output path and format, apply following functions.

//[[Rcpp::export]]
void c_init_observer(String result_folder, String algorithm_name, String algorithm_info,
					String complete_triggers, int number_interval_triggers,
					String base_evaluation_triggers, int number_target_triggers,
					String parameters_name) {


	char *observer_options = IOHprofiler_strdupf( "result_folder:  %s "
                                                "algorithm_name: %s "
                                                "algorithm_info: %s "
                                                "complete_triggers: %s "
                                                "number_interval_triggers: %d "
                                                "base_evaluation_triggers: %s "
                                                "number_target_triggers: %d "
                                                "parameters_name: %s",
                                                result_folder.get_cstring(),algorithm_name.get_cstring(),
                                                algorithm_info.get_cstring(),complete_triggers.get_cstring(),
                                                number_interval_triggers, base_evaluation_triggers.get_cstring(),
                                                number_target_triggers,parameters_name.get_cstring());

	current_observer = IOHprofiler_observer("PBO", observer_options);
}


//[[Rcpp::export]]
IntegerVector get_problem_list() {
	if(current_suite == NULL){
		return NULL;
	}
	IntegerVector problem_list;
	for (int i = 0; i < current_suite->number_of_functions; ++i)
	{
		if(current_suite->functions[i] != 0){
			problem_list.push_back(current_suite->functions[i]);
		}
	}
	return problem_list;
}

//[[Rcpp::export]]
IntegerVector get_dimension_list() {
	if(current_suite == NULL){
		return NULL;
	}
	IntegerVector dimension_list;
	for (int i = 0; i < current_suite->number_of_dimensions; ++i)
	{
		if(current_suite->dimensions[i] != 0){
			dimension_list.push_back(current_suite->dimensions[i]);
		}
	}
	return dimension_list;
}


//[[Rcpp::export]]
List get_suite_info() {
	if(current_suite == NULL) {
		return NULL;
	}
	else{
		return List::create(_["problems"] = get_problem_list(),
						_["dimensions"] =  get_dimension_list(),
						_["instances"] = IntegerVector(current_suite->instances,current_suite->instances + current_suite->number_of_instances));
	}
}


//[[Rcpp::export]]
List get_problem_info() {
	if(current_problem == NULL) {
		return NULL;
	}
	else {
		return List::create(_["problem"] = current_suite->functions[current_suite->current_function_idx] ,
						_["dimension"] = current_problem->dimension,
						_["instance"] = current_suite->instances[current_suite->current_instance_idx]);
	}
}


// Acquire a problem in suite by order.

//[[Rcpp::export]]
List c_get_next_problem() {
	if(current_suite != NULL && current_observer != NULL){
		current_problem = IOHprofiler_suite_get_next_problem(current_suite, current_observer);
		if(current_problem == NULL){
			IOHprofiler_observer_free(current_observer);
		  current_observer = NULL;
	  	IOHprofiler_suite_free(current_suite);
		  current_suite = NULL;
		}
		else{
			return get_problem_info();
		}
	}
	return NULL;
}

// To reset status of current problem. Applying this function to re-run current problem.

//[[Rcpp::export]]
List c_reset_problem() {
	if(current_problem == NULL){
		Rcout << "There is no problem exists.\n";
		return NULL;
	}
	else{
		current_problem = IOHprofiler_suite_reset_problem(current_suite, current_observer);
		return get_problem_info();
	}
}

//[[Rcpp::export]]
double c_eval(IntegerVector x) {
	if(current_problem == NULL) {
	  Rcout << "Error! No function selected.\n";
		return -DBL_MAX;
	}

	double *result = IOHprofiler_allocate_vector(1);

	if(x.size() != current_problem->number_of_variables) {
		Rcout << "Error! Dimension of input vector is incorrect.\n";
	 	return -DBL_MAX;
	}


	IOHprofiler_evaluate_function(current_problem, INTEGER(x), result);
	return result[0];
}

// Set parameters.

//[[Rcpp::export]]
void c_set_parameters(NumericVector parameters){
  IOHprofiler_problem_set_parameters(current_problem,parameters.size(),REAL(parameters));
}

// To test if the optimal has been found. Return 1 if it is found.

//[[Rcpp::export]]
int c_is_target_hit() {
	if(current_problem == NULL) {
		return -1;
	}

	return IOHprofiler_problem_final_target_hit(current_problem);
}

// Returns evaluation time has been done.

//[[Rcpp::export]]
int c_get_evaluations() {
	if(current_problem == NULL) {
		return -1;
	}

	return IOHprofiler_problem_get_evaluations(current_problem);
}

// Return statement of optimal variables.

//[[Rcpp::export]]
IntegerVector c_get_xopt() {
	if(current_problem == NULL) {
		return NULL;
	}

	return IntegerVector(current_problem->best_parameter,current_problem->best_parameter+current_problem->number_of_variables);
}

// Return the optimal value.

//[[Rcpp::export]]
double c_get_fopt() {
	if(current_problem == NULL) {
		return -DBL_MAX;
	}

	return current_problem->best_value[0];
}


// Free memory.

//[[Rcpp::export]]
void IOHfree() {
  if(current_problem != NULL){
    IOHprofiler_problem_free(current_problem);
    current_problem = NULL;
    current_suite->current_problem = NULL;
  }
  if(current_observer != NULL){
    IOHprofiler_observer_free(current_observer);
    current_observer = NULL;
  }
  if(current_suite != NULL){
    IOHprofiler_suite_free(current_suite);
    current_suite = NULL;
  }
}

//[[Rcpp::export]]
void freeProblem(){
  if(current_problem != NULL){
    IOHprofiler_problem_free(current_problem);
    current_suite->current_problem = NULL;
    current_problem = NULL;
  }
}

//[[Rcpp::export]]
void freeSuite(){
  if(current_suite != NULL){
    IOHprofiler_suite_free(current_suite);
    current_suite = NULL;
  }
}
//[[Rcpp::export]]
void freeObserver(){
  if(current_observer != NULL){
    IOHprofiler_observer_free(current_observer);
    current_observer = NULL;
  }
}


