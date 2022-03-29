#include "main.hpp"

/**
 * @brief Main function
 * 
 * @author GerrFrog
 * 
 * @param argc Argument counter 
 * @param argv Argument char pointer
 * @return int Exit status
 */
int main(int argc, char *argv[])
{
    try {
        dotenv::load("../.env");

        string http_proxy = getenv("HTTP_PROXY");
        string https_proxy = getenv("HTTPS_PROXY");

        nlohmann::json configuration = nlohmann::json::parse(
            std::ifstream("../config.json")
        );

        cxxopts::Options options(
            "./CPUMinerRandomX",
            "Miner for CPU with RandomX algorithm"
        );

        options.add_options()
            ("s,server", "Server configuration")
            ("p,proxy", "Proxy configuration")
            ("h,help", "Help for arguments list")
        ;

        auto result = options.parse(argc, argv);
        
        if (result.count("help"))
        {
            cout << options.help() << endl;
            exit(0);
        }
        if (result.count("proxy"))
            cout 
                << "HTTP_PROXY: " << http_proxy << endl
                << "HTTPS_PROXY: " << https_proxy << endl
            << endl;
        if (result.count("server"))
            cout
                << "SERVER HOST: " << (string)configuration["server"]["host"] << endl
                << "SERVER PORT: " << (int)configuration["server"]["port"] << endl
            << endl;

        Pools::Pool_V1 pool(configuration["pool"]);
        std::cin.ignore();

    } catch (std::logic_error& exp) {
        cout 
            << exp.what() << endl
            << "Probably cannot find .env file" << endl
        << endl;

        return EXIT_FAILURE;
    } catch (nlohmann::detail::parse_error& exp) {
        cout 
            << exp.what() << endl
            << "Probably cannot find config.json file" << endl
        << endl;

        return EXIT_FAILURE;
    } catch (std::exception &exp) {
        cout 
            << exp.what() << endl
            << "Unhandled exception!" << endl
        << endl;

        return EXIT_FAILURE;
    } catch (...) {
        cout << "Something horrable happened!" << endl;
        
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}









