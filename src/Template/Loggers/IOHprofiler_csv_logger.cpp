/// \file IOHprofiler_csv_logger.cpp
/// \brief cpp file for class IOHprofiler_csv_logger.
///
/// A detailed file description.
///
/// \author Furong Ye
/// \date 2019-06-27
#include "IOHprofiler_csv_logger.h"

void IOHprofiler_csv_logger::activate_logger() {
  openIndex();
}

int IOHprofiler_csv_logger::openIndex() { 
  std::string experiment_folder_name = IOHprofiler_experiment_folder_name();
  return IOHprofiler_create_folder(experiment_folder_name);
}


int IOHprofiler_csv_logger::IOHprofiler_create_folder(std::string folder_name) { 
  if (fs::create_directory(folder_name)) {
    return 1;
  } else {
    IOH_error("Error on creating directory");
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
  while (fs::exists(renamed_directory.c_str())) {
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

  if (!fs::exists(sub_directory_name.c_str())) {
    if (!fs::create_directory(sub_directory_name.c_str())) {
      IOH_error("Error on creating sub-directory for problem " + std::to_string(problem_id));
    }
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

/// \fn void IOHprofiler_csv_logger::target_problem(int problem_id, int dimension, int instance)
///
/// This function is to be invoked by IOHprofiler_problem class.
/// To update info of current working problem, and to write headline in corresponding files.
void IOHprofiler_csv_logger::target_problem(int problem_id, int dimension, int instance, std::string problem_name){
  /// Handle info of the previous problem.
  if(infoFile.is_open()) write_info(this->instance,this->found_optimal[0],this->optimal_evaluations);

  this->optimal_evaluations = 0;

  /// TO DO: Update the method of initializing this value.
  this->found_optimal.clear();
  this->found_optimal.push_back(-DBL_MAX);
  
  reset_observer();

  this->problem_id = problem_id;
  this->dimension = dimension;
  this->instance = instance;
  this->problem_name = problem_name;
  
  openInfo(problem_id,dimension,problem_name);
  
};

void IOHprofiler_csv_logger::target_suite(std::string suite_name){
  this->suite_name = suite_name;
}

void IOHprofiler_csv_logger::set_parameters(const std::vector<std::shared_ptr<double>> &parameters) {
  if (this->logging_parameters.size() != 0) {
    this->logging_parameters.clear();
    this->logging_parameters_name.clear();
  }

  for (size_t i = 0; i != parameters.size(); i++) {
    this->logging_parameters.push_back(parameters[i]);
    this->logging_parameters_name.push_back("parameter" + std::to_string(i+1));
  }
}

void IOHprofiler_csv_logger::set_parameters(const std::vector<std::shared_ptr<double>> &parameters, const std::vector<std::string> &parameters_name) {
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

void IOHprofiler_csv_logger::write_line(const std::vector<double> &logger_info) {
  this->write_line( (size_t)(logger_info[0]),logger_info[1],logger_info[2],logger_info[3],logger_info[4]);
};


/// todo The precision of double values.
void IOHprofiler_csv_logger::write_line(const size_t &evaluations, const double &y, const double &best_so_far_y,
                 const double &transformed_y, const double &best_so_far_transformed_y) {
  if (evaluations == 1) {
    this->write_header();
  }

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
  if (complete_trigger()) {
    if (!this->cdat.is_open()) {
      IOH_error("*.cdat file is not open");
    }
    this->cdat << written_line;
  }
  if (interval_trigger(evaluations)) {
    if (!this->idat.is_open()) {
      IOH_error("*.idat file is not open");
    }
    this->idat << written_line;
  }
  if (update_trigger(transformed_y)) {
    if (!this->dat.is_open()) {
      IOH_error("*.dat file is not open");
    }
    this->dat << written_line;
  }
  if (time_points_trigger(evaluations)) {
    if (!this->tdat.is_open()) {
      IOH_error("*.tdat file is not open");
    }
    this->tdat << written_line;
  }

  if (transformed_y > this->found_optimal[0]) {
    this->update_logger_info(evaluations,transformed_y);
  }
};

void IOHprofiler_csv_logger::openInfo(int problem_id, int dimension, std::string problem_name) {
  std::string titleflag = "";
  
  if(problem_id != this->last_problem_id) {
    this->infoFile.close();
    std::string infoFile_name = this->output_directory + IOHprofiler_path_separator 
                          + this->folder_name + IOHprofiler_path_separator
                          + "IOHprofiler_f" + std::to_string(problem_id)
                          + "_" + problem_name
                          + ".info";
    if (fs::exists(infoFile_name.c_str())) {
      titleflag = "\n";
    }
    this->infoFile.open(infoFile_name.c_str(),std::ofstream::out | std::ofstream::app);
    this->infoFile << titleflag;
    this->infoFile << "suite = " << this->suite_name << ", funcId = " << problem_id << ", DIM = " << dimension << ", algId = " << this->algorithm_name << ", algInfo = " << this->algorithm_info << "\n%\n";
    this->infoFile << "data_f" << problem_id << "_" << problem_name << "/IOHprofiler_f" << problem_id << "_DIM" << dimension << ".dat";     
    this->last_problem_id = problem_id;
    this->last_dimension = dimension;
  } else if (dimension != this->last_dimension) {
    this->infoFile << "\nsuite = " << this->suite_name << ", funcId = " << problem_id << ", DIM = " << dimension << ", algId = " << this->algorithm_name << ", algInfo = " << this->algorithm_info << "\n%\n";
    this->infoFile << "data_f" << problem_id << "_" << problem_name << "/IOHprofiler_f" << problem_id << "_DIM" << dimension << ".dat";    
    this->last_problem_id = problem_id;
    this->last_dimension = dimension;
  }
}

void IOHprofiler_csv_logger::write_info(int instance, double optimal, int evaluations) {
  if (!infoFile.is_open()) {
    IOH_error("write_info(): writing info into unopened infoFile");
  }
  infoFile << ", " << instance << ":" << evaluations << "|" << optimal; 
}

/// \fn void IOHprofiler_csv_logger::update_logger_info(size_t optimal_evaluations, std::vector<double> found_optimal)
/// This functions is to update infomation to be used in *.info files.
void IOHprofiler_csv_logger::update_logger_info(size_t optimal_evaluations, double found_optimal) {
  this->optimal_evaluations = optimal_evaluations;
  this->found_optimal[0] =  found_optimal;
};

void IOHprofiler_csv_logger::clear_logger() {
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
  if (infoFile.is_open()) {
    write_info(this->instance,this->found_optimal[0],this->optimal_evaluations);
    infoFile.close();
  }
}

