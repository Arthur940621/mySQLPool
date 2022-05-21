cmake_minimum_required(VERSION 3.0)

project(mySQLPoolTest)

include_directories(
    ./MySQLPool/include
    ./Json/include
    /usr/local/lib
    /usr/include/mysql
)

aux_source_directory(./MySQLPool SRC_LIST1)
aux_source_directory(./Json SRC_LIST2)
aux_source_directory(. SRC_MAIN)


add_compile_options(-Wall -pthread -lmysqlclient -std=c++17)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(mySQLPoolTest ${SRC_MAIN} ${SRC_LIST1} ${SRC_LIST2})
target_link_libraries(mySQLPoolTest libmysqlclient.so)
target_link_libraries (mySQLPoolTest pthread) 