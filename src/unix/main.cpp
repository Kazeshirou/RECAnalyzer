#include "analyzer.hpp"
#include "args_parser.hpp"

int main(int argc, char* argv[]) {
    auto args = args_parser(argc, argv);

    Analyzer analyzer;

    return analyzer.run(args["cfg"].as<std::string>());
}