#include "ioh.hpp"


std::vector<std::string> read_file(const fs::path& path){
    std::vector<std::string> output;
    std::ifstream stream (path);
    std::string line;
    while(std::getline(stream, line)){
        if (!line.empty()){
            std::cout << line << std::endl;
            output.push_back(line);
        }
    }
    return output;
}

int main(){
    std::cout << "hebben we cout " << std::endl;
}