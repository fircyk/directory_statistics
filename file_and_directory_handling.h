#ifndef FILE_AND_DIRECTORY_HANDLING_H
#define FILE_AND_DIRECTORY_HANDLING_H

#include <iostream>
#include <filesystem>
#include <string>
#include <thread>
#include <cstdlib>
#include <fstream>


namespace fs = std::filesystem;


void initialisation();
void go_through_directories();
void display_data();


#endif // FILE_AND_DIRECTORY_HANDLING_H
