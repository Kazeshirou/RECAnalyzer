#include "args_parser.hpp"

cxxopts::ParseResult args_parser(int argc, char* argv[]) {
    cxxopts::Options options(
        "RECAnalyzer", "Допустимые аргументы командной строки RECAnalyzer");
    auto add = options.add_options();
    add("h, help", "Информация об использовании аргументов командной строки");
    add("cfg", "Путь к файлу конфигурации анализа",
        cxxopts::value<std::string>());
    auto result_parse = options.parse(argc, argv);

    if (result_parse.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    if (!result_parse.count("cfg")) {
        std::cout << "Пропущен обязательный аргумент \"cfg\"!" << std::endl;
        exit(1);
    }

    return result_parse;
}
