#ifndef _IOHPROFILER_LOGGER_H
#define _IOHPROFILER_LOGGER_H


#include "common.hpp"

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

namespace fs = boost::filesystem;

// Implementation for logging process of problem evaluations.
// A folder for storing data with given name 'path' will be created
// when the class is generated. To activate logger functions as 
// evaluating problems, a 'logger' must be added to a
// IOHprofiler_problem with format 'problem.add_logger(logger)'.
//
// Hint: Only use an unique logger for each experiments for an algorithm.
// But a logger is avaiable for test of an algorithm on multiple prorblems.

class IOHprofiler_logger
{
public:

  // Once a logger is created, the working directory is established. Also, files to
  // be logged are set up.
  // Experiments on different problems can sure the same logger, but the problem info
  // needs to be set by function "IOHprofiler_logger_target_problem()" for experiment 
  // on each function. So does parameters infomation.
  IOHprofiler_logger(std::string directory, std::string folder_name,
                     std::string alg_name, std::string alg_info, 
                     int complete_triggers, int number_interval_triggers,
                    ) {
    this->output_directory = directory;
    this->folder_name = folder_name;
    this->algorithm_name =  alg_name;
    this->algorithm_info = alg_info;

    if(complete_triggers == 1) {
      cFlag = true;
    }
    if(number_interval_triggers > 0){
      iFlag = true;
    }
  };
  //~IOHprofiler_logger();

  /***************************************************************
  ******** Functions to be invoked by evaluate of problem ********
  ****************************************************************/

  void IOHprofiler_logger_target_problem(int problem_id, int dimension, int instance, std::vector<std::string> parameters_name){
    this->problem_id = problem_id;
    this->dimension = dimension;
    this->instance = instance;
    
    std::sub_directory_name = this->output_directory + IOHprofiler_path_separator 
                            + this->folder_name + IOHprofiler_path_separator
                            + "data_f" + std::to_string(problem_id);
    
    // Create the specific folder storing data for the problem, if it does not exist. 
    if(!fs::exists(sub_directory_name.c_str())){
      if(!fs::create_directory(folder_name)) {
        printf("Error on creating sub-directory for problem %d\n", problem_id);
      }
    }


    /***************************************
    Need to do exception handling.
    ***************************************/
    std::string infoFile_name = this->output_directory + IOHprofiler_path_separator 
                              + this->folder_name + IOHprofiler_path_separator
                              + "IOHprofiler_f" + std::to_string(problem_id) + "_i1"
                              + ".info"; 
    if(infoFile.is_open()) infoFile.close();
    infoFile.open(infoFile_name.c_str());

    if(cFlag == true){
      std::string cdat_name = sub_directory_name + IOHprofiler_path_separator 
                            + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                            + std::to_string(dimension) + "_i1.cdat";

      if(cdat.is_open()) cdat.close();
      cdat.open(cdat_name);
    }

    if(iFlag == true){
      std::string idat_name = sub_directory_name + IOHprofiler_path_separator 
                            + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                            + std::to_string(dimension) + "_i1.idat";

      if(idat.is_open()) idat.close();
      idat.open(idat_name);
    }

    std::string dat_name = sub_directory_name + IOHprofiler_path_separator 
                            + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                            + std::to_string(dimension) + "_i1.dat";

    if(dat.is_open()) dat.close();
    dat.open(dat_name);
    dat << dat_header;
  }



  // Creating the folder for logging files of the corresponding problem.
  int IOHprofiler_logger_openIndex();

  // Initilizing the files for current logging process.
  int IOHprofiler_logger_initilizing_files();

  // To be used in IOHprofiler_problem class. Files are targeted by `problem_id`
  // and `dimension`, and the log_line will be written.
  void IOHprofiler_problem_logger(int problem_id, int dimension, int instance, std::string log_line);


private:
  size_t evaluations;
  std::string folder_name;
  std::string output_directory;


  // The information of logged problems.
  int dimension;
  int problem_id;
  int instance;
  std::string algorithm_name;
  std::string algorithm_info;
  std::string suite_name;
  std::vector<std::string> parameter_name;

  /***************************************************************
  *****************  Variables for logging files  ****************
  ****************************************************************/
  const string dat_header = "\"function evaluation\" \"current f(x)\" \"best-so-far f(x)\" \"current af(x)+b\"  \"best af(x)+b\"";

  bool cFlag = false;
  bool iFlag = false;

  std::ofstream cdat;
  std::ofstream idat;
  std::ofstream dat;
  std::ofstream infoFile;

  // Parameters for .tdat file.
  int number_evaluation_triggers = 0;
  char * base_evaluation_triggers = 0;

  // The interval value for .idat files. .idat file is not to be generated if set as 0.
  int number_interval_triggers = 0;

  // .cdat file is to be generated if set as 1.
  int complete_triggers = 1;



  /***************************************************************
  **********************  Private functions  *********************
  ****************************************************************/
  void IOHprofiler_logger_write_infoFile(char * line);
  
  // Creates a folder for storing info. files and sub-folders of all experiments 
  int IOHprofiler_create_folder(const std::string path);

  // Tests if the corresponding folder for tested function exists. Create a new
  // folder if it does not exists.
  int IOHprofiler_check_function_folder(const std::string );


  // Returns a name that is allowed.
  std::string IOHprofiler_experiment_folder_name();
};

#endif