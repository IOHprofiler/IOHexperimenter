#include "IOHprofiler_logger.h"

static const char *PBO_file_header_str = "\"function evaluation\" "
    "\"current f(x)\" "
    "\"best-so-far f(x)\" "
    "\"current af(x)+b\"  "
    "\"best af(x)+b\" ";

static const char *PBO_file_para_str = "\"%s\" ";

void IOHprofiler_logger::IOHprofiler_setting(int number_evaluation_triggers, int number_interval_triggers, int complete_triggers) { 
  this->number_evaluation_triggers = number_evaluation_triggers;
  this->number_interval_triggers = number_interval_triggers;
  this->complete_triggers = complete_triggers;
}

int IOHprofiler_logger::IOHprofiler_logger_openIndex() { 


  string current_file_path = output_path + IOHprofiler_path_separator + output_filename;

  int countPath = 1;
  string creatingPath = current_file_path;
  while(access(creatingPath.c_str()) != -1) { 
    creatingPath = current_file_path + "-00" + toString(countPath);
    ++countPath;
  }
  IOHprofiler_create_folder(creatingPath);

  string current_file_name = "";

  current_file_path.append("_f").append(std::toString(function_id)).append("_i1.info");

  infoFile.open(current_file_path.c_str(),ios::app);
  infoFile << "suite = PBO, funcId = " << function_id << ", DIM = " << dimension << ", algId = \'" + algorithm_name + "\' algInfo = " + algorithm_info + '\n';  
  infoFile << "%\n";
}

int IOHprofiler_logger::IOHprofiler_logger_initilizing_files();
  if(complete_triggers == 1) { 
    current_file_path =  output_path + output_filename;
    current_file_path.append("data_f").append(std::toString(function_id)).append("/IOHprofiler_f").append(std::toString(function_id)).append("_DIM").append(std::toString(dimension)).append("_i1.cdat");
    
  }
}

int IOHprofiler_create_folder(string path) { 
	string directory_name = IOHprofiler_folder_name(path);
	if(boost::filesystem::create_directory(directory_name)) {
		return 1;
	} else {
		printf("Error on creating directory\n");
		return 0;
	}
}


// Create a new folder with privilieges for the user.
// If the folder exist, rename the creating folder with suffix.
string IOHprofiler_logger::IOHprofiler_folder_name(string path) {
	string renamed_path = path;
	string suffix;
	int index = 0;
	while(boost::filesystem::exist(renamed_path.c_str()) && 
		  boost::filesystem::is_directory(renamed_path.c_str())) {
		sprintf(suffix,"%3d",index);
		++index;
		renamed_path = path + suffix;
	}
	return renamed_path;
}