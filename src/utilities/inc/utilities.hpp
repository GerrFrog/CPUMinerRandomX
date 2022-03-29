#pragma once

#ifndef UTILITIES_HEADER
#define UTILITIES_HEADER

#include <string>
#include <iostream>
#include <vector>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdint.h>
#include <algorithm>
#include <iomanip>
#include <cstdint>
#include <cstring>

#include "../../exceptions/inc/exceptions.hpp"

using std::cout;
using std::endl;
using std::vector;
using std::string;

using binary = vector<unsigned char>;

/**
 * @brief All utilities
 * 
 * @author GerrFrog
 */
namespace Utilities
{
    /**
     * @brief HEX String object
     * 
     * @author GerrFrog
     */
    class HEX_String
    {
        private:
            /**
             * @brief Decoded string to binary
             * 
             * @author GerrFrog
             */
            binary decoded;

            /**
             * @brief Encoded string 
             * 
             * @author GerrFrog
             */
            string encoded;

        public:
            /**
             * @brief Construct a new hex string object
             * 
             * @author GerrFrog
             */
            HEX_String() = default;

            /**
             * @brief Construct a new hex string object
             * 
             * @author GerrFrog
             * 
             * @param hex_encoded HEX String 
             */
            HEX_String(const string& hex_encoded) :
                encoded(hex_encoded)
            {
                // if ((hex_encoded.length() % 2) != 0)
                // 	throw runtime_error("String has not valid length ...");

                size_t cnt = hex_encoded.length() / 2;

                for (size_t i = 0; cnt > i; ++i)
                {
                    unsigned char s = strtol(hex_encoded.substr(i * 2, 2).c_str(), nullptr, 16);
                    decoded.push_back(s);
                }
            }

            /**
             * @brief Construct a new hex string object
             * 
             * @author GerrFrog
             * 
             * @param bin_data Decode binary vector
             */
            HEX_String(const binary& bin_data) :
                decoded(bin_data)
            {
                std::stringstream ss;
                for (auto b : decoded)
                    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(b);
                encoded = ss.str();
            }

            /**
             * @brief Construct a new hex string object
             * 
             * @author GerrFrog
             * 
             * @param num_data Decoded binary vector
             */
            HEX_String(uint32_t num_data) :
                decoded(((unsigned char*)&num_data), 
                ((unsigned char*)&num_data) + sizeof(uint32_t))
            {
                std::stringstream ss;
                for (auto b : decoded)
                    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(b);
                encoded = ss.str();
            }

            /**
             * @brief Destroy the hex string object
             * 
             * @author GerrFrog
             */
            ~HEX_String() = default;

            /**
             * @brief Operator overlay for string
             * 
             * @author GerrFrog
             * 
             * @return string && 
             */
            operator string && ()
            {
                return std::move(encoded);
            }

            /**
             * @brief Operator overlay for binary type
             * 
             * @author GerrFrog
             * 
             * @return binary && 
             */
            operator binary && ()
            {
                return std::move(decoded);
            }

            /**
             * @brief Operator overlay for uint32_t type
             * 
             * @author GerrFrog
             * 
             * @return uint32_t 
             */
            operator uint32_t()
            {
                uint32_t res = 0;
                // if (decoded.size() > sizeof(uint32_t))
                //     throw "Failed convert hex string to uint32_t";
                std::memcpy(&res, &decoded[0], decoded.size());
                return res;
            }

            /**
             * @brief Get the decoded
             * 
             * @author GerrFrog
             * 
             * @return binary 
             */
            binary get_decoded() { return this->decoded; }

            /**
             * @brief Get the encoded object
             * 
             * @author GerrFrog
             * 
             * @return string 
             */
            string get_encoded() { return this->encoded; }
    };
}

/**
 * @brief All utilities for pool objects
 * 
 * @author GerrFrog
 */
namespace Utilities::Pools
{
    /**
     * @brief New job message from pool using Stratum V1
     * 
     * @author GerrFrog
     */
    struct New_Job_V1
    {
        // TODO: Implement
    };

    /**
     * @brief New job message from pool using Stratum V2
     * 
     * @author GerrFrog
     */
    struct New_Job_V2
    {
        // TODO: Implement
    };
}









#endif