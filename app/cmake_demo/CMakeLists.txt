cmake_minimum_required(VERSION 3.20.0)

project(
	Hello_Cmake
	VERSION  1.0
	DESCRIPTION "Print HELLO CMAKE"
	LANGUAGES C

)

add_library(
	my_lib
	STATIC
	src/my_lib.c
)

target_include_directories(
	my_lib
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(
	${PROJECT_NAME}
	src/main.c
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	my_lib
)
