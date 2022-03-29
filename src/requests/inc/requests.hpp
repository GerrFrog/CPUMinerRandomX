#pragma once

#ifndef REQUEST_HEADER
#define REQUEST_HEADER

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <istream>
#include <ostream>
#include <memory>
#include <string>
#include <thread>
#include <future>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "../../exceptions/inc/exceptions.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;

using tcp = net::ip::tcp;

using std::cout;
using std::endl;
using std::string;
using std::unordered_map;

/**
 * @brief Abstracts classes for Request objects
 *
 * @author GerrFrog
 */
namespace Requests::Abstracts
{
    /**
     * @brief Abstract class for all Request objects
     *
     * @note https://www.boost.org/doc/libs/1_71_0/libs/beast/doc/html/beast/quick_start/http_client.html
     * @note https://stackoverflow.com/questions/33877032/boostasio-usage-in-self-contained-class
     *
     * @author GerrFrog
     */
    class Request
    {
        private:

        protected:
            /**
             * @brief Base URL to request
             *
             * @author GerrFrog
             */
            string host;

            /**
             * @brief Port number
             *
             * @author GerrFrog
             */
            string port = "80";

            /**
             * @brief Input/Output context
             *
             * @author GerrFrog
             */
            net::io_context ioc;

            /**
             * @brief Resolver for TCP connection
             *
             * @author GerrFrog
             */
            tcp::resolver resolver;

            /**
             * @brief TCP Stream
             *
             * @author GerrFrog
             */
            beast::tcp_stream stream;

            /**
             * @brief Buffer for reading. Contains server response
             *
             * @author GerrFrog
             */
            beast::flat_buffer buffer;

            /**
             * @brief Container to hold the response from server
             *
             * @author GerrFrog
             */
            http::response<http::dynamic_body> response;

            /**
             * @brief Setup Parameters for request
             *
             * @author GerrFrog
             *
             * @param parameters Parameters for request
             * @return string URL string with setup parameters
             */
            string joinQueryParameters(const std::unordered_map<string, string> &parameters)
            {
                string queryString = "";
                for (auto it = parameters.cbegin(); it != parameters.cend(); ++it)
                {
                    if (it == parameters.cbegin())
                    {
                        queryString += it->first + '=' + it->second;
                    }

                    else
                    {
                        queryString += '&' + it->first + '=' + it->second;
                    }
                }

                return queryString;
            }

            /**
             * @brief Set the up HTTP request body
             *
             * @author GerrFrog
             *
             * @param target URL to send request
             * @return http::request<http::string_body>
             */
            virtual http::request<http::string_body> setup_http(string &target) = 0;

        public:
            /**
             * @brief Construct a new Request object
             *
             * @author GerrFrog
             *
             * @param host Base URL to request
             * @param port Port
             */
            Request(
                const string &host,
                const string &port) : host(host), port(port),
                                    resolver(ioc),
                                    stream(ioc)
            {
                this->stream.connect(
                    this->resolver.resolve(this->host, this->port));
            }

            /**
             * @brief Destroy the Request object
             *
             * @author GerrFrog
             */
            ~Request() = default;

            /**
             * @brief Close connection with server
             *
             * @author GerrFrog
             *
             * @return beast::error_code
             */
            beast::error_code close_connection()
            {
                beast::error_code ec;

                this->stream.socket().shutdown(tcp::socket::shutdown_both, ec);

                return ec;
            }

            /**
             * @brief Virtual method for sending request
             *
             * @author GerrFrog
             *
             * @param urlPath URL Path
             * @param parameters Parameters for Request
             * @return string Content of Response
             */
            virtual string send_request_string(
                const string &urlPath,
                unordered_map<string, string> &parameters) = 0;

            /**
             * @brief Virtual method for sending request
             *
             * @author GerrFrog
             *
             * @param urlPath URL Path
             * @param parameters Parameters for Request
             * @return nlohmann::json JSON Content of Response
             */
            virtual nlohmann::json send_request_json(
                const string &urlPath,
                unordered_map<string, string> &parameters) = 0;
    };
}

/**
 * @brief Contains all Requests objects to remote host
 *
 * @author GerrFrog
 */
namespace Requests
{
    /**
     * @brief Send GET Request
     *
     * @author GerrFrog
     */
    class GET_Request : virtual public Requests::Abstracts::Request
    {
    private:
        /**
         * @brief Set the up HTTP request body
         *
         * @author GerrFrog
         *
         * @param target URL to send request
         * @return http::request
         */
        http::request<http::string_body> setup_http(string &target)
        {
            http::request<http::string_body> req{http::verb::get, target, 11};

            req.set(http::field::host, this->host);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            return req;
        }

    public:
        /**
         * @brief Construct a new get request object
         *
         * @author GerrFrog
         *
         * @param host Host to request (e.g http://google.com)
         * @param port Port (default 8080)
         */
        GET_Request(
            const string &host,
            const string &port) : Request(host, port)
        {
        }

        /**
         * @brief Destroy the get request object
         *
         * @author GerrFrog
         */
        virtual ~GET_Request() = default;

        /**
         * @brief Virtual method for sending request
         *
         * @author GerrFrog
         *
         * @param urlPath URL Path
         * @param parameters Parameters for Request
         * @return string Content of Response
         */
        string send_request_string(
            const string &urlPath,
            unordered_map<string, string> &parameters)
        {
            string url = urlPath + '?';

            if (!parameters.empty())
                url += this->joinQueryParameters(parameters);

            http::request<http::string_body> request_message =
                this->setup_http(url);

            http::write(this->stream, request_message);
            http::read(this->stream, this->buffer, this->response);

            return beast::buffers_to_string(this->response.body().data());
        }

        /**
         * @brief Virtual method for sending request
         *
         * @author GerrFrog
         *
         * @param urlPath URL Path
         * @param parameters Parameters for Request
         * @return nlohmann::json JSON Content of Response
         */
        nlohmann::json send_request_json(
            const string &urlPath,
            unordered_map<string, string> &parameters)
        {
            string url = urlPath + '?';

            if (!parameters.empty())
                url += this->joinQueryParameters(parameters);

            http::request<http::string_body> request_message =
                this->setup_http(url);

            http::write(this->stream, request_message);
            http::read(this->stream, this->buffer, this->response);

            return nlohmann::json::parse(beast::buffers_to_string(this->response.body().data()));
        }
    };
}

#endif