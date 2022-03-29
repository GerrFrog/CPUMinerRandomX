#pragma once

#ifndef TEST_HEADER
#define TEST_HEADER

#include <string>
#include <iostream>

#include "../../exceptions/inc/exceptions.hpp"
#include "../../requests/inc/requests.hpp"

using std::cout;
using std::endl;
using std::string;

/**
 * @brief All abstract classes for pools objects
 * 
 * @author GerrFrog
 */
namespace Pools::Test::Abstract
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
            /**
             * @brief Host
             * 
             * @author GerrFrog
             */
            string host;

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
             * @brief Input/Output service
             * 
             * @author GerrFrog
             */
            net::io_service io_service;

            /**
             * @brief Socket object
             * 
             * @author GerrFrog
             */
            tcp::socket socket;

            /**
             * @brief Read buffer
             * 
             * @author GerrFrog
             */
            std::array<char, 65536> read_buffer;

            /**
             * @brief Write buffer
             * 
             * @author GerrFrog
             */
            std::array<char, 65536> write_buffer;

        public:
            /**
             * @brief Construct a new Socket object
             * 
             * @author GerrFrog
             * 
             * @param host Host
             * @param port Port
             */
            Stratum_Socket(
                const string& host,
                const string& port
            ) : host(host),
                port(port),
                socket(io_service)
            {
                tcp::resolver resolver(this->io_service);
                tcp::resolver::query query(host, port);
                tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

                net::connect(this->socket, endpoint_iterator);
            }

            /**
             * @brief Destroy the Socket object
             * 
             * @author GerrFrog
             */
            ~Stratum_Socket()
            {
                this->io_service.stop();
            }

            /**
             * @brief Read from socket
             * 
             * @author GerrFrog
             * 
             * @return nlohmann::json JSON Response
             */
            nlohmann::json read()
            {
                std::size_t bytes_received = this->socket.receive(
                    net::buffer(this->read_buffer)
                );
                string response;

                for (std::size_t start = 0, end = 0; end < bytes_received; end++, start = end)
                {
                    while (end < bytes_received && read_buffer[ end ] != '\n')
                        end++;

                    if (end == bytes_received) {
                        string unfinished_message(
                            std::next(std::begin(read_buffer), start),
                            std::next(std::begin(read_buffer), end + 1)
                        );
                        break;
                    }

                    if (start == end)
                        continue;

                    string raw_message(
                        std::next(std::begin(read_buffer), start),
                        std::next(std::begin(read_buffer), end)
                    );
                    response = raw_message;
                }

                return nlohmann::json::parse(response);
            }

            /**
             * @brief Write to socket
             * 
             * @author GerrFrog
             * 
             * @param msg Message
             * @return std::size_t Number of written bytes
             */
            std::size_t write(nlohmann::json &msg)
            {
                std::ostringstream message;

                nlohmann::json id = {
                    {"id", this->command_id}};

                msg.insert(id.begin(), id.end());

                message << msg.dump() << "\n";

                this->command_id++;

                std::size_t bytes_transferred = this->socket.send(
                    net::buffer(message.str()));

                return bytes_transferred;
            }
    };

    /**
     * @brief Abstract class for pool object
     * 
     * @author GerrFrog
     */
    class Pool_Abstract
    {
        private:

        protected:
            /**
             * @brief Message to subscribe to pool
             * 
             * @author GerrFrog
             */
            nlohmann::json subscribe_message = {
                {"jsonrpc", "2.0"}
            };

            /**
             * @brief Message to authorize to pool
             * 
             * @author GerrFrog
             */
            nlohmann::json authorize_message = {
                {"jsonrpc", "2.0"}
            };

            /**
             * @brief Stratum socket object
             * 
             * @author GerrFrog
             */
            Pools::Test::Abstract::Stratum_Socket stratum_socket;

        public:
            /**
             * @brief Construct a new Pool_Abstract object
             * 
             * @author GerrFrog
             * 
             * @param configuration Configuration for pool
             */
            Pool_Abstract(
                nlohmann::json &configuration
            ) : stratum_socket(
                    (string)configuration["host"],
                    (string)configuration["port"]
                )
            { }

            /**
             * @brief Destroy the Pool_Abstract object
             * 
             * @author GerrFrog
             */
            ~Pool_Abstract() = default;

            /**
             * @brief Write to Pool
             * 
             * @author GerrFrog
             * 
             * @param message Message
             * @return std::size_t 
             */
            std::size_t write(nlohmann::json &message)
            {
                return this->stratum_socket.write(message);
            }

            /**
             * @brief Read from pool
             * 
             * @author GerrFrog
             * 
             * @return nlohmann::json 
             */
            nlohmann::json read()
            {
                return this->stratum_socket.read();
            }

            /**
             * @brief Connect to pool
             * 
             * @author GerrFrog
             */
            virtual void connect() = 0;

    };
}

/**
 * @brief All pool classes with Stratum V1 protocol
 * 
 * @author GerrFrog
 */
namespace Pools::Test::Stratum_V1
{
    /**
     * @brief Connector to XMR pool with specific method of Stratum protocol
     * (e.g. {"method": "login"})
     * 
     * @note https://github.com/jtgrassie/monero-pool/blob/master/sss.md
     * 
     * @author GerrFrog
     */
    class XMR_Pool : virtual public Pools::Test::Abstract::Pool_Abstract
    {
        private:

        public:
            /**
             * @brief Construct a new xmr pool object
             * 
             * @author GerrFrog
             * 
             * @param pool_config Configuration for pool
             */
            XMR_Pool(
                nlohmann::json &pool_config
            ) : Pool_Abstract(pool_config)
            {
                nlohmann::json mining_authorize({
                    {"method", "login"},
                    {"params", {
                        {"login", (string)pool_config["login"]},
                        {"pass", (string)pool_config["password"]},
                        {"agent", "user-agent/0.1"},
                        {"mode", "self-select"}
                    }}
                });

                this->authorize_message.insert(
                    mining_authorize.begin(),
                    mining_authorize.end()
                );
            }

            /**
             * @brief Destroy the xmr pool object
             * 
             * @author GerrFrog
             */
            virtual ~XMR_Pool() = default;

            /**
             * @brief Connect to pool
             * 
             * @author GerrFrog
             */
            void connect()
            {
                this->stratum_socket.write(this->authorize_message);
            }
    };

    /**
     * @brief Connector to ETH pool with specific method of Stratum protocol
     * (e.g. {"method": "eth_submitLogin"})
     * 
     * @note https://github.com/sammy007/open-ethereum-pool/blob/master/docs/STRATUM.md
     * 
     * @author GerrFrog
     */
    class ETH_Pool : virtual public Pools::Test::Abstract::Pool_Abstract
    {
        private:
            /**
             * @brief Message to get work from pool
             * 
             * @author GerrFrog
             */
            nlohmann::json get_work_message = {
                {"jsonrpc", "2.0"},
                {"method", "eth_getWork"}
            };

            /**
             * @brief Message to submit work for pool
             * 
             * @author GerrFrog
             */
            nlohmann::json submit_work_message = {
                {"jsonrpc", "2.0"},
                {"method", "eth_submitWork"}
            };

        public:
            /**
             * @brief Construct a new eth pool object
             * 
             * @author GerrFrog
             * 
             * @param pool_config Configuration for pool
             */
            ETH_Pool(
                nlohmann::json &pool_config
            ) : Pool_Abstract(pool_config)
            {
                nlohmann::json mining_authorize({
                    {"method", "eth_submitLogin"},
                    {"params", {
                        (string)pool_config["login"],
                        (string)pool_config["password"],
                    }}
                });

                this->authorize_message.insert(
                    mining_authorize.begin(),
                    mining_authorize.end()
                );
            }

            /**
             * @brief Destroy the eth pool object
             * 
             * @author GerrFrog
             */
            virtual ~ETH_Pool() = default;

            /**
             * @brief Connect to pool
             * 
             * @author GerrFrog
             */
            void connect()
            {
                this->stratum_socket.write(this->authorize_message);
            }
    };
}

/**
 * @brief All pool classes with Stratum V2 protocol
 * 
 * @author GerrFrog
 */
namespace Pools::Test::Stratum_V2
{
    /**
     * @brief Connector to pool with standard methods of Stratum protocol.
     * (e.g. {"method": "mining.subscribe"})
     * 
     * @author GerrFrog
     */
    class Pool : virtual public Pools::Test::Abstract::Pool_Abstract
    {
        private:

        public:
            /**
             * @brief Construct a new Pool object
             * 
             * @author GerrFrog
             * 
             * @param pool_config Configuration for pool
             */
            Pool(
                nlohmann::json &pool_config
            ) : Pool_Abstract(pool_config)
            {
                nlohmann::json mining_subscribe({
                    {"method", "mining.subscribe"},
                    {"params", {
                        "CPUMinerRandomX",
                        nullptr,
                        (string)pool_config["host"],
                        (string)pool_config["port"],
                    }}
                });
                nlohmann::json mining_authorize({
                    {"method", "mining.authorize"},
                    {"params", {
                        (string)pool_config["login"],
                        (string)pool_config["password"],
                    }}
                });

                this->subscribe_message.insert(
                    mining_subscribe.begin(),
                    mining_subscribe.end()
                );
                this->authorize_message.insert(
                    mining_authorize.begin(),
                    mining_authorize.end()
                );
            }

            /**
             * @brief Destroy the Pool object
             * 
             * @author GerrFrog
             */
            virtual ~Pool() = default;

            /**
             * @brief Connect to pool
             * 
             * @author GerrFrog
             */
            void connect()
            {
                this->stratum_socket.write(this->subscribe_message);
                this->stratum_socket.write(this->authorize_message);
            }
    };
}







#endif