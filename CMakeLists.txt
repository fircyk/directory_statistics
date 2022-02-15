cmake_minimum_required(VERSION 3.5)


#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)


#add_executable(directory_statistics main.cpp)


#cmake_minimum_required(VERSION 3.16)

project(directory_statistics LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)



add_executable(directory_statistics main.cpp)

#target_link_libraries(helloworld PRIVATE Qt6::Widgets)
