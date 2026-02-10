cmake_minimum_required(VERSION 3.10.0)
project(Game VERSION 0.1.0 LANGUAGES C CXX)

#include all inc/* directories
file(GLOB V_GLOB LIST_DIRECTORIES true "inc/*")
foreach(item ${V_GLOB})
	if(IS_DIRECTORY ${item})
		add_subdirectory(${item})
	endif()
endforeach()

add_executable(Game src/main.cpp)

target_link_libraries(Game glfw UnBBoolean GLAD Linmath)