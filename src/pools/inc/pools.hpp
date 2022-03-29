#pragma once

#ifndef POOLS_HEADER
#define POOLS_HEADER

#include <string>
#include <iostream>
#include <vector>

#include "../../exceptions/inc/exceptions.hpp"
#include "../../requests/inc/requests.hpp"
#include "../../utilities/inc/utilities.hpp"

using std::cout;
using std::endl;
using std::vector;
using std::string;

/**
 * @brief Implementators for Pool object
 * 
 * @author GerrFrog
 */
namespace Pools::Implementors
{
    /**
     * @brief Connector with Stratum protocol using JSON method
     * 
     * @note https://gist.github.com/beached/d2383f9b14dafcc0f585
     * @note https://stackoverflow.com/questions/66215701/boost-awaitable-write-into-a-socket-and-await-particular-response
     * @note https://github.com/bmind7/stratum_example
     * 
     * @author GerrFrog
     */
    class Stratum_Socket
    {
        private:

        protected:
            /**
             * @brief Input/Output Service
             * 
             * @author GerrFrog
             */
            net::io_service io_service;

            /**
             * @brief Working thread
             * 
             * @author GerrFrog
             */
            net::io_service::work working;

            /**
             * @brief Resolver for host
             * 
             * @author GerrFrog
             */
            tcp::resolver resolver;

            /**
             * @brief Socket object
             * 
             * @author GerrFrog
             */
            tcp::socket socket;

            /**
             * @brief Buffer for reading from server
             * 
             * @author GerrFrog
             */
            std::array<char, 65536> read_buffer;

            /**
             * @brief Subscribe message
             * 
             * @author GerrFrog
             */
            nlohmann::json subscribe_message = {
                {"jsonrpc", "2.0"}
            };

            /**
             * @brief Authorize message
             * 
             * @author GerrFrog
             */
            nlohmann::json authorize_message = {
                {"jsonrpc", "2.0"}
            };

            /**
             * @brief Server (host)
             * 
             * @author GerrFrog
             */
            string server;

            /**
             * @brief Port
             * 
             * @author GerrFrog
             */
            string port;

            /**
             * @brief Command ID number
             * 
             * @author GerrFrog
             */
            int command_id = 1;

            /**
             * @brief Prepare messagge for requesting
             * 
             * @author GerrFrog
             * 
             * @param msg JSON Message
             * @return string String prepared message
             */
            string prepare_message(nlohmann::json &msg)
            {
                std::ostringstream message;

                nlohmann::json id = {
                    {"id", this->command_id}
                };

                msg.insert(id.begin(), id.end());

                this->command_id++;

                message << msg.dump() << "\n";

                return message.str();
            }

            /**
             * @brief Convert streambuffer to string
             * 
             * @author GerrFrog
             * 
             * @param streambuf Streambuf object
             * @return string String value
             */
            string streambuf2string(net::streambuf& streambuf)
            {
                return {
                    buffers_begin(streambuf.data()), 
                    buffers_end(streambuf.data())
                };
            }

            /**
             * @brief Connect to pool
             * 
             * @author GerrFrog
             */
            void connect()
            {
                tcp::resolver::query query(
                    this->server,
                    this->port
                );

                this->resolver.async_resolve(
                    query,
                    boost::bind(
                        &Stratum_Socket::handle_resolve,
                        this,
                        net::placeholders::error,
                        net::placeholders::iterator
                    )
                );
            }

            /**
             * @brief Callback when resolve server
             * 
             * @author GerrFrog
             * 
             * @param err Error code
             * @param endpoint_iterator Endpoint iterator
             */
            void handle_resolve(
                const boost::system::error_code &err,
                tcp::resolver::iterator endpoint_iterator
            ) 
            {
                if (!err)
                {
                    this->socket.async_connect(
                        endpoint_iterator->endpoint(),
                        boost::bind(
                            &Stratum_Socket::handle_connect,
                            this,
                            net::placeholders::error
                        )
                    );
                } else {
                    // TODO: Error
                }
            }

            /**
             * @brief Callback when written to server
             * 
             * @author GerrFrog
             * 
             * @param err Error code
             */
            void handle_write_completed(
                const boost::system::error_code& err,
                std::size_t bytes_transferred
            )
            {
                if (!err && bytes_transferred != 0)
                {
                    this->socket.async_receive(
                        net::buffer(this->read_buffer),
                        boost::bind(
                            &Stratum_Socket::handle_server_msg,
                            this,
                            net::placeholders::error,
                            net::placeholders::bytes_transferred
                        )
                    );
                } else {
                    // TODO: Error
                }
            }

            /**
             * @brief Callback when read server message
             * 
             * @author GerrFrog
             * 
             * @param err Error code
             */
            virtual void handle_server_msg(
                const boost::system::error_code& err,
                std::size_t bytes_transferred
            ) = 0;

            /**
             * @brief Callback when connected to server
             * 
             * @author GerrFrog
             * 
             * @param err Error code
             */
            virtual void handle_connect(
                const boost::system::error_code& err
            ) = 0;

        public:
            /**
             * @brief Construct a new Stratum_Socket object
             * 
             * @author GerrFrog
             * 
             * @param server Server
             * @param port Port
             */
            Stratum_Socket(
                const string &server,
                const string &port
            ) : working(io_service),
                resolver(io_service),
                socket(io_service),
                server(server),
                port(port)
            { }

            /**
             * @brief Destroy the Stratum_Socket object
             * 
             * @author GerrFrog
             */
            ~Stratum_Socket()
            {
                this->io_service.stop();
            }
    };
}

/**
 * @brief Parsers for Stratum V1 messages (specific type)
 * 
 * @author GerrFrog
 */
namespace Pools::Implementors::Parsers
{
    /**
     * @brief Parser for XMR pools
     * 
     * @author GerrFrog
     */
    class Parser_V1
    {
        // TODO: Implement
        private:
            /**
             * @brief Response ID when login to pool
             * 
             * @author GerrFrog
             */
            string rpc_id;

            /**
             * @brief Login status
             * 
             * @author GerrFrog
             */
            bool status = false;

        public:
            /**
             * @brief Construct a new xmr parser object
             * 
             * @author GerrFrog
             */
            Parser_V1() = default;

            /**
             * @brief Destroy the xmr parser object
             * 
             * @author GerrFrog
             */
            ~Parser_V1() = default;

            /**
             * @brief Parse server message
             * 
             * @author GerrFrog
             * 
             * @param message Server message
             */
            Utilities::Pools::New_Job_V1 parse(string &message)
            {
                nlohmann::json json_message = nlohmann::json::parse(message);
                nlohmann::json params;

                Utilities::Pools::New_Job_V1 new_job;

                if (!json_message.contains("params"))
                {
                    if ((string)json_message["result"]["status"] == "OK")
                        this->status = true;
                    if (json_message["result"].contains("id"))
                    {
                        this->rpc_id = (string)json_message["result"]["id"];
                        params = json_message["result"]["job"];
                    } else {
                    }
                } else {
                    params = json_message["params"];
                }

                string blob = params["blob"];
                string seed_hash = params["seed_hash"];

                new_job.height = params["height"];
                new_job.blob = Utilities::HEX_String(blob);
                new_job.job_id = params["job_id"];
                new_job.target = params["target"];
                new_job.seed_hash = Utilities::HEX_String(seed_hash);

                return new_job;
            }
    };

    /**
     * @brief Parser for Stratum V2 messages (standard type)
     * 
     * @author GerrFrog
     */
    class Parser_V2
    {
        // TODO: Implement
        private:

        protected:

        public:
            /**
             * @brief Construct a new Stratum_Parser_V2 object
             * 
             * @author GerrFrog
             */
            Parser_V2() = default;

            /**
             * @brief Destroy the Stratum_Parser_V2 object
             * 
             * @author GerrFrog
             */
            ~Parser_V2() = default;

            /**
             * @brief Parse server message
             * 
             * @author GerrFrog
             * 
             * @param message Server message
             */
            Utilities::Pools::New_Job_V2 parse(string &message)
            {
                // TODO: Implement parse message
                Utilities::Pools::New_Job_V2 new_job;
                cout << message << endl;

                return new_job;
            }
    };
}

/**
 * @brief All pools
 * 
 * @author GerrFrog
 */
namespace Pools
{
    /**
     * @brief Connector to pool with specific method of Stratum protocol
     * 
     * @author GerrFrog
     */
    class Pool
        : virtual public Pools::Implementors::Stratum_Socket,
          virtual public Pools::Implementors::Parsers::Parser_V1,
          virtual public Pools::Implementors::Parsers::Parser_V2
    {
        private:
            /**
             * @brief Callback when connected to server
             * 
             * @author GerrFrog
             * 
             * @param err Error code
             */
            void handle_connect(
                const boost::system::error_code& err
            )
            {
                if (!err)
                {
                    // TODO: Send messages to authorize via this->prepare_message(this->authorize_message)
                    string message;

                    this->socket.async_send(
                        net::buffer(message),
                        boost::bind(
                            &Pool::handle_write_completed,
                            this,
                            net::placeholders::error,
                            net::placeholders::bytes_transferred
                        )
                    );
                }
            }

            /**
             * @brief Callback when read server message
             * 
             * @author GerrFrog
             * 
             * @param err Error code
             * @param bytes_transferred Raw transferred bytes
             */
            void handle_server_msg(
                const boost::system::error_code& err,
                std::size_t bytes_transferred
            )
            {
               if (!err)
                {
                    for (std::size_t start = 0, end = 0; end < bytes_transferred; end++, start = end)
                    {
                        while (end < bytes_transferred && read_buffer[ end ] != '\n')
                            end++;

                        if (end == bytes_transferred)
                        {
                            string unfinished_message(
                                std::next(std::begin(read_buffer), start),
                                std::next(std::begin(read_buffer), end + 1)
                            );
                            cout << "[ERROR] unfinished data: " << unfinished_message << std::endl;
                            break;
                        }

                        if (start == end)
                            continue;

                        string raw_message(
                            std::next(std::begin(read_buffer), start),
                            std::next(std::begin(read_buffer), end)
                        );

                        // TODO: Handle raw message
                        cout << raw_message << endl;

                        this->socket.async_receive(
                            net::buffer(this->read_buffer),
                            boost::bind(
                                &Pool::handle_server_msg,
                                this,
                                net::placeholders::error,
                                net::placeholders::bytes_transferred
                            )
                        );
                    }
                } else {
                    // TODO: Error
                }
            }

        public:
            /**
             * @brief Construct a new xmr pool object
             * 
             * @author GerrFrog
             * 
             * @param config Pool configuration
             */
            Pool(
                nlohmann::json &config
            ) : Stratum_Socket(
                    (string)config["host"],
                    (string)config["port"]
                ),
                Parser_V1(),
                Parser_V2()
            {
                // TODO: Create authorize messages

                auto _ = std::async(
                    boost::bind(
                        &net::io_service::run,
                        &this->io_service
                    )
                );
                this->io_service.post(
                    boost::bind(
                        &Pool::connect,
                        this
                    )
                );
            }
    
            /**
             * @brief Destroy the xmr pool object
             * 
             * @author GerrFrog
             */
            virtual ~Pool() = default;
    };
}























#endif 