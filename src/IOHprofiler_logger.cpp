#include "IOHprofiler_logger.h"

static const char *PBO_file_header_str = "\"function evaluation\" "
    "\"current f(x)\" "
    "\"best-so-far f(x)\" "
    "\"current af(x)+b\"  "
    "\"best af(x)+b\" ";

static const char *PBO_file_para_str = "\"%s\" ";

// void IOHprofiler_logger::IOHprofiler_setting(int number_evaluation_triggers, int number_interval_triggers, int complete_triggers) { 
//   this->number_evaluation_triggers = number_evaluation_triggers;
//   this->number_interval_triggers = number_interval_triggers;
//   this->complete_triggers = complete_triggers;
// }

int IOHprofiler_logger::IOHprofiler_logger_openIndex() { 


  std::string experiment_folder_name = IOHprofiler_experiment_folder_name();

  IOHprofiler_create_folder(experiment_folder_name);

  // std::string current_file_name = "";

  // current_file_path.append("_f").append(std::toString(function_id)).append("_i1.info");

  // infoFile.open(current_file_path.c_str(),ios::app);
  // infoFile << "suite = PBO, funcId = " << function_id << ", DIM = " << dimension << ", algId = \'" + algorithm_name + "\' algInfo = " + algorithm_info + '\n';  
  // infoFile << "%\n";
}

// int IOHprofiler_logger::IOHprofiler_logger_initilizing_files();
//   if(complete_triggers == 1) { 
//     current_file_path =  output_path + output_filename;
//     current_file_path.append("data_f").append(std::toString(function_id)).append("/IOHprofiler_f").append(std::toString(function_id)).append("_DIM").append(std::toString(dimension)).append("_i1.cdat");
    
//   }
// }



// Both folder structure and file names format are still following COCO style. `sub_directory_name` and 
// `filename_prefix` need to be update in the future if necessary.
int IOHprofiler_logger::IOHprofiler_problem_logger(int problem_id, int dimension, int instance, std::string log_line) {
  std::sub_directory_name = this->output_directory + IOHprofiler_path_separator 
                          + this->folder_name + IOHprofiler_path_separator
                          + "data_f" + std::to_string(problem_id);
  
  std::filename_prefix = sub_directory_name + IOHprofiler_path_separator + "IOHprofiler_f" 
                       + std::string(problem_id) + "_DIM" + std::string(dimension) + "i"
                       + std::string(instance);

  if(!fs::exists(sub_directory_name.c_str())){
    if(!fs::create_directory(folder_name)) {
      printf("Error on creating sub-directory for problem %d\n", problem_id);
    }
  }
}



int IOHprofiler_logger::IOHprofiler_create_folder(std::string folder_name) { 
  if(fs::create_directory(folder_name)) {
    return 1;
  } else {
    printf("Error on creating directory\n");
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
std::string IOHprofiler_logger::IOHprofiler_experiment_folder_name() {
  std::string renamed_directory = this->output_directory + IOHprofiler_path_separator + this->folder_name;
  int index = 0;
  while(fs::exists(renamed_directory.c_str())) {
    ++index;
    this->folder_name = this->folder_name  + '-' + std::to_string(index);
    renamed_directory = this->output_directory + IOHprofiler_path_separator + this->folder_name;
  }
  return renamed_directory;
}


