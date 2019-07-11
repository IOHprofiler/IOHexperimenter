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
  IOHprofiler_create_folder(experiment_folder_name);
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

/// \fn void IOHprofiler_csv_logger::target_problem(int problem_id, int dimension, int instance)
///
/// This function is to be invoked by IOHprofiler_problem class.
/// To update info of current working problem, and to write headline in corresponding files.
void IOHprofiler_csv_logger::target_problem(int problem_id, int dimension, int instance){
  /// Handle info of the previous problem.
  if(infoFile.is_open()) write_info(this->instance,this->found_optimal[0],this->optimal_evaluations);

  this->optimal_evaluations = 0;
  this->found_optimal.clear();
  reset_observer();

  this->problem_id = problem_id;
  this->dimension = dimension;
  this->instance = instance;

  std::string sub_directory_name = this->output_directory + IOHprofiler_path_separator 
                          + this->folder_name + IOHprofiler_path_separator
                          + "data_f" + std::to_string(problem_id);
  
  
  const std::string dat_header = "\"function evaluation\" \"current f(x)\" \"best-so-far f(x)\" \"current af(x)+b\"  \"best af(x)+b\"";

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
    this->cdat.open(cdat_name.c_str(),std::ofstream::app);
    this->cdat << dat_header << '\n';
  }

  if (interval_status()) {
    std::string idat_name = sub_directory_name + IOHprofiler_path_separator 
                          + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                          + std::to_string(dimension) + ".idat";

    if (this->idat.is_open()) {
      this->idat.close();
    }
    this->idat.open(idat_name.c_str(),std::ofstream::app);
    this->idat << dat_header << '\n';
  }

  if (update_status()) {
    std::string dat_name = sub_directory_name + IOHprofiler_path_separator 
                            + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                            + std::to_string(dimension) + ".dat";

    if (this->dat.is_open()) {
      this->dat.close();
    }
    this->dat.open(dat_name.c_str(),std::ofstream::app);
    this->dat << dat_header << '\n';
  }

  if (time_points_status()) {
    std::string tdat_name = sub_directory_name + IOHprofiler_path_separator 
                            + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                            + std::to_string(dimension) + ".tdat";

    if (this->tdat.is_open()) {
      this->tdat.close();
    }
    this->tdat.open(tdat_name.c_str(),std::ofstream::app);
    this->tdat << dat_header << '\n';
  }
  openInfo(problem_id,dimension);
  
};

void IOHprofiler_csv_logger::target_suite(std::string suite_name){
  this->suite_name = suite_name;
}

/// todo The precision of double values.
void IOHprofiler_csv_logger::write_line(size_t evaluations, double y, double best_so_far_y,
                           double transformed_y, double best_so_far_transformed_y,
                           std::vector<double> parameters) {

  std::string written_line = std::to_string(evaluations) + " " + std::to_string(y) + " "
                           + std::to_string(best_so_far_y) + " " + std::to_string(transformed_y) + " "
                           + std::to_string(best_so_far_transformed_y);
  for (std::vector<double>::iterator iter = parameters.begin(); iter != parameters.end(); ++iter) {
    written_line = written_line + " " + std::to_string(*iter);
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
};

/// todo The precision of double values.
void IOHprofiler_csv_logger::write_line(size_t evaluations, double y, double best_so_far_y,
                           double transformed_y, double best_so_far_transformed_y) {

  std::string written_line = std::to_string(evaluations) + " " + std::to_string(y) + " "
                           + std::to_string(best_so_far_y) + " " + std::to_string(transformed_y) + " "
                           + std::to_string(best_so_far_transformed_y);
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
};

void IOHprofiler_csv_logger::openInfo(int problem_id, int dimension) {
  std::string titleflag = "";
  
  if(problem_id != this->last_problem_id) {
    this->infoFile.close();
    std::string infoFile_name = this->output_directory + IOHprofiler_path_separator 
                          + this->folder_name + IOHprofiler_path_separator
                          + "IOHprofiler_f" + std::to_string(problem_id)
                          + ".info";
    if (fs::exists(infoFile_name.c_str())) {
      titleflag = "\n";
    }
    this->infoFile.open(infoFile_name.c_str(),std::ofstream::app);
    this->infoFile << titleflag;
    this->infoFile << "suite = " << this->suite_name << ", funcId = " << problem_id << ", DIM = " << dimension << ", algId = " << this->algorithm_name << ", algInfo = " << this->algorithm_info << "\n%\n";
    this->infoFile << "data_f" << problem_id << "/IOHprofiler_f" << problem_id << "_DIM" << dimension << ".dat";     
    this->last_problem_id = problem_id;
    this->last_dimension = dimension;
  } else if (dimension != this->last_dimension) {
    this->infoFile << "\nsuite = " << this->suite_name << ", funcId = " << problem_id << ", DIM = " << dimension << ", algId = " << this->algorithm_name << ", algInfo = " << this->algorithm_info << "\n%\n";
    this->infoFile << "data_f" << problem_id << "/IOHprofiler_f" << problem_id << "_DIM" << dimension << ".dat";    
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
void IOHprofiler_csv_logger::update_logger_info(size_t optimal_evaluations, std::vector<double> found_optimal) {
  this->optimal_evaluations = optimal_evaluations;
  copyVector(found_optimal,this->found_optimal);
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

