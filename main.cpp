#include "lib/include/comparepackages.hpp"
boost::json::value readJsonFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return boost::json::value();
    }

    boost::json::value json;
    try {
        file >> json;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }

    file.close();
    return json;
}


int main() {
    boost::json::value branch1packages = readJsonFile("sisyphus.json");
    boost::json::value branch2packages = readJsonFile("p10.json");

    boost::json::object results = comparePackages(branch1packages, branch2packages);

    std::ofstream output("results.json");
    output << boost::json::serialize(results);
    output.close();

    return 0;
}
