#include "lib/include/comparepackages.hpp"

int main(int argc, char* argv[]) {
    std::string branch1 = "sisyphus";
    std::string branch2 = "p10";

    boost::json::value branch1packages = getPackages(branch1);
    boost::json::value branch2packages = getPackages(branch2);

    boost::json::object results = comparePackages(branch1packages, branch2packages);

    std::ofstream output("results.json");
    output << boost::json::serialize(results);
    output.close();

    return 0;
}

