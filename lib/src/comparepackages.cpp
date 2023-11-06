#include "../include/comparepackages.hpp"

namespace http = boost::beast::http;

boost::json::value getPackages(const std::string& branch, const std::string& arch) {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::beast::tcp_stream stream(io_context);

    const std::string host = "rdb.altlinux.org";
    const std::string target = "/api/export/branch_binary_packages/" + branch + "?arch=" + arch;

    boost::asio::ip::tcp::resolver::results_type results = resolver.resolve(host, "http");

    boost::asio::ip::tcp::endpoint endpoint = *results;

    stream.connect(endpoint);

    http::request<http::string_body> request{http::verb::get, target, 11};
    request.set(http::field::host, host);

    http::write(stream, request);
    std::cout << "request " << request << " sent" << std::endl;
    http::response<http::string_body> response;
    boost::beast::flat_buffer buffer;

    http::read(stream, buffer, response);
    std::cout << "response " << response << " received" << std::endl;
    std::cout << "parse begins" << std::endl;
    boost::json::value result = boost::json::parse(response.body());
    return result;
}

boost::json::object comparePackages(const boost::json::value& branch1, const boost::json::value& branch2) {
    std::set<boost::json::string> branch1packages;
    std::set<boost::json::string> branch2packages;

    for (const boost::json::value& package : branch1.at("packages").as_array()) {
        branch1packages.insert(package.at("name").as_string());
    }
    for (const boost::json::value& package : branch2.at("packages").as_array()) {
        branch2packages.insert(package.at("name").as_string());
    }

    boost::json::array onlyBranch1packages;
    boost::json::array onlyBranch2packages;
    boost::json::array differentVersionPackages;

    for (const boost::json::value& package : branch1.at("packages").as_array()) {
        const boost::json::string name = package.at("name").as_string();
        const boost::json::string version = package.at("version-release").as_string();
        if (branch2packages.find(name) == branch2packages.end()) {
            onlyBranch1packages.emplace_back(package);
        } else {
            for (const boost::json::value& other_package : branch2.at("packages").as_array()) {
                if (other_package.at("name").as_string() == name) {
                    if (other_package.at("version-release").as_string() < version) {
                        differentVersionPackages.emplace_back(package);
                    }
                    break;
                }
            }
        }
    }

    for (const boost::json::value& package : branch2.at("packages").as_array()) {
        const boost::json::string name = package.at("name").as_string();
        if (branch1packages.find(name) == branch1packages.end()) {
            onlyBranch2packages.emplace_back(package);
        }
    }

    boost::json::object differences;
    differences["onlyBranch1packages"] = onlyBranch1packages;
    differences["onlyBranch2packages"] = onlyBranch2packages;
    differences["differentVersionPackages"] = differentVersionPackages;

    return differences;
}
