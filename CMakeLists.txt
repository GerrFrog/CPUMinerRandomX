cmake_minimum_required( VERSION 3.1.0 )

project ( CPUMinerBase VERSION 1.0.0 LANGUAGES CXX )

# Prepare CMake
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fsanitize=address" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu++0x -D__STDC_INT64__" )
set( Boost_USE_MULTITHREADED ON )
set( CMAKE_THREAD_PREFER_PTHREAD TRUE )
set( THREADS_PREFER_PTHREAD_FLAG TRUE )

################ VARIABLES ###########################
set( LIBS_FILES src/libs )
set( EXCEPTIONS_FILES src/exceptions )
set( REQUEST_FILES src/requests )
set( POOLS_FILES src/pools )
set( UTILITIES_FILES src/utilities )
set( SOLVERS_FILES src/solvers )
set( HASHES_FILES src/hashes )
############# END VARIABLES ############################

############### SOURCE FILES ##############################
set(
    HEADER_FILES 
    src/main.hpp
    ${EXCEPTIONS_FILES}/inc/exceptions.hpp
    ${REQUEST_FILES}/inc/requests.hpp
    ${POOLS_FILES}/inc/pools.hpp
    ${POOLS_FILES}/inc/test.hpp
    ${UTILITIES_FILES}/inc/utilities.hpp
    ${SOLVERS_FILES}/inc/solvers.hpp
    ${HASHES_FILES}/inc/hashes.hpp
)
set(
    IMPLEMENTED_FILES
    src/main.cpp
    ${EXCEPTIONS_FILES}/src/exceptions.cpp
    ${REQUEST_FILES}/src/requests.cpp
    ${POOLS_FILES}/src/pools.cpp
    ${POOLS_FILES}/src/test.cpp
    ${UTILITIES_FILES}/src/utilities.cpp
    ${SOLVERS_FILES}/src/solvers.cpp
    ${HASHES_FILES}/src/hashes.cpp
)

set(
    LIBS_HEADER_FILES
    ${LIBS_FILES}/dotenv/include/dotenv.hpp
    ${LIBS_FILES}/csv/csv.hpp
)

set(
    LIBS_IMPLEMENTED_FILES
    ${LIBS_FILES}/dotenv/src/dotenv.cpp
)
############## END SOURCE FILES ###########################

##################### PACKAGES#######################
find_package( Threads REQUIRED )
find_package( Boost COMPONENTS system REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( nlohmann_json REQUIRED )
####################### END PACKAGES#######################

#################### LINKING ##########################
include_directories(
    /usr/local/include
    ${MYSQL_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)

add_definitions(
    -UNDEBUG
    -DDTF_HEADER_ONLY
    -D__FLATJSON__CHILDS_TYPE=std::uint32_t
    -D__FLATJSON__VLEN_TYPE=std::uint32_t
)

add_executable(
    ${PROJECT_NAME}
    ${HEADER_FILES}
    ${IMPLEMENTED_FILES}
    ${LIBS_HEADER_FILES}
    ${LIBS_IMPLEMENTED_FILES}
)

target_link_libraries(
    ${PROJECT_NAME}
    z
    crypto
    ssl
    pthread
    mysqlclient
    Threads::Threads
    OpenSSL::SSL
    nlohmann_json::nlohmann_json
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
    -L${BOOST_LIB_DIR}
)
#################### END LINKING ####################################










