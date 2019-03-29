#include <Rcpp.h>
#include <string>
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
	if(suite.current_suite != NULL){
		IOHprofiler_suite_free(suite.current_suite);
		Rcout << "Current suite has been erased.\n";
	}
	String ins = " instances: ";
	String func = " function_indices: ";
	String dim =  " dimensions: ";
	ins += instances;
	func += functions;
	dim += dimensions;
	func += dim;
	suite.current_suite = IOHprofiler_suite("PBO", ins.get_cstring() ,func.get_cstring());
	suite.name = "PBO";
	suite.instance = instances;
	suite.dimension = dimensions;
	suite.function = functions;
	suite.state = 1;
	Rcout << "Suite PBO has been initialized.\n";
}

// Create a default suite including all functions and instances in dimension 100,500,1000,2000,3000.

// //[[Rcpp::export]]
// void default_suite(){
// 	if(suite.current_suite != NULL){
// 		IOHprofiler_suite_free(suite.current_suite);
// 		Rcout << "Current suite has been erased.\n";
// 	}
// 	String ins = " instances: ";
// 	String func = " function_indices: ";
// 	String dim =  " dimensions: ";
// 	ins += "1-100";
// 	func += "1-24";
// 	dim += "100,500,1000,2000,3000";
// 	func += dim;
// 	suite.current_suite = IOHprofiler_suite("PBO", ins.get_cstring() ,func.get_cstring());
// 	suite.name = "PBO";
// 	suite.instance = "1-100";
// 	suite.dimension = "100,500,1000,2000,3000";
// 	suite.function = "1-24";
// 	suite.state = 1;
// 	Rcout << "Suite PBO has been initialized.\n";
// }


// Create an observer. All options are with default values. To config output path and format, apply following functions. 

//[[Rcpp::export]]
void init_observer(){
	if(observer.current_observer != NULL){
		IOHprofiler_observer_free(observer.current_observer);
		Rcout << "Current observer has been erased.\n";
	}
	char *observer_options = IOHprofiler_strdupf( "result_folder:  %s "
                                                "algorithm_name: %s "
                                                "algorithm_info: %s "
                                                "complete_triggers: %s "
                                                "number_interval_triggers: %d "
                                                "base_evaluation_triggers: %s "
                                                "number_target_triggers: %d "
                                                "parameters_name: %s", 
                                                observer.result_folder.get_cstring(),observer.algorithm_name.get_cstring(),
                                                observer.algorithm_info.get_cstring(),observer.complete_triggers.get_cstring(),
                                                observer.number_interval_triggers, observer.base_evaluation_triggers.get_cstring(),
                                                observer.number_target_triggers,observer.parameters_name.get_cstring());
 
	observer.current_observer = IOHprofiler_observer("PBO", observer_options);
	observer.name = "PBO";
	observer.state = 1;
	Rcout << "A new observer PBO has been created.\n";
}


// Those would not be necesary

// //[[Rcpp::export]]
// void set_result_folder(String folder){
// 	observer.result_folder = folder;
// 	Rcout << "Output path has been set as " << folder.get_cstring() << ".\n";
// }

// //[[Rcpp::export]]
// void set_algorithm_name(String alg_name){
// 	observer.algorithm_name = alg_name;
// 	Rcout << "Algorithm name has been set as " << alg_name.get_cstring() << ".\n";
// }

// //[[Rcpp::export]]
// void set_algorithm_info(String alg_info){
// 	observer.algorithm_info = alg_info;
// 	Rcout << "Algorithm info has been set as " << alg_info.get_cstring() << ".\n";
// }

// //[[Rcpp::export]]
// void set_complete_trigger(String complete_trig){
// 	observer.complete_triggers = complete_trig;
// 	if(complete_trig == "True" || complete_trig == "true" || complete_trig == "TRUE"){
// 		Rcout << ".cdat files that store evaluations for each iteration will be output.\n";
// 	}else{
// 		Rcout << ".cdat files are closed.\n";
// 	}
// }

// //[[Rcpp::export]]
// void set_number_interval_triggers(int num_interval_trig){
// 	observer.number_target_triggers = num_interval_trig;
// 	if(num_interval_trig == 0){
// 		Rcout << ".idat files are closed.\n";
// 	}else{
// 		Rcout << ".idat files that store evaluations for each " << num_interval_trig << " iteration will be output.\n";
// 	}
// }

// //[[Rcpp::export]]
// void set_number_target_triggers(int num_target_targ){
// 	observer.number_target_triggers = num_target_targ;
// 	Rcout << "number_target_triggers has been set as " << num_target_targ << "\n.";
// }

// //[[Rcpp::export]]
// void set_parameter_names(String para_names){
// 	observer.parameters_name = para_names;
// 	Rcout << "Parameter names have been set as " << para_names.get_cstring() << ".\n Please confirm that there is no blank, and multiple names are separate by comma \',\'.\n";
// }

// Acquire a problem based on arguments, but the problem must be included in the suite.
// @Furong: set each c procedure name with prefix 'c_'
//[[Rcpp::export]]
void c_set_function(int dimension, int function_id, int instance_id){
	if(suite.state == 0){
		Rcout << "Please create a suite at first.\n";
	}
	if(observer.state == 0){
		Rcout << "Please create an observer at first.\n";
	}
	suite.current_problem = IOHprofiler_suite_get_problem_by_function_dimension_instance(suite.current_suite,problem_id,dimension,instance_id);
	if(suite.current_problem == NULL){
		Rcout << "The problem is not found in suite.\n";
	}else{
		Rcout << "Currently working on " << IOHprofiler_problem_get_name(suite.current_problem) << ".\n";	
	}
}

// Acquire a problem in suite by order.

//[[Rcpp::export]]
void get_next_problem(){
	// if(suite.state == 0){
	// 	Rcout << "Please create a suite at first.\n";
	// }
	// if(observer.state == 0){
	// 	Rcout << "Please create an observer at first.\n";
	// }
	suite.current_problem = IOHprofiler_suite_get_next_problem(suite.current_suite, observer.current_observer);
	if(suite.current_problem == NULL){
		IOHprofiler_observer_free(observer.current_observer);
  		IOHprofiler_suite_free(suite.current_suite);
  		suite.state = 0;
  		observer.state = 0;
  		Rcout << "All problems have been tested, suite and observer are free. Thanks for using IOHExperimentor.\n";
	}
	else{
		Rcout << "Currently working on " << IOHprofiler_problem_get_name(suite.current_problem) << ".\n";
	}
}

// To reset status of current problem. Applying this function to re-run current problem.

//[[Rcpp::export]]
void reset_problem(){
	if(suite.current_problem == NULL){
		Rcout << "There is no problem exists.\n";
	}
	else{
		suite.current_problem = IOHprofiler_suite_reset_problem(suite.current_suite, observer.current_observer);
		Rcout << IOHprofiler_problem_get_name(suite.current_problem) << "resets.\n";	
	}
}

// // Returns a list of problem id of current suite.
// //[[Rcpp::export]]
// NumericVector get_problem_list(){
// 	if(suite.current_suite == NULL){
// 		Rcout << "There is no suite exists.\n";
// 		return 1;
// 	}
// 	NumericVector problem_list;
// 	for (int i = 0; i < suite.current_suite->number_of_functions; ++i)
// 	{
// 		if(suite.current_suite->functions[i] != 0){
// 			problem_list.push_back(suite.current_suite->functions[i]);
// 		}
// 	}
// 	return problem_list;
// }

// // Returens a list of dimensions of current suite.

// //[[Rcpp::export]]
// NumericVector get_dimension_list(){
// 	if(suite.current_suite == NULL){
// 		Rcout << "There is no suite exists.\n";
// 		return 1;
// 	}
// 	NumericVector dimension_list;
// 	for (int i = 0; i < suite.current_suite->number_of_dimensions; ++i)
// 	{
// 		if(suite.current_suite->dimensions[i] != 0){
// 			dimension_list.push_back(suite.current_suite->dimensions[i]);
// 		}
// 	}
// 	return dimension_list;
// }

// // Returns a list of instances of current suite.

// //[[Rcpp::export]]
// NumericVector get_instance_list(){
// 	if(suite.current_suite == NULL){
// 		Rcout << "There is no suite exists.\n";
// 		return 1;
// 	}
// 	NumericVector instance_list(suite.current_suite->number_of_instances);
// 	for(int i = 0; i < suite.current_suite->number_of_instances; ++i){
// 		instance_list(i) = suite.current_suite->instances[i];
// 	} 
// 	return instance_list;
// }

// Returns name string of current problem.

//[[Rcpp::export]]
String get_problem_info(){
	if(suite.current_problem == NULL){
		Rcout << "No problem exists.\n";
		return 1;
	}
	return IOHprofiler_problem_get_name(suite.current_problem); 
}

// Return fitness of input variables.

//[[Rcpp::export]]
double c_eval(NumericVector x){
	double *result = IOHprofiler_allocate_vector(1);
	int n = x.size();
	// if(n != suite.current_problem->number_of_variables){
	// 	Rcout << "Error! Dimension of input vector is incorrect.\n";
	// 	return -DBL_MAX;
	// }

	int *input = IOHprofiler_allocate_int_vector(n);
	for(int i = 0; i < n; ++i){
		input[i] = x(i);
	}
	IOHprofiler_evaluate_function(suite.current_problem, input, result);
	return result[0];
}

// Free memory.

//[[Rcpp::export]]
void IOHfree(){
	if(suite.current_problem != NULL){
		IOHprofiler_problem_free(suite.current_problem);
	}
	if(observer.current_observer != NULL){
		IOHprofiler_observer_free(observer.current_observer);
	}
	if(suite.current_suite != NULL){
  		IOHprofiler_suite_free(suite.current_suite);
  	}
  	Rcout << "IOHExperimentor free.\n";
}