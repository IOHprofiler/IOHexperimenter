#include "IOHprofiler_common.h"

void IOH_error(std::string error_info) {
  std::cout << "IOH_ERROR_INFO : " << error_info << std::endl;
  exit(1);
}

void IOH_warning(std::string warning_info) {
  std::cout << "IOH_WARNING_INFO : " << warning_info << std::endl;
}

void IOH_log(std::string log_info) {
  std::cout << "IOH_LOG_INFO : " << log_info << std::endl;
}

void IOH_log(std::string log_info, std::ofstream &log_stream) {
  log_stream << "IOH_LOG_INFO : " << log_info << std::endl;
}