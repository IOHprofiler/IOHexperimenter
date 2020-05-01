/// \file IOHprofiler_csv_logger.cpp
/// \brief cpp file for class IOHprofiler_csv_logger.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#include "IOHprofiler_csv_logger.hpp"

IOHprofiler_csv_logger::IOHprofiler_csv_logger() {
  this->output_directory = "./";
  this->folder_name = "IOHprofiler_test";
  this->algorithm_name =  "algorithm";
  this->algorithm_info = "algorithm_info";
}

IOHprofiler_csv_logger::IOHprofiler_csv_logger(std::string directory, std::string folder_name,
                        std::string alg_name, std::string alg_info) {
  this->output_directory = directory;
  this->folder_name = folder_name;
  this->algorithm_name =  alg_name;
  this->algorithm_info = alg_info;
}

IOHprofiler_csv_logger::~IOHprofiler_csv_logger() {
  this->clear_logger();
}

bool IOHprofiler_csv_logger::folder_exist(std::string folder_name) {
  std::fstream _file;
  _file.open(folder_name, std::ios::in);
  if(!_file) {
    return false;
  } else {
    return true;
  }
}

void IOHprofiler_csv_logger::activate_logger() {
  openIndex();
}

int IOHprofiler_csv_logger::openIndex() { 
  std::string experiment_folder_name = IOHprofiler_experiment_folder_name();
  return IOHprofiler_create_folder(experiment_folder_name);
}

int IOHprofiler_csv_logger::IOHprofiler_create_folder(std::string folder_name) { 
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW64__) || defined(__CYGWIN__)  
  if (mkdir(folder_name.c_str()) == 0) {
#else
  if (mkdir(folder_name.c_str(),S_IRWXU) == 0) {
#endif
    return 1;
  } else {
    IOH_error("Error on creating directory" + folder_name);
    return 0;
  }
}

/// \fn std::string IOHprofiler_csv_logger::IOHprofiler_experiment_folder_name()
///
/// To create a name of the folder logging files.
/// If there exists a files or a folder with the same name, the expected 
/// directory will be renamed by adding a suffix.
/// For example,
///     If a folder or file 'test' has already been in currect path, the 
///     expected directory will be renamed as 'test-1', 'test-2', ... 
///     until there is no such a folder or file. 
std::string IOHprofiler_csv_logger::IOHprofiler_experiment_folder_name() {
  std::string renamed_directory = this->output_directory + IOHprofiler_path_separator + this->folder_name;
  std::string temp_folder_name = this->folder_name;
  int index = 0;
  //while (fs::exists(renamed_directory.c_str())) {
  while (folder_exist(renamed_directory) ) {
    ++index;
    temp_folder_name = this->folder_name  + '-' + std::to_string(index);
    renamed_directory = this->output_directory + IOHprofiler_path_separator + temp_folder_name;
  }
  this->folder_name = temp_folder_name;
  return renamed_directory;
}

/// \fn void IOHprofiler_csv_logger::write_header()
///
/// This function is to be invoked before recoring evaluations (if evaluations == 0).
void IOHprofiler_csv_logger::write_header() {
  std::string sub_directory_name = this->output_directory + IOHprofiler_path_separator 
                        + this->folder_name + IOHprofiler_path_separator
                        + "data_f" + std::to_string(problem_id)
                        + "_" + problem_name;
  
  std::string dat_header = "\"function evaluation\" \"current f(x)\" \"best-so-far f(x)\" \"current af(x)+b\"  \"best af(x)+b\"";
  if(this->logging_parameters_name.size() != 0) {
    for (int i = 0; i != this->logging_parameters_name.size(); ++i) {
      dat_header = dat_header + " \"" + this->logging_parameters_name[i] + "\"";
    }
  }

  // if (!fs::exists(sub_directory_name.c_str())) {
  if (!folder_exist(sub_directory_name)) {
    IOHprofiler_create_folder(sub_directory_name.c_str());
  }

  if (complete_status()) {
    std::string cdat_name = sub_directory_name + IOHprofiler_path_separator 
                          + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                          + std::to_string(dimension) + ".cdat";

    if (this->cdat.is_open()) {
      this->cdat.close();
    }
    this->cdat.open(cdat_name.c_str(),std::ofstream::out | std::ofstream::app);
    this->cdat << dat_header << '\n';
  }

  if (interval_status()) {
    std::string idat_name = sub_directory_name + IOHprofiler_path_separator 
                          + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                          + std::to_string(dimension) + ".idat";

    if (this->idat.is_open()) {
      this->idat.close();
    }
    this->idat.open(idat_name.c_str(),std::ofstream::out | std::ofstream::app);
    this->idat << dat_header << '\n';
  }

  if (update_status()) {
    std::string dat_name = sub_directory_name + IOHprofiler_path_separator 
                            + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                            + std::to_string(dimension) + ".dat";

    if (this->dat.is_open()) {
      this->dat.close();
    }
    this->dat.open(dat_name.c_str(),std::ofstream::out | std::ofstream::app);
    this->dat << dat_header << '\n';
  }

  if (time_points_status()) {
    std::string tdat_name = sub_directory_name + IOHprofiler_path_separator 
                            + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                            + std::to_string(dimension) + ".tdat";

    if (this->tdat.is_open()) {
      this->tdat.close();
    }
    this->tdat.open(tdat_name.c_str(),std::ofstream::out | std::ofstream::app);
    this->tdat << dat_header << '\n';
  }
}

/// \fn void IOHprofiler_csv_logger::track_problem(int problem_id, int dimension, int instance)
///
/// This function is to be invoked by IOHprofiler_problem class.
/// To update info of current working problem, and to write headline in corresponding files.
void IOHprofiler_csv_logger::track_problem(const int problem_id, const int dimension, const int instance, const std::string problem_name, const IOH_optimization_type maximization_minimization_flag){
  /// Handle info of the previous problem.
  if (infoFile.is_open()) {
    write_info(this->instance, this->best_y[0], this->best_transformed_y[0], this->optimal_evaluations,
                                    this->last_y[0], this->last_transformed_y[0], this->last_evaluations);
  }

  this->optimal_evaluations = 0;
  this->last_evaluations = 0;

  /// TO DO: Update the method of initializing this value.

  if (maximization_minimization_flag == IOH_optimization_type::Maximization) {
    this->best_y.clear();
    this->best_y.push_back(-DBL_MAX);
    this->best_transformed_y.clear();
    this->best_transformed_y.push_back(-DBL_MAX);
    this->last_y.clear();
    this->last_y.push_back(-DBL_MAX);
    this->last_transformed_y.clear();
    this->last_transformed_y.push_back(-DBL_MAX);
  } else {
    this->best_y.clear();
    this->best_y.push_back(DBL_MAX);
    this->best_transformed_y.clear();
    this->best_transformed_y.push_back(DBL_MAX);
    this->last_y.clear();
    this->last_y.push_back(DBL_MAX);
    this->last_transformed_y.clear();
    this->last_transformed_y.push_back(DBL_MAX);
  }
  
  reset_observer(maximization_minimization_flag);

  this->problem_id = problem_id;
  this->dimension = dimension;
  this->instance = instance;
  this->problem_name = problem_name;
  this->maximization_minimization_flag = maximization_minimization_flag;
  
  openInfo(problem_id,dimension,problem_name);
  header_flag = false;
}


/// \fn void IOHprofiler_csv_logger::track_problem(int problem_id, int dimension, int instance)
///
/// This function is to be invoked by IOHprofiler_problem class.
/// To update info of current working problem, and to write headline in corresponding files.
void IOHprofiler_csv_logger::track_problem(const IOHprofiler_problem<int> & problem) {
  // this->tracked_problem_int = nullptr;
  // this->tracked_problem_double = nullptr;

  this->track_problem(
    problem.IOHprofiler_get_problem_id(), 
    problem.IOHprofiler_get_number_of_variables(), 
    problem.IOHprofiler_get_instance_id(),
    problem.IOHprofiler_get_problem_name(),
    problem.IOHprofiler_get_optimization_type()
  );
  
  // this->problem_type = problem.IOHprofiler_get_problem_type();
  // this->tracked_problem_int = std::make_shared<IOHprofiler_problem<int> >(problem);
}

void IOHprofiler_csv_logger::track_problem(const IOHprofiler_problem<double> & problem) {
  // this->tracked_problem_int = nullptr;
  // this->tracked_problem_double = nullptr;
  this->track_problem(
    problem.IOHprofiler_get_problem_id(), 
    problem.IOHprofiler_get_number_of_variables(), 
    problem.IOHprofiler_get_instance_id(),
    problem.IOHprofiler_get_problem_name(),
    problem.IOHprofiler_get_optimization_type()
  );
  
  // this->problem_type = problem.IOHprofiler_get_problem_type();
  // this->tracked_problem_double = std::make_shared<IOHprofiler_problem<double> >(problem);
}

void IOHprofiler_csv_logger::track_suite(std::string suite_name){
  this->suite_name = suite_name;
}

void IOHprofiler_csv_logger::set_parameters(const std::vector<std::shared_ptr<double> > &parameters) {
  if (this->logging_parameters.size() != 0) {
    this->logging_parameters.clear();
    this->logging_parameters_name.clear();
  }

  for (size_t i = 0; i != parameters.size(); i++) {
    this->logging_parameters.push_back(parameters[i]);
    this->logging_parameters_name.push_back("parameter" + std::to_string(i+1));
  }
}

void IOHprofiler_csv_logger::set_parameters(const std::vector<std::shared_ptr<double> > &parameters, const std::vector<std::string> &parameters_name) {
  if (parameters_name.size() != parameters.size()) {
    IOH_error("Parameters and their names are given with different size.");
  }
  if (this->logging_parameters.size() != 0) {
    this->logging_parameters.clear();
    this->logging_parameters_name.clear();
  }
  for (size_t i = 0; i != parameters.size(); i++) {
    this->logging_parameters.push_back(parameters[i]);
    this->logging_parameters_name.push_back(parameters_name[i]);
  }
}

void IOHprofiler_csv_logger::do_log(const std::vector<double> & log_info) {
  this->write_line( (size_t)(log_info[0]),log_info[1],log_info[2],log_info[3],log_info[4]);
};

void IOHprofiler_csv_logger::write_line(const std::vector<double> & log_info) {
  this->write_line( (size_t)(log_info[0]),log_info[1],log_info[2],log_info[3],log_info[4]);
};


/// todo The precision of double values.
void IOHprofiler_csv_logger::write_line(const size_t evaluations, const double y, const double best_so_far_y,
                 const double transformed_y, const double best_so_far_transformed_y) {
  if (header_flag == false) {
    this->write_header();
    header_flag = true;
  }

  this->last_evaluations = evaluations;
  this->last_y[0] = y;
  this->last_transformed_y[0] = transformed_y;

  bool cdat_flag = complete_trigger();
  bool idat_flag = interval_trigger(evaluations);
  bool dat_flag = update_trigger(transformed_y,maximization_minimization_flag);
  bool tdat_flag = time_points_trigger(evaluations);

  bool need_write =  cdat_flag || idat_flag || dat_flag || tdat_flag;


  if (need_write) {
    std::string written_line = std::to_string(evaluations) + " " + std::to_string(y) + " "
                             + std::to_string(best_so_far_y) + " " + std::to_string(transformed_y) + " "
                             + std::to_string(best_so_far_transformed_y);
    
    if (this->logging_parameters.size() != 0) {
      for (size_t i = 0; i != this->logging_parameters.size(); i++) {
        written_line += " ";
        written_line += std::to_string(*logging_parameters[i]);
      }
    }
    
    written_line += '\n';

    if (cdat_flag) {
      if (!this->cdat.is_open()) {
        IOH_error("*.cdat file is not open");
      }
      this->cdat << written_line;
    }

    if (idat_flag) {
      if (!this->idat.is_open()) {
        IOH_error("*.idat file is not open");
      }
      this->idat << written_line;
    }

    if (dat_flag) {
      if (!this->dat.is_open()) {
        IOH_error("*.dat file is not open");
      }
      this->dat << written_line;
    }

    if (tdat_flag) {
      if (!this->tdat.is_open()) {
        IOH_error("*.tdat file is not open");
      }
      this->tdat << written_line;
    }
  }

  if (compareObjectives(transformed_y,this->best_transformed_y[0],this->maximization_minimization_flag)) {
    this->update_logger_info(evaluations,y,transformed_y);
  }
};

void IOHprofiler_csv_logger::openInfo(int problem_id, int dimension, std::string problem_name) {
  std::string titleflag = "";
  std::string optimization_type;
  if (this->maximization_minimization_flag == IOH_optimization_type::Maximization) {
    optimization_type = "T";
  } else {
    optimization_type = "F";
  }

  if (problem_id != this->last_problem_id) {
    this->infoFile.close();
    std::string infoFile_name = this->output_directory + IOHprofiler_path_separator 
                          + this->folder_name + IOHprofiler_path_separator
                          + "IOHprofiler_f" + std::to_string(problem_id)
                          + "_" + problem_name
                          + ".info";
    // if (fs::exists(infoFile_name.c_str())) {
    if (folder_exist(infoFile_name)) {
      titleflag = "\n";
    }
    this->infoFile.open(infoFile_name.c_str(),std::ofstream::out | std::ofstream::app);
    this->infoFile << titleflag;
    this->infoFile << "suite = \"" << this->suite_name << "\", funcId = " <<  problem_id << ", funcName = \""<< problem_name << "\", DIM = "  << dimension << ", maximization = \"" << optimization_type << "\", algId = \"" << this->algorithm_name << "\", algInfo = \"" << this->algorithm_info << "\"";
	  for (std::map<std::string,std::string>::iterator iter = this->attr_name_value.begin(); iter != this->attr_name_value.end(); iter++) {
		  this->infoFile << ", \"" << iter->first << "=" << iter->second;
	  }
    this->infoFile << "\n%\n";
    this->infoFile << "data_f" << problem_id << "_" << problem_name << "/IOHprofiler_f" << problem_id << "_DIM" << dimension << ".dat";     
    this->last_problem_id = problem_id;
    this->last_dimension = dimension;
  } else if (dimension != this->last_dimension) {
    this->infoFile << "\nsuite = \"" << this->suite_name << "\", funcId = " << problem_id << ", funcName = \""<< problem_name << "\", DIM = " << dimension << ", maximization = \"" << optimization_type << "\", algId = \"" << this->algorithm_name << "\", algInfo = \"" << this->algorithm_info << "\"";
    for (std::map<std::string,std::string>::iterator iter = this->attr_name_value.begin(); iter != this->attr_name_value.end(); iter++) {
		  this->infoFile << ", \"" << iter->first << "=" << iter->second;
	  }
    this->infoFile << "\n%\n";
    this->infoFile << "data_f" << problem_id << "_" << problem_name << "/IOHprofiler_f" << problem_id << "_DIM" << dimension << ".dat";    
    this->last_problem_id = problem_id;
    this->last_dimension = dimension;
  }
}

void IOHprofiler_csv_logger::write_info(int instance, double best_y, double best_transformed_y, int evaluations, 
                                        double last_y, double last_transformed_y, int last_evaluations) {
  if (!infoFile.is_open()) {
    IOH_error("write_info(): writing info into unopened infoFile");
  }
  infoFile << ", " << instance << ":" << evaluations << "|" << best_y; 

  bool need_write = (evaluations != last_evaluations);
  if (need_write) {
    std::string written_line = std::to_string(last_evaluations) + " " + std::to_string(last_y) + " "
                              + std::to_string(best_y) + " " + std::to_string(last_y) + " "
                              + std::to_string(best_transformed_y);
    
    if (this->logging_parameters.size() != 0) {
      for (size_t i = 0; i != this->logging_parameters.size(); i++) {
        written_line += " ";
        written_line += std::to_string(*logging_parameters[i]);
      }
    }
    
    written_line += '\n';
    if (this->cdat.is_open()) {
      this->cdat << written_line;
    }
    if (this->idat.is_open()) {
      this->idat << written_line;
    }
    if (this->dat.is_open()) {
      this->dat << written_line;
    }
    if (this->tdat.is_open()) {
      this->tdat << written_line;
    }
  }
}

/// \fn void IOHprofiler_csv_logger::update_logger_info(size_t optimal_evaluations, std::vector<double> found_optimal)
/// This functions is to update infomation to be used in *.info files.
void IOHprofiler_csv_logger::update_logger_info(size_t optimal_evaluations, double y, double transformed_y) {
  this->optimal_evaluations = optimal_evaluations;
  this->best_y[0] =  y;
  this->best_transformed_y[0] = transformed_y;
}


void IOHprofiler_csv_logger::add_attribute(std::string name, double value) {
  this->attr_name_value[name] = std::to_string(value);
}

void IOHprofiler_csv_logger::add_attribute(std::string name, int value) {
  this->attr_name_value[name] = std::to_string(value);
}

void IOHprofiler_csv_logger::add_attribute(std::string name, float value) {
  this->attr_name_value[name] = std::to_string(value);
}

void IOHprofiler_csv_logger::add_attribute(std::string name, std::string value) {
  this->attr_name_value[name] = value;
}

void IOHprofiler_csv_logger::delete_attribute(std::string name) {
  this->attr_name_value.erase(name);
}

void IOHprofiler_csv_logger::clear_logger() {
  if (infoFile.is_open()) {
    write_info(this->instance, this->best_y[0], this->best_transformed_y[0], this->optimal_evaluations,
              this->last_y[0], this->last_transformed_y[0], this->last_evaluations);
    infoFile.close();
  }

  /// Close .dat files after .info file to record evaluations of the last iteration.
  if (cdat.is_open()) {
    cdat.close();
  }
  if (idat.is_open()) {
    idat.close();
  }
  if (dat.is_open()) {
    dat.close();
  }
  if (tdat.is_open()) {
    tdat.close();
  }

  // if(this->tracked_problem_double != nullptr) {
  //   this->tracked_problem_double = nullptr;
  // }

  // if(this->tracked_problem_int != nullptr) {
  //   this->tracked_problem_int = nullptr;
  // }
}

