#include "../include/ioh/common/utils.hpp"



namespace ioh
{
	namespace common
	{

		namespace log
		{
			void error(std::string error_info)
			{
				std::cerr << "IOH_ERROR_INFO : " << error_info << std::endl;
				exit(1);
			}

			void warning(std::string warning_info)
			{
				std::cout << "IOH_WARNING_INFO : " << warning_info << std::endl;
			}

			void info(std::string log_info)
			{
				std::cout << "IOH_LOG_INFO : " << log_info << std::endl;
			}

			void info(std::string log_info, std::ofstream& log_stream)
			{
				log_stream << "IOH_LOG_INFO : " << log_info << std::endl;

		}
		}
		
	}
}
