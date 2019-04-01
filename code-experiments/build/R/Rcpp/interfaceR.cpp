#include <Rcpp.h>
#include <string>
#undef Realloc
#undef Free
#include "IOHprofiler.c"




using namespace Rcpp;

IOHprofiler_observer_t * current_observer;
IOHprofiler_suite_t* current_suite;
IOHprofiler_problem_t* current_problem;

// struct Suite{
//     IOHprofiler_suite_t* current_suite;
//     IOHprofiler_problem_t* current_problem;
//     String name;
//     String instance;
//     String function;
//     String dimension;
//     int current_problem_id = 0;
//     int current_instance_id = 0;
//     int current_dimension = 0;
//     int state = 0;
//     int problem_flag = 0;
// }suite;


// struct Observer{
// 	IOHprofiler_observer_t * current_observer;
// 	String name;
// 	int state = 0;	
// 	String result_folder = "IOHresult";
// 	String algorithm_name = "ALG";
// 	String algorithm_info = "ALG";
// 	String complete_triggers = "false";
// 	int number_interval_triggers = 0;
// 	String base_evaluation_triggers = "1,2,3";
// 	int number_target_triggers = 3;
// 	String parameters_name = "";
// }observer;

// Create a suite with specific arguments.

//[[Rcpp::export]]
void init_suite(String functions, String dimensions, String instances){
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

// Create a default suite including all functions and instances in dimension 100,500,1000,2000,3000.

// //[[Rcpp::export]]
// void default_suite(){
// 	if(current_suite != NULL){
// 		IOHprofiler_suite_free(current_suite);
// 		Rcout << "Current suite has been erased.\n";
// 	}
// 	String ins = " instances: ";
// 	String func = " function_indices: ";
// 	String dim =  " dimensions: ";
// 	ins += "1-100";
// 	func += "1-24";
// 	dim += "100,500,1000,2000,3000";
// 	func += dim;
// 	current_suite = IOHprofiler_suite("PBO", ins.get_cstring() ,func.get_cstring());
// 	name = "PBO";
// 	instance = "1-100";
// 	dimension = "100,500,1000,2000,3000";
// 	function = "1-24";
// 	state = 1;
// 	Rcout << "Suite PBO has been initialized.\n";
// }


// Create an  All options are with default values. To config output path and format, apply following functions. 

//[[Rcpp::export]]
void init_observer(String result_folder, String algorithm_name, String algorithm_info, 
					String complete_triggers, int number_interval_triggers,
					String base_evaluation_triggers, int number_target_triggers,
					String parameters_name) {
//	if(current_observer != NULL){
//		IOHprofiler_observer_free(current_observer);
//		Rcout << "Current observer has been erased.\n";
//	}

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
//	Rcout << "A new observer PBO has been created.\n";
}


// Those would not be necesary

// //[[Rcpp::export]]
// void set_result_folder(String folder){
// 	result_folder = folder;
// 	Rcout << "Output path has been set as " << folder.get_cstring() << ".\n";
// }

// //[[Rcpp::export]]
// void set_algorithm_name(String alg_name){
// 	algorithm_name = alg_name;
// 	Rcout << "Algorithm name has been set as " << alg_name.get_cstring() << ".\n";
// }

// //[[Rcpp::export]]
// void set_algorithm_info(String alg_info){
// 	algorithm_info = alg_info;
// 	Rcout << "Algorithm info has been set as " << alg_info.get_cstring() << ".\n";
// }

// //[[Rcpp::export]]
// void set_complete_trigger(String complete_trig){
// 	complete_triggers = complete_trig;
// 	if(complete_trig == "True" || complete_trig == "true" || complete_trig == "TRUE"){
// 		Rcout << ".cdat files that store evaluations for each iteration will be output.\n";
// 	}else{
// 		Rcout << ".cdat files are closed.\n";
// 	}
// }

// //[[Rcpp::export]]
// void set_number_interval_triggers(int num_interval_trig){
// 	number_target_triggers = num_interval_trig;
// 	if(num_interval_trig == 0){
// 		Rcout << ".idat files are closed.\n";
// 	}else{
// 		Rcout << ".idat files that store evaluations for each " << num_interval_trig << " iteration will be output.\n";
// 	}
// }

// //[[Rcpp::export]]
// void set_number_target_triggers(int num_target_targ){
// 	number_target_triggers = num_target_targ;
// 	Rcout << "number_target_triggers has been set as " << num_target_targ << "\n.";
// }

// //[[Rcpp::export]]
// void set_parameter_names(String para_names){
// 	parameters_name = para_names;
// 	Rcout << "Parameter names have been set as " << para_names.get_cstring() << ".\n Please confirm that there is no blank, and multiple names are separate by comma \',\'.\n";
// }

// Acquire a problem based on arguments, but the problem must be included in the 
// @Furong: set each c procedure name with prefix 'c_'
// //[[Rcpp::export]]
// void c_set_function(int dimension, int problem_id, int instance_id){
//	if(state == 0){
//		Rcout << "Please create a suite at first.\n";
//	}
//	if(state == 0){
//		Rcout << "Please create an observer at first.\n";
//	}
//	if(current_problem != NULL) {
//		IOHprofiler_problem_free(current_problem);
//	}
//	current_problem = IOHprofiler_suite_get_problem_by_function_dimension_instance(current_suite,problem_id,dimension,instance_id);
//	current_problem = IOHprofiler_problem_add_observer(current_problem,current_observer);
//	if(current_problem == NULL){
//		Rcout << "The problem is not found in \n";
//	}else{
//		Rcout << "Currently working on " << IOHprofiler_problem_get_name(current_problem) << ".\n";	
//	}
//}

// Returns a list of problem ids.

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

// Returens a list of dimensions. 

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
List get_next_problem() {
	// if(state == 0){
	// 	Rcout << "Please create a suite at first.\n";
	// }
	// if(state == 0){
	// 	Rcout << "Please create an observer at first.\n";
	// }
	if(current_suite != NULL && current_observer != NULL){
		current_problem = IOHprofiler_suite_get_next_problem(current_suite, current_observer);
		if(current_problem == NULL){
			IOHprofiler_observer_free(current_observer);
	  		IOHprofiler_suite_free(current_suite);
	//  		Rcout << "All problems have been tested, suite and observer are free. Thanks for using IOHExperimentor.\n";
		}
		else{
			return get_problem_info();
	//		Rcout << "Currently working on " << IOHprofiler_problem_get_name(current_problem) << ".\n";
		}
	}
	return NULL;
}

// To reset status of current problem. Applying this function to re-run current problem.

//[[Rcpp::export]]
List reset_problem() {
	if(current_problem == NULL){
		Rcout << "There is no problem exists.\n";
		return NULL;
	}
	else{
		current_problem = IOHprofiler_suite_reset_problem(current_suite, current_observer);
		return get_problem_info();
	}
}


// 

// //[[Rcpp::export]]
// NumericVector get_instance_list(){
// 	if(current_suite == NULL){
// 		Rcout << "There is no suite exists.\n";
// 		return 1;
// 	}
// 	NumericVector instance_list(current_suite->number_of_instances);
// 	for(int i = 0; i < current_suite->number_of_instances; ++i){
// 		instance_list(i) = current_suite->instances[i];
// 	} 
// 	return instance_list;
// }

// Returns name string of current problem.



// Return fitness of input variables.

//[[Rcpp::export]]
double c_eval(IntegerVector x) {
	if(current_problem == NULL) {
		return NULL;
	}

	double *result = IOHprofiler_allocate_vector(1);
	
	if(x.size() != current_problem->number_of_variables){
		Rcout << "Error! Dimension of input vector is incorrect.\n";
	 	return -DBL_MAX;
	}

	IOHprofiler_evaluate_function(current_problem, INTEGER(x), result);
	return result[0];
}

// To test if the optimal has been found. Return 1 if it is found.

//[[Rcpp::export]]
int hit_optimal() {
	if(current_problem == NULL) {
		return -1;
	}

	return IOHprofiler_problem_final_target_hit(current_problem);
}

// Returns evaluation time has been done.

//[[Rcpp::export]]
int eval_times() {
	if(current_problem == NULL) {
		return -1;
	}

	return IOHprofiler_problem_get_evaluations(current_problem);
}

// Return statement of optimal variables.

//[[Rcpp::export]]
IntegerVector optimal_variables() {
	if(current_problem == NULL) {
		return NULL;
	}

	return IntegerVector(current_problem->best_parameter,current_problem->best_parameter+current_problem->number_of_variables);
}

// Return the optimal value.

//[[Rcpp::export]]
double optimal() {
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
	}
	if(current_observer != NULL){
		IOHprofiler_observer_free(current_observer);
	}
	if(current_suite != NULL){
  		IOHprofiler_suite_free(current_suite);
  	}
  	Rcout << "IOHExperimentor free.\n";
}

//[[Rcpp::export]]
void freeProblem(){
	if(current_problem != NULL){
		IOHprofiler_problem_free(current_problem);
	}
}

//[[Rcpp::export]]
void freeSuite(){
	if(current_suite != NULL){
  		IOHprofiler_suite_free(current_suite);
  	}
}
//[[Rcpp::export]]
void freeObserver(){
	if(current_observer != NULL){
		IOHprofiler_observer_free(current_observer);
	}
}



