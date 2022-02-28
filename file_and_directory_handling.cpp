#include "file_and_directory_handling.h"



int dir_count;
int file_count;
int txt_count;
int c_and_cpp_count;
int h_count;

int empty_lines;
int cpp_lines;
int h_lines;
int txt_lines;

fs::path work_path;


void initialisation(){
    dir_count = 0;
    file_count = 0;
    txt_count = 0;
    c_and_cpp_count = 0;
    h_count = 0;

    cpp_lines = 0;
    h_lines = 0;
    txt_lines = 0;
    empty_lines = 0;

    work_path = fs::current_path().parent_path();
    work_path = work_path / "directory_statistics";

}

void go_through_directories(){
    for(const fs::directory_entry &path : fs::recursive_directory_iterator(work_path)){
       // std::cout << path << std::endl;
        std::string string_path = path.path().u8string();
        std::cout << path.path().u8string() << std::endl;

        if((string_path.find(".cpp") != std::string::npos) || (string_path.find(".c") != std::string::npos)){
            c_and_cpp_count++;
            std::ifstream file_cpp;

            file_cpp.open(string_path, std::fstream::in);
            std::string read;
            while(getline(file_cpp, read)){
                cpp_lines++;
                if(read.empty()){
                    empty_lines++;
                }
            }
            read.clear();



        }
        if((string_path.find(".hpp") != std::string::npos) || (string_path.find(".h") != std::string::npos)){
            h_count++;


            std::ifstream file_h;

            file_h.open(string_path, std::fstream::in);
            std::string read;
            while(std::getline(file_h, read)){
                h_lines++;
                if(read.empty()){
                    empty_lines++;
                }
            }



        }
        if(string_path.find(".txt") != std::string::npos){
            txt_count++;


            std::ifstream file_txt;

            file_txt.open(string_path, std::fstream::in);
            std::string read;
            while(std::getline(file_txt, read)){
                txt_lines++;
                if(read.empty()){
                    empty_lines++;
                }
            }


        }
        if(path.is_directory()){
            dir_count++;
        }
        if(path.is_character_file()){
            file_count++;
        }





    }

}

void display_data(){

    std::cout << work_path << std::endl;
    std::cout << "c and cpp files: " << c_and_cpp_count << std::endl;
    std::cout << "h files: " << h_count << std::endl;
    std::cout << "txt files: " << txt_count << std::endl;
    std::cout << "directories: " << dir_count << std::endl;

    std::cout << "empty lines: " << empty_lines << std::endl;
    std::cout << "total lines: " << txt_lines+cpp_lines+h_lines << std::endl;
    std::cout << "h lines: " << h_lines << std::endl;
    std::cout << "c and cpp lines: " << cpp_lines << std::endl;
    std::cout << "txt lines: " << txt_lines << std::endl;

}
