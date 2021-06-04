#include "analyzer.hpp"

#include <fmt/format.h>
#include <exception>
#include <fstream>
#include <regex>

#include "eaf_parser.hpp"
#include "etf_parser.hpp"
#include "logger.hpp"
#include "mc_by_time_slots_mining.hpp"
#include "mc_entities_binary_file.hpp"
#include "mc_entities_json_file.hpp"
#include "mc_target_tier_mining.hpp"
#include "mc_window_mining.hpp"
#include "rec_entry_binary_file.hpp"
#include "rec_entry_json_file.hpp"
#include "rec_template_binary_file.hpp"
#include "rec_template_json_file.hpp"

bool Analyzer::run(const std::string& cfg_filename) {
    std::ifstream cfg_file(cfg_filename);
    if (!cfg_file.is_open()) {
        my_log::Logger::critical(
            "analyzer",
            fmt::format("Не удалось открыть файл {}", cfg_filename));
        return false;
    }

    nlohmann::json cfg;
    try {
        cfg_file >> cfg;
    } catch (const std::exception& exeption) {
        my_log::Logger::critical(
            "analyzer", fmt::format("Не удалось считать json файл {} ({})",
                                    cfg_filename, exeption.what()));
        return false;
    }

    if (auto convert_cfg = cfg.find("convert"); convert_cfg != cfg.end()) {
        my_log::Logger::info("analyzer",
                             "Считана конфигурация для конвертации файлов");
        process_converts(*convert_cfg);
    }

    auto target_folder = cfg.find("target_folder");
    if (target_folder == cfg.end()) {
        my_log::Logger::critical("analyzer",
                                 "Не удалось считать target_folder");
        return false;
    } else {
        my_log::Logger::info(
            "analyzer", fmt::format("Считан путь до целевой директории {}",
                                    target_folder.value().get<std::string>()));
    }

    auto name = cfg.find("name");
    if (name == cfg.end()) {
        my_log::Logger::critical("analyzer", "Не удалось считать name");
        return false;
    } else {
        my_log::Logger::info("analyzer",
                             fmt::format("Считано название текущего анализа {}",
                                         name.value().get<std::string>()));
    }

    analisys_folder_ =
        fmt::format("{}{}", target_folder.value().get<std::string>(),
                    name.value().get<std::string>());
    std::error_code err_code;
    if (!fs::create_directory(analisys_folder_, err_code)) {
        if (err_code) {
            my_log::Logger::critical(
                "analyzer",
                fmt::format("Не удалось создать папку анализа {}: {} ({})",
                            analisys_folder_, err_code.message(),
                            err_code.value()));
            return false;
        }
        my_log::Logger::info(
            "analyzer",
            fmt::format("Папка анализа {} уже существует", analisys_folder_));
    } else {
        my_log::Logger::info(
            "analyzer",
            fmt::format("Создана папка текущего анализа {}", analisys_folder_));
    }

    if (auto rec_template_cfg = cfg.find("rec_template");
        rec_template_cfg == cfg.end()) {
        my_log::Logger::warning("analyzer",
                                "Не удалось считать поле rec_template");
    } else {
        my_log::Logger::info(
            "analyzer",
            fmt::format("Считан путь до файла шаблона разметки {}",
                        rec_template_cfg.value().get<std::string>()));
        process_rec_template(rec_template_cfg.value().get<std::string>());
    }

    if (auto transactions_cfg = cfg.find("transactions");
        transactions_cfg != cfg.end()) {
        my_log::Logger::info("analyzer",
                             "Считана конфигурация для выделения транзакций");
        process_transactions(*transactions_cfg);
    }

    if (auto sets_cfg = cfg.find("sets"); sets_cfg != cfg.end()) {
        my_log::Logger::info(
            "analyzer",
            "Считана конфигурация для выделения часто встречающихся наборов");
        process_sets(*sets_cfg);
    }

    if (auto rules_cfg = cfg.find("rules"); rules_cfg != cfg.end()) {
        my_log::Logger::info(
            "analyzer",
            "Считана конфигурация для выделения ассоциативных правил");
        process_rules(*rules_cfg);
    }

    if (auto clustering_cfg = cfg.find("clustering");
        clustering_cfg != cfg.end()) {
        my_log::Logger::info("analyzer",
                             "Считана конфигурация для кластеризации");
        process_clusterings(*clustering_cfg);
    }

    return true;
}

bool Analyzer::process_rec_template(const std::string& filename) {
    auto rec_template_opt = read_rec_template(filename);
    if (rec_template_opt.has_value()) {
        rec_template_ = rec_template_opt.value();
        return true;
    }

    return false;
}


bool Analyzer::process_converts(const nlohmann::json& convert_cfg) {
    bool   rv            = true;
    size_t convert_count = convert_cfg.size();
    my_log::Logger::info(
        "analyzer",
        fmt::format("Считано {} конфигураций для конвертации файлов",
                    convert_count));
    size_t i{0};
    size_t success{0};
    for (const auto& convert : convert_cfg) {
        my_log::Logger::info(
            "analyzer",
            fmt::format("    Выполняется конвертация файла {}/{}...", ++i,
                        convert_count));
        bool current_result = process_convert(convert);
        my_log::Logger::info(
            "analyzer",
            fmt::format("    Конвертация файла {}",
                        current_result ? "прошла успешно" : "провалена"));
        rv &= current_result;
        if (current_result) {
            ++success;
        }
    }

    my_log::Logger::info("analyzer", fmt::format("Успешно выполнено {}/{}",
                                                 success, convert_count));
    return rv;
}

bool Analyzer::process_transactions(const nlohmann::json& transactions_cfg) {
    bool   rv                 = true;
    size_t transactions_count = transactions_cfg.size();
    my_log::Logger::info(
        "analyzer",
        fmt::format("Считано {} конфигураций для выделения транзакций",
                    transactions_count));
    size_t i{0};
    size_t success{0};
    for (const auto& transaction : transactions_cfg) {
        my_log::Logger::info(
            "analyzer",
            fmt::format("    Выполняется выделение транзакций {}/{}...", ++i,
                        transactions_count));
        bool current_result = process_transaction(transaction);
        my_log::Logger::info(
            "analyzer",
            fmt::format("    Выделение транзакций {}",
                        current_result ? "прошло успешно" : "провалено"));
        rv &= current_result;
        if (current_result) {
            ++success;
        }
    }

    my_log::Logger::info("analyzer", fmt::format("Успешно выполнено {}/{}",
                                                 success, transactions_count));
    return rv;
}

bool Analyzer::process_sets(const nlohmann::json& sets_cfg) {
    bool   rv         = true;
    size_t sets_count = sets_cfg.size();
    my_log::Logger::info(
        "analyzer",
        fmt::format(
            "Считано {} конфигураций для выделения часто встречающихся наборов",
            sets_count));
    size_t i{0};
    size_t success{0};
    for (const auto& set : sets_cfg) {
        my_log::Logger::info("analyzer",
                             fmt::format("    Выполняется выделение часто "
                                         "встречающихся наборов {}/{}...",
                                         ++i, sets_count));
        bool current_result = process_set(set);
        my_log::Logger::info(
            "analyzer",
            fmt::format("    Выделение часто встречающихся наборов {}",
                        current_result ? "прошло успешно" : "провалено"));
        rv &= current_result;
        if (current_result) {
            ++success;
        }
    }

    my_log::Logger::info("analyzer", fmt::format("Успешно выполнено {}/{}",
                                                 success, sets_count));
    return rv;
}

bool Analyzer::process_rules(const nlohmann::json& rules_cfg) {
    bool   rv          = true;
    size_t rules_count = rules_cfg.size();
    my_log::Logger::info(
        "analyzer",
        fmt::format(
            "Считано {} конфигураций для выделения ассоциативных правил",
            rules_count));
    size_t i{0};
    size_t success{0};
    for (const auto& rule : rules_cfg) {
        my_log::Logger::info(
            "analyzer",
            fmt::format(
                "    Выполняется выделение ассоциативных правил {}/{}...", ++i,
                rules_count));
        bool current_result = process_rule(rule);
        my_log::Logger::info(
            "analyzer",
            fmt::format("    Выделение ассоциативных правил {}",
                        current_result ? "прошло успешно" : "провалено"));
        rv &= current_result;
        if (current_result) {
            ++success;
        }
    }

    my_log::Logger::info("analyzer", fmt::format("Успешно выполнено {}/{}",
                                                 success, rules_count));
    return rv;
}

bool Analyzer::process_clusterings(const nlohmann::json& clustering_cfg) {
    bool   rv               = true;
    size_t clustering_count = clustering_cfg.size();
    my_log::Logger::info(
        "analyzer", fmt::format("Считано {} конфигураций для кластеризации",
                                clustering_count));
    size_t i{0};
    size_t success{0};
    for (const auto& transaction : clustering_cfg) {
        my_log::Logger::info(
            "analyzer", fmt::format("    Выполняется кластеризация {}/{}...",
                                    ++i, clustering_count));
        bool current_result = process_clustering(transaction);
        my_log::Logger::info(
            "analyzer",
            fmt::format("    Кластеризация {}",
                        current_result ? "прошла успешно" : "провалена"));
        rv &= current_result;
        if (current_result) {
            ++success;
        }
    }

    my_log::Logger::info("analyzer", fmt::format("Успешно выполнено {}/{}",
                                                 success, clustering_count));
    return rv;
}

bool Analyzer::process_convert(const nlohmann::json& convert_cfg) {
    auto file_type_json = convert_cfg.find("type");
    if (file_type_json == convert_cfg.end()) {
        my_log::Logger::warning("analyzer", "Не удалось считать поле type");
        return false;
    }
    auto file_type = file_type_json.value().get<std::string>();

    auto from_json = convert_cfg.find("from");
    if (from_json == convert_cfg.end()) {
        my_log::Logger::warning("analyzer", "Не удалось считать поле from");
        return false;
    }
    auto from = from_json.value().get<std::string>();

    auto to_json = convert_cfg.find("to");
    if (to_json == convert_cfg.end()) {
        my_log::Logger::warning("analyzer", "Не удалось считать поле to");
        return false;
    }
    auto to = to_json.value().get<std::string>();

    if (file_type == "rec_template") {
        auto opt = read_rec_template(from);
        if (!opt.has_value()) {
            return false;
        }
        return write_rec_template(opt.value(), to);
    } else if (file_type == "rec_entry") {
        rec_template_ = rec::rec_template_t{};
        auto opt      = read_rec_entry(from);
        if (!opt.has_value()) {
            return false;
        }
        return write_rec_entry(opt.value(), to);
    } else if (file_type == "case") {
        auto opt = read_case(from);
        if (!opt.has_value()) {
            return false;
        }
        return write_case(opt.value(), to);
    }

    my_log::Logger::warning("analyzer",
                            "Не известный тип файла для конвертации");
    return false;
}

bool Analyzer::process_transaction(const nlohmann::json& transactions_cfg) {
    auto input_files_cfg = transactions_cfg.find("files");
    if (input_files_cfg == transactions_cfg.end()) {
        my_log::Logger::warning("analyzer",
                                "Не указаны исходные файлы (поле files)");
        return false;
    }

    std::vector<fs::path> filenames;
    for (const auto& file : *input_files_cfg) {
        auto curr_files_path = file.get<std::string>();
        auto files           = find_files(curr_files_path);
        if (!files.size()) {
            my_log::Logger::warning(
                "analyzer", fmt::format("Не было найдено ни одного файла "
                                        "соответствующего {}",
                                        curr_files_path));
            continue;
        }

        my_log::Logger::info("analyzer",
                             fmt::format("Было найдено {} файлов "
                                         "соответствующих {}:",
                                         files.size(), curr_files_path));
        for (const auto& filename : files) {
            my_log::Logger::info("analyzer",
                                 fmt::format("    {}", filename.string()));
        }
        filenames.insert(filenames.end(), files.begin(), files.end());
    }


    if (!filenames.size()) {
        my_log::Logger::warning("analyzer",
                                "Не было найдено ни одного входного файла");
        return false;
    }
    return true;
}

bool Analyzer::process_set([[maybe_unused]] const nlohmann::json& sets_cfg) {
    return true;
}

bool Analyzer::process_rule([[maybe_unused]] const nlohmann::json& rules_cfg) {
    return true;
}

bool Analyzer::process_clustering(
    [[maybe_unused]] const nlohmann::json& clustering_cfg) {
    return true;
}

FILE_EXTENSION Analyzer::file_extension(const std::string& filename) {
    std::regex etf_regex(".*\\.etf$");
    std::regex eaf_regex(".*\\.eaf$");
    std::regex json_regex(".*\\.json$");
    std::regex csv_regex(".*\\.csv$");

    if (std::regex_match(filename, etf_regex)) {
        return FILE_EXTENSION::ETF;
    }
    if (std::regex_match(filename, eaf_regex)) {
        return FILE_EXTENSION::EAF;
    }
    if (std::regex_match(filename, json_regex)) {
        return FILE_EXTENSION::JSON;
    }
    if (std::regex_match(filename, csv_regex)) {
        return FILE_EXTENSION::CSV;
    }

    return FILE_EXTENSION::UNKNOWN;
}

std::optional<rec::rec_template_t> Analyzer::read_rec_template(
    const std::string& filename) {
    if (!filename.size()) {
        my_log::Logger::warning(
            "analyzer", "Передан пустой путь до файла шаблона разметки");
        return {};
    }
    auto ext = file_extension(filename);
    switch (ext) {
        case FILE_EXTENSION::ETF: {
            auto rec_template_opt = rec::etf::Parser::parse_file(filename);
            if (!rec_template_opt.has_value()) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format(
                        "Не удалось распарсить etf файл шаблона разметки {}",
                        filename));
                return {};
            }

            return rec_template_opt.value();
        } break;
        case FILE_EXTENSION::JSON: {
            rec::rec_template_t rec_template;
            if (!rec::files::json::from_file(filename, rec_template)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format(
                        "Не удалось распарсить json файл шаблона разметки {}",
                        filename));
                return {};
            }

            return rec_template;
        } break;
        default: {
            rec::rec_template_t rec_template;
            if (!rec::files::binary::from_file(filename, rec_template)) {
                my_log::Logger::warning(
                    "analyzer", fmt::format("Не удалось распарсить binary файл "
                                            "шаблона разметки  {}",
                                            filename));
                return {};
            }

            return rec_template;
        }
    }
}

bool Analyzer::write_rec_template(const rec::rec_template_t& rec_template,
                                  const std::string&         filename) {
    if (!filename.size()) {
        my_log::Logger::warning(
            "analyzer", "Передан пустой путь до нового файла шаблона разметки");
        return false;
    }
    auto ext = file_extension(filename);
    switch (ext) {
        case FILE_EXTENSION::JSON:
            if (!rec::files::json::to_file(rec_template, filename)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format("Не удалось записать шаблон разметки в json {}",
                                filename));
                return false;
            }

            return true;
        default: {
            if (!rec::files::binary::to_file(rec_template, filename)) {
                my_log::Logger::warning(
                    "analyzer", fmt::format("Не удалось записать шаблон "
                                            "разметки в бинарный файл {}",
                                            filename));
                return false;
            }

            return true;
        }
    }
}

std::optional<rec::rec_entry_t> Analyzer::read_rec_entry(
    const std::string& filename) {
    if (!filename.size()) {
        my_log::Logger::warning("analyzer",
                                "Передан пустой путь до файла разметки");
        return {};
    }
    auto ext = file_extension(filename);
    switch (ext) {
        case FILE_EXTENSION::EAF: {
            auto opt = rec::eaf::Parser::parse_file(filename);
            if (!opt.has_value()) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format("Не удалось распарсить eaf файл разметки {}",
                                filename));
                return {};
            }

            return rec::rec_entry_t{rec_template_, opt.value()};
        } break;
        case FILE_EXTENSION::JSON: {
            rec::rec_entry_t rec_entry{rec_template_};
            if (!rec::files::json::from_file(filename, rec_entry)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format("Не удалось распарсить json файл разметки {}",
                                filename));
                return {};
            }

            return rec_entry;
        } break;
        default: {
            rec::rec_entry_t rec_entry{rec_template_};
            if (!rec::files::binary::from_file(filename, rec_entry)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format("Не удалось распарсить binary файл разметки {}",
                                filename));
                return {};
            }

            return rec_entry;
        }
    }
}

bool Analyzer::write_rec_entry(const rec::rec_entry_t& rec_entry,
                               const std::string&      filename) {
    if (!filename.size()) {
        my_log::Logger::warning("analyzer",
                                "Передан пустой путь до нового файла разметки");
        return false;
    }
    auto ext = file_extension(filename);
    switch (ext) {
        case FILE_EXTENSION::JSON:
            if (!rec::files::json::to_file(rec_entry, filename)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format("Не удалось записать файл разметки в json {}",
                                filename));
                return false;
            }

            return true;
        default: {
            if (!rec::files::binary::to_file(rec_entry, filename)) {
                my_log::Logger::warning(
                    "analyzer", fmt::format("Не удалось записать файл "
                                            "разметки в бинарный файл {}",
                                            filename));
                return false;
            }

            return true;
        }
    }
}

std::optional<mc::case_t> Analyzer::read_case(const std::string& filename) {
    if (!filename.size()) {
        my_log::Logger::warning("analyzer",
                                "Передан пустой путь до файла эксперимента");
        return {};
    }
    auto ext = file_extension(filename);
    switch (ext) {
        case FILE_EXTENSION::JSON: {
            mc::case_t new_case;
            if (!mc::files::json::from_file(filename, new_case)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format(
                        "Не удалось распарсить json файл эксперимента {}",
                        filename));
                return {};
            }

            return new_case;
        } break;
        default: {
            mc::case_t new_case;
            if (!mc::files::binary::from_file(filename, new_case)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format(
                        "Не удалось распарсить binary файл эксперимента {}",
                        filename));
                return {};
            }

            return new_case;
        }
    }
}

bool Analyzer::write_case(const mc::case_t&  clusters,
                          const std::string& filename) {
    if (!filename.size()) {
        my_log::Logger::warning(
            "analyzer", "Передан пустой путь до нового файла эксперимента");
        return false;
    }
    auto ext = file_extension(filename);
    switch (ext) {
        case FILE_EXTENSION::JSON:
            if (!mc::files::json::to_file(clusters, filename)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format(
                        "Не удалось записать файл эксперимента в json {}",
                        filename));
                return false;
            }

            return true;
        default: {
            if (!mc::files::binary::to_file(clusters, filename)) {
                my_log::Logger::warning(
                    "analyzer", fmt::format("Не удалось записать файл "
                                            "эксперимента в бинарный файл {}",
                                            filename));
                return false;
            }

            return true;
        }
    }
}

std::vector<fs::path> Analyzer::find_files(const std::string& filename) {
    std::vector<fs::path> rv;
    try {
        fs::path   path(filename);
        std::regex filename_regex(path.filename().string());

        using iterator = fs::recursive_directory_iterator;
        const iterator end;
        for (iterator iter{path.remove_filename()}; iter != end; ++iter) {
            const std::string current_filename =
                iter->path().filename().string();
            if (fs::is_regular_file(*iter) &&
                std::regex_match(current_filename, filename_regex)) {
                rv.push_back(*iter);
            }
        }
    } catch (const fs::filesystem_error& err) {
        my_log::Logger::warning(
            "analyzer",
            fmt::format("Не удалось открыть директорию для поиска файлов: {}",
                        filename));
    } catch (const std::regex_error& err) {
        my_log::Logger::warning(
            "analyzer", fmt::format("Не правильно составлено регулярное "
                                    "выражения для поиска файлов: {}",
                                    filename));
    } catch (const std::exception& err) {
        my_log::Logger::warning(
            "analyzer", fmt::format("Неожиданная ошибка: {}", err.what()));
    }
    return rv;
}