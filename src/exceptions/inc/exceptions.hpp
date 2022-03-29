#pragma once

#ifndef EXCEPTIONS_HEADER
#define EXCEPTIONS_HEADER

#include <stdexcept>
#include <exception>
#include <typeinfo>
#include <string>

using std::string;

/**
 * @brief Request to URL Exceptions
 * 
 * @author 
 */
namespace Exceptions::Requests
{
    /**
     * @brief HTTP Request to URL Exception
     * 
     * @author GerrFrog
     */
    class HTTP_Request_Error : virtual public std::exception
    {
        protected:
            /**
             * @brief Error number
             * 
             * @author GerrFrog
             */
            int error_number;

            /**
             * @brief Error offset
             * 
             * @author GerrFrog
             */
            int error_offset;

            /**
             * @brief Error message
             * 
             * @author GerrFrog
             */
            string error_message;

        public:
            /**
             * @brief Construct a new http request error object
             * 
             * @author GerrFrog
             * 
             * @param msg Error Message
             * @param err_num Error number
             * @param err_off Error offset
             */
            explicit HTTP_Request_Error(
                const string& msg,
                int err_num,
                int err_off
            ) : error_number(err_num),
                error_offset(err_off),
                error_message(msg)
            { }

            /**
             * @brief Destroy the http request error object
             * 
             * @author GerrFrog
             */
            virtual ~HTTP_Request_Error() throw()
            { }

            /**
             * @brief What method of exceptions
             * 
             * @author GerrFrog
             * 
             * @return const char* 
             */
            virtual const char* what() const throw () { return error_message.c_str(); }
            
            /**
             * @brief Get the Error Number 
             * 
             * @author GerrFrog
             * 
             * @return int Error number
             */
            virtual int getErrorNumber() const throw() { return error_number; }
            
            /**
             * @brief Get the Error Offset 
             * 
             * @author GerrFrog
             * 
             * @return int Error ofset
             */
            virtual int getErrorOffset() const throw() { return error_offset; }
    };

    /**
     * @brief HTTP Response to URL Exception
     * 
     * @author GerrFrog
     */
    class HTTP_Response_Error : virtual public std::exception
    {
            /**
             * @brief Error number
             * 
             * @author GerrFrog
             */
            int error_number;

            /**
             * @brief Error offset
             * 
             * @author GerrFrog
             */
            int error_offset;

            /**
             * @brief Error message
             * 
             * @author GerrFrog
             */
            string error_message;

        public:
            /**
             * @brief Construct a new http response error object
             * 
             * @author GerrFrog
             * 
             * @param msg Error Message
             * @param err_num Error number
             * @param err_off Error offset
             */
            explicit HTTP_Response_Error(
                const string& msg,
                int err_num,
                int err_off
            ) : error_number(err_num),
                error_offset(err_off),
                error_message(msg)
            { }

            /**
             * @brief Destroy the http response error object
             * 
             * @author GerrFrog
             */
            virtual ~HTTP_Response_Error() throw()
            { }

            /**
             * @brief What method of exceptions
             * 
             * @author GerrFrog
             * 
             * @return const char* 
             */
            virtual const char* what() const throw () { return error_message.c_str(); }
            
            /**
             * @brief Get the Error Number 
             * 
             * @author GerrFrog
             * 
             * @return int Error number
             */
            virtual int getErrorNumber() const throw() { return error_number; }
            
            /**
             * @brief Get the Error Offset 
             * 
             * @author GerrFrog
             * 
             * @return int Error ofset
             */
            virtual int getErrorOffset() const throw() { return error_offset; }
    };
}


























#endif 