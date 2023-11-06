#include "lib/include/comparepackages.hpp"

int main() {
    std::vector<std::string> branches = {"sisyphus", "p10"};
    std::vector<std::string> architectures = {"x86_64"};

    boost::json::object results;
    std::cout << "for begins" << std::endl;
    for (const std::string& branch : branches) {
        results[branch] = boost::json::object();
        for (const std::string& arch : architectures) {
            boost::json::value branch_packages = getPackages(branch, arch);
            results.at(branch).at(arch) = comparePackages(branch_packages, branch_packages);
        }
    }
    std::cout << "for ends" << std::endl;
    std::ofstream output("results.json");
    output << boost::json::serialize(results);
    output.close();

    return 0;
}
