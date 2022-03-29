# Content table
- [Content table](#content-table)
- [Request namespace](#request-namespace)
  - [Requests::Abstracts](#requestsabstracts)
  - [Requests](#requests)
- [Examples](#examples)
# Request namespace
This header file and implementation file contains all classes to execute any type of request to any services via HTTP, HTTPS, JSON and Strarum protocols.

## Requests::Abstracts
This namespace contains all abstract classes for Request objects using "Boost::ASIO" library. These Requests objects based on boost::asio::io_context object.
- [x] **Requests::Abstracts::Request** - Basic class for any HTTP Request Method: GET, POST, DELETE, PATCH, PUT.
- [x] **Requests::Abstract::Socket** - Basic class for any socket connectors

## Requests
This namespace contains all Request objects using "ASIO" library based on boost::asio::io_context object.
- [x] **Requests::GET_Request** - Request object with GET method
- [ ] **Requests::POST_Request** - Request object with POST method
- [ ] **Requests::JSON_Request** - Request object with JSON format
- [x] **Requests::Stratum_Socket** - Request object with Stratum protocol using JSON format



# Examples
Connect to https://jsonplaceholder.typicode.com using GET_Request connector
```C
std::unordered_map<string,string> parameters;

Requests::GET_Request get_request_context("jsonplaceholder.typicode.com", "80");

string string_response = get_request_context.send_request_string("/posts", parameters);
nlohmann::json json_respone = get_request_context.send_request_json("/posts", parameters);

get_request_context.close_connection();
```

Connect to aionpool using Stratum_Socket connector
```C
string host = "cluster.aionpool.tech";
string port = "2222";
string login = "0xa0f41e6a0e098b324977e7334f91e69055ab6ca963ae8dcfa0bda08006518432.testworker";
string password = "x";

std::size_t written_bytes;

nlohmann::json response;

nlohmann::json subscribe_message = {
    {"id", 1},
    {"method", "mining.subscribe"},
    {"params", {
        "CPUMinerRandomX",
        nullptr,
        (string)aionpool_configuration["host"],
        (string)aionpool_configuration["port"],                        
    }}
};

nlohmann::json authorize_message = {
    {"id", 2},
    {"method", "mining.authorize"},
    {"params", {
        (string)aionpool_configuration["login"],
        (string)aionpool_configuration["password"],
    }}
};

Requests::Stratum_Socket stratum_socket(
    host,
    port,
    login,
    password
);

written_bytes = stratum_socket.write(subscribe_message);
written_bytes = stratum_socket.write(authorize_message);

response = stratum_socket.read();
```







