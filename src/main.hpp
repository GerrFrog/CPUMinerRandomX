#pragma once

#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <cxxopts.hpp>
#include <limits>

#include "requests/inc/requests.hpp"
#include "pools/inc/pools.hpp"
#include "pools/inc/test.hpp"
#include "libs/csv/csv.hpp"
#include "libs/dotenv/include/dotenv.hpp"

using std::cout;
using std::endl; 
using std::string;

int main(int argc, char *argv[]);

#endif