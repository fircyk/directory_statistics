#include <iostream>
#include <filesystem>
#include <string>
#include <thread>
#include <cstdlib>
#include <fstream>
#include <threadpoolapiset.h>

namespace fs = std::filesystem;

int main()
{
    int dir_count = 0;
    int file_count = 0;
    int txt_count = 0;
    int c_and_cpp_count = 0;
    int h_count = 0;

    int cpp_lines = 0;
    int h_lines = 0;
    int txt_lines = 0;

    std::cout << fs::current_path() << std::endl;

    fs::path work_path = fs::current_path().parent_path();
    work_path = work_path / "directory_statistics";


    for(const fs::directory_entry &path : fs::recursive_directory_iterator(work_path)){
       // std::cout << path << std::endl;
        std::string string_path = path.path().u8string();
        std::cout << path.path().u8string() << std::endl;

        if((string_path.find(".cpp") != std::string::npos) || (string_path.find(".c") != std::string::npos)){
            c_and_cpp_count++;
            std::ifstream file_cpp;

            file_cpp.open(string_path, std::fstream::in);
            std::string read;
            while(file_cpp >> read){
                cpp_lines++;
            }
            read.clear();

            std::cout << "c/cpp lines: " << cpp_lines << std::endl;

        }
        if((string_path.find(".hpp") != std::string::npos) || (string_path.find(".h") != std::string::npos)){
            h_count++;
            std::cout << "jestemtu" << std::endl;
            std::cout << "hpp: " << h_count << std::endl;

            std::ifstream file_h;

            file_h.open(string_path, std::fstream::in);
            std::string read;
            while(file_h >> read){
                h_lines++;
            }

            std::cout << "h lines: " << h_lines << std::endl;

        }
        if(string_path.find(".txt") != std::string::npos){
            txt_count++;
            std::cout << "txtp: " << txt_count << std::endl;

            std::ifstream file_txt;

            file_txt.open(string_path, std::fstream::in);
            std::string read;
            while(file_txt >> read){
                txt_lines++;
            }
            std::cout << "txt lines: " << txt_lines << std::endl;

        }
        if(path.is_directory()){
            dir_count++;
        }
        if(path.is_character_file()){
            file_count++;
        }





    }



    std::cout << work_path << std::endl;
    std::cout << "cpp: " << c_and_cpp_count << std::endl;
    std::cout << "h: " << h_count << std::endl;
    std::cout << "txt: " << txt_count << std::endl;
    std::cout << "dir: " << dir_count << std::endl;

    return 0;
}

