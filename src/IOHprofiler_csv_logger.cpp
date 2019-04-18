#include "IOHprofiler_csv_logger.h"

int IOHprofiler_csv_logger::openIndex() { 
  std::string experiment_folder_name = IOHprofiler_experiment_folder_name();
  IOHprofiler_create_folder(experiment_folder_name);
}


int IOHprofiler_csv_logger::IOHprofiler_create_folder(std::string folder_name) { 
  if(fs::create_directory(folder_name)) {
    return 1;
  } else {
    IOH_error("Error on creating directory");
    return 0;
  }
}


// To create a folder for logging files.
// If there exists a files or a folder with the same name, the expected 
// directory will be renamed by adding a suffix.
// For example,
//     If a folder or file 'test' has already been in currect path, the 
//     expected directory will be renamed as 'test-1', 'test-2', ... 
//     until there is no such a folder or file. 
std::string IOHprofiler_csv_logger::IOHprofiler_experiment_folder_name() {
  std::string renamed_directory = this->output_directory + IOHprofiler_path_separator + this->folder_name;
  std::string temp_folder_name;
  int index = 0;
  while(fs::exists(renamed_directory.c_str())) {
    ++index;
    temp_folder_name = this->folder_name  + '-' + std::to_string(index);
    renamed_directory = this->output_directory + IOHprofiler_path_separator + temp_folder_name;
  }
  this->folder_name = temp_folder_name;
  return renamed_directory;
}


void IOHprofiler_csv_logger::target_problem(int problem_id, int dimension, int instance){
  this->problem_id = problem_id;
  this->dimension = dimension;
  this->instance = instance;
  
  std::string sub_directory_name = this->output_directory + IOHprofiler_path_separator 
                          + this->folder_name + IOHprofiler_path_separator
                          + "data_f" + std::to_string(problem_id);
  
  
  const std::string dat_header = "\"function evaluation\" \"current f(x)\" \"best-so-far f(x)\" \"current af(x)+b\"  \"best af(x)+b\"";

  // Create the specific folder storing data for the problem, if it does not exist. 
  if(!fs::exists(sub_directory_name.c_str())){
    if(!fs::create_directory(sub_directory_name.c_str())) {
      IOH_error("Error on creating sub-directory for problem " + std::to_string(problem_id));
    }
  }

  std::string infoFile_name = this->output_directory + IOHprofiler_path_separator 
                            + this->folder_name + IOHprofiler_path_separator
                            + "IOHprofiler_f" + std::to_string(problem_id) + "_i1"
                            + ".info"; 
  if(infoFile.is_open()) infoFile.close();
  infoFile.open(infoFile_name.c_str());


  if(complete_status()){
    std::string cdat_name = sub_directory_name + IOHprofiler_path_separator 
                          + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                          + std::to_string(dimension) + "_i1.cdat";

    if(cdat.is_open()) cdat.close();
    cdat.open(cdat_name.c_str());
    cdat << dat_header;
  }

  if(interval_status()){
    std::string idat_name = sub_directory_name + IOHprofiler_path_separator 
                          + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                          + std::to_string(dimension) + "_i1.idat";

    if(idat.is_open()) idat.close();
    idat.open(idat_name.c_str());
    idat << dat_header;
  }

  if(update_status()) {
    std::string dat_name = sub_directory_name + IOHprofiler_path_separator 
                            + "IOHprofiler_f" + std::to_string(problem_id) + "_DIM"
                            + std::to_string(dimension) + "_i1.dat";

    if(dat.is_open()) dat.close();
    dat.open(dat_name.c_str());
    dat << dat_header;
  }
  
};

void IOHprofiler_csv_logger::write_line(size_t evaluations, double y, double best_so_far_y,
                           double transformed_y, double best_so_far_transformed_y,
                           std::vector<double> parameters) {
  std::string written_line = std::to_string(evaluations) + " " + std::to_string(y) + " "
                           + std::to_string(best_so_far_y) + " " + std::to_string(transformed_y) + " "
                           + std::to_string(best_so_far_transformed_y);
  for (std::vector<double>::iterator iter = parameters.begin(); iter != parameters.end(); ++iter) {
    written_line = written_line + " " + std::to_string(*iter);
  }
  written_line += "\n";

  if(complete_trigger()) {
    if(!cdat.is_open()) IOH_error("*.cdat file is not open");
    cdat << written_line;
  }
  if(interval_trigger(evaluations)) {
    if(!idat.is_open()) IOH_error("*.idat file is not open");
    idat << written_line;
  }
  if(update_trigger(transformed_y)) {
    if(!dat.is_open()) IOH_error("*.dat file is not open");
    dat << written_line;
  }
};

void IOHprofiler_csv_logger::write_line(size_t evaluations, double y, double best_so_far_y,
                           double transformed_y, double best_so_far_transformed_y) {
  std::string written_line = std::to_string(evaluations) + " " + std::to_string(y) + " "
                           + std::to_string(best_so_far_y) + " " + std::to_string(transformed_y) + " "
                           + std::to_string(best_so_far_transformed_y) + "\n";

  if(complete_trigger()) {
    if(!cdat.is_open()) IOH_error("*.cdat file is not open");
    cdat << written_line;
  }
  if(interval_trigger(evaluations)) {
    if(!idat.is_open()) IOH_error("*.idat file is not open");
    idat << written_line;
  }
  if(update_trigger(transformed_y)) {
    if(!dat.is_open()) IOH_error("*.dat file is not open");
    dat << written_line;
  }
};