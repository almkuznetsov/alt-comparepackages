#include "../include/comparepackages.hpp"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

boost::json::value getPackages(const std::string& branch) {
    CURL *curl = curl_easy_init();
    CURLcode res;
    std::string response_data;

    if (curl) {
        const std::string host = "https://rdb.altlinux.org";
        const std::string target = host + "/api/export/branch_binary_packages/" + branch;

        curl_easy_setopt(curl, CURLOPT_URL, target.c_str());
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        std::cout << "downloading " << branch << std::endl;
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize libcurl." << std::endl;
    }
    boost::json::value result = boost::json::parse(response_data);
    return result;
}


boost::json::object comparePackages(const boost::json::value& branch1, const boost::json::value& branch2) {
    std::set<boost::json::string> archs = {
            "x86_64", "i586", "noarch", "aarch64", "armh", "ppc64le", "srpm"
    };
    boost::json::object differences;

    for (const boost::json::string& arch : archs) {
        std::cout << "beginning to parse arch " << arch << std::endl;
        std::set<boost::json::string> branch1packages;
        std::set<boost::json::string> branch2packages;
        for (const boost::json::value &package: branch1.at("packages").as_array()) {
            if (package.at("arch").as_string() == arch) {
                        branch1packages.insert(package.at("name").as_string());
                }
        }
        for (const boost::json::value &package: branch2.at("packages").as_array()) {
            if (package.at("arch").as_string() == arch) {
                branch2packages.insert(package.at("name").as_string());
            }
        }

        boost::json::array onlyBranch1packages;
        boost::json::array onlyBranch2packages;
        boost::json::array differentVersionPackages;
        for (const boost::json::value &package: branch1.at("packages").as_array()) {
            const boost::json::string name = package.at("name").as_string();
            const boost::json::string version = package.at("version").as_string();
            if (branch2packages.find(name) == branch2packages.end()) {
                onlyBranch1packages.emplace_back(package);
            } else {
            for (const boost::json::value& other_package : branch2.at("packages").as_array()) {
                if (other_package.at("name").as_string() == name) {
                    if (other_package.at("version").as_string() < version) {
                        differentVersionPackages.emplace_back(package);
                    }
                    break;
                }
            }
        }
        }

        for (const boost::json::value &package: branch2.at("packages").as_array()) {
            const boost::json::string name = package.at("name").as_string();
            if (branch1packages.find(name) == branch1packages.end()) {
                onlyBranch2packages.emplace_back(package);
            }
        }

        differences[std::string(arch.c_str()) + "_onlyBranch1packages"] = onlyBranch1packages;
        differences[std::string(arch.c_str()) + "_onlyBranch2packages"] = onlyBranch2packages;
        differences[std::string(arch.c_str()) + "_differentVersionPackages"] = differentVersionPackages;

    }
    return differences;
}
