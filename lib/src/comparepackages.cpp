#include "../include/comparepackages.hpp"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int compareVersions(const std::string version1, const std::string version2) {
    int vnum1 = 0, vnum2 = 0;

    for (int i = 0, j = 0; (i < version1.length()
                            || j < version1.length());) {

        while (i < version1.length() && version1[i] != '.') {
            vnum1 = vnum1 * 10 + (version1[i] - '0');
            i++;
        }

        while (j < version2.length() && version2[j] != '.') {
            vnum2 = vnum2 * 10 + (version2[j] - '0');
            j++;
        }

        if (vnum1 > vnum2)
            return 1;
        if (vnum2 > vnum1)
            return -1;

        vnum1 = vnum2 = 0;
        i++;
        j++;
    }
    return 0;
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
        boost::json::array branch1packages;
        boost::json::array branch2packages;
        for (const boost::json::value &package: branch1.at("packages").as_array()) {
            if (package.at("arch").as_string() == arch) {
                branch1packages.emplace_back(package);
                }
        }
        for (const boost::json::value &package: branch2.at("packages").as_array()) {
            if (package.at("arch").as_string() == arch) {
                branch2packages.emplace_back(package);
            }
        }

        boost::json::array onlyBranch1packages;
        boost::json::array onlyBranch2packages;
        boost::json::array differentVersionPackages;
        bool packageFound = false;
        for (const boost::json::value &package : branch1packages) {
            const boost::json::string name = package.at("name").as_string();
            const int epoch = package.at("epoch").as_int64();
            const boost::json::string version = package.at("version").as_string();
            const boost::json::string release = package.at("release").as_string();
            for (const boost::json::value& other_package : branch2packages) {
                if (other_package.at("name").as_string() == name) {
                    const int otherEpoch = other_package.at("epoch").as_int64();
                    if (epoch > otherEpoch) {
                        differentVersionPackages.emplace_back(package);
                    } else {
                        const boost::json::string otherVersion = other_package.at("version").as_string();
                        if (otherEpoch == epoch && compareVersions(version.c_str(), otherVersion.c_str()) == 1) {
                            differentVersionPackages.emplace_back(package);
                        }
                            else {
                            const boost::json::string otherRelease = other_package.at("release").as_string();
                            if (compareVersions(otherVersion.c_str(), version.c_str()) == 0 && compareVersions(release.c_str(), otherRelease.c_str()) == 1) {
                                differentVersionPackages.emplace_back(package);
                            }
                        }
                    }
                    packageFound = true;
                    break;
                }
            }
            if (!packageFound) {
                onlyBranch1packages.emplace_back(package);
            }
        }

        packageFound = false;
        for (const boost::json::value &package : branch2packages) {
            const boost::json::string name = package.at("name").as_string();
            for (const boost::json::value& other_package : branch1packages) {
                if (other_package.at("name").as_string() == name) {
                    packageFound = true;
                    break;
                }
            }
            if (!packageFound) {
                onlyBranch2packages.emplace_back(package);
            }
        }

        differences[std::string(arch.c_str()) + "_onlyBranch1packages"] = onlyBranch1packages;
        differences[std::string(arch.c_str()) + "_onlyBranch2packages"] = onlyBranch2packages;
        differences[std::string(arch.c_str()) + "_differentVersionPackages"] = differentVersionPackages;

    }
    return differences;
}
