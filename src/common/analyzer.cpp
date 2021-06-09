#include "analyzer.hpp"

#include <fmt/format.h>
#include <exception>
#include <fstream>
#include <regex>

#include "apriori.hpp"
#include "clope.hpp"
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
        process_rec_template(*rec_template_cfg);
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

bool Analyzer::process_rec_template(const nlohmann::json& rec_template_cfg) {
    if (auto base_json = rec_template_cfg.find("base_rec_template");
        base_json != rec_template_cfg.end()) {
        auto base             = base_json.value().get<std::string>();
        auto rec_template_opt = read_rec_template(base);
        if (rec_template_opt.has_value()) {
            rec_template_ = rec_template_opt.value();
            my_log::Logger::info(
                "analyzer",
                fmt::format("Считан базовый шаблон разметки {}", base));
        } else {
            my_log::Logger::info(
                "analyzer",
                fmt::format("Не удалось считать базовый шаблон разметки {}",
                            base));
            return false;
        }
    } else {
        my_log::Logger::info("analyzer", "Базовый шаблон разметки не указан");
        rec_template_ = rec::rec_template_t{};
    }

    std::string ext = process_output_file_extension(rec_template_cfg);

    std::vector<fs::path> filenames = process_files(rec_template_cfg);
    if (!filenames.size()) {
        my_log::Logger::info(
            "analyzer", "Не найдено файлов разметки для расширения шаблона");
    } else {
        size_t success{0};
        for (const auto& filename : filenames) {
            auto rec_entry_opt = read_rec_entry(filename.string());
            if (!rec_entry_opt.has_value()) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format("Не удалось распарсить файл разметки: {}",
                                filename.string()));
                continue;
            }
            ++success;
        }
        my_log::Logger::info(
            "analyzer", fmt::format("Удалось распарсить файлы разметки: {}/{}",
                                    success, filenames.size()));
    }

    std::string output_filename = fmt::format(
        "{}/{}_rec_template.{}", analisys_folder_, my_log::Logger::time(), ext);
    bool rv = true;
    if (!write_rec_template(rec_template_, output_filename)) {
        rv = false;
    } else {
        my_log::Logger::info(
            "analyzer",
            fmt::format("Шаблон разметки записан в {}", output_filename));
    }

    return rv;
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
        bool current_result = process_transaction(transaction, i);
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

bool Analyzer::process_transaction(const nlohmann::json& transactions_cfg,
                                   size_t                uniq_num) {
    std::vector<fs::path> filenames = process_files(transactions_cfg);

    if (!filenames.size()) {
        my_log::Logger::warning("analyzer",
                                "Не было найдено ни одного входного файла");
        return false;
    }

    std::string type;
    if (auto type_json = transactions_cfg.find("type");
        type_json != transactions_cfg.end()) {
        type = type_json.value().get<std::string>();
        my_log::Logger::info(
            "analyzer", fmt::format("Метод выделения транзакций: {}", type));
    } else {
        my_log::Logger::info(
            "analyzer",
            "Метод выделения транзакций не указан, по умолчанию: time_slots");
    }

    mc::transaction::algorithm::Transaction_mining_interface* alg = nullptr;
    if (type == "time_slots") {
        alg = new mc::transaction::algorithm::By_time_slots_mining;
    } else if (type == "window") {
        using Window = mc::transaction::algorithm::Window_mining;
        auto                      real_alg = new Window;
        Window::window_settings_t settings;

        if (auto size = transactions_cfg.find("size");
            size == transactions_cfg.end()) {
            my_log::Logger::warning("analyzer",
                                    "Не указана настройка метода size, будет "
                                    "использовано значение по умолчанию");
        } else {
            settings.size = size.value().get<size_t>();
        }

        if (auto step_multiplier = transactions_cfg.find("step_multiplier");
            step_multiplier == transactions_cfg.end()) {
            my_log::Logger::warning(
                "analyzer",
                "Не указана настройка метода step_multiplier, будет "
                "использовано значение по умолчанию");
        } else {
            settings.step_multiplier = step_multiplier.value().get<double>();
        }

        my_log::Logger::info(
            "analyzer",
            fmt::format("Настройки метода: size = {}; step_multiplier = {}",
                        settings.size, settings.step_multiplier));
        real_alg->set_window_settings(settings);
        alg = real_alg;
        type +=
            fmt::format("_s{}_m{}", settings.size, settings.step_multiplier);
    } else if (type == "target_tier") {
        using Target_tier = mc::transaction::algorithm::Target_tier_mining;
        auto                         real_alg = new Target_tier;
        Target_tier::tier_settings_t settings;


        if (auto target_tier = transactions_cfg.find("target_tier");
            target_tier == transactions_cfg.end()) {
            my_log::Logger::warning(
                "analyzer", "Не указана настройка метода target_tier, будет "
                            "использовано значение по умолчанию");
        } else {
            settings.target_tier = target_tier.value().get<std::string>();
        }

        if (auto ignore_intervals_without_target_tier =
                transactions_cfg.find("ignore_intervals_without_target_tier");
            ignore_intervals_without_target_tier == transactions_cfg.end()) {
            my_log::Logger::warning(
                "analyzer", "Не указана настройка метода "
                            "ignore_intervals_without_target_tier, будет "
                            "использовано значение по умолчанию");
        } else {
            settings.ignore_intervals_without_target_tier =
                ignore_intervals_without_target_tier.value().get<bool>();
        }

        my_log::Logger::info(
            "analyzer",
            fmt::format("Настройки метода: target_tier = {}; "
                        "ignore_intervals_without_target_tier = {}",
                        settings.target_tier,
                        settings.ignore_intervals_without_target_tier));
        real_alg->set_tier_settings(settings);
        alg = real_alg;
        type +=
            fmt::format("_t{}_i{}", settings.target_tier,
                        settings.ignore_intervals_without_target_tier ? 1 : 0);
    } else {
        my_log::Logger::warning(
            "analyzer",
            fmt::format("Неизвестный метод выделения транзакций: {}", type));
        return false;
    }

    mc::transaction::algorithm::transaction_mining_settings_t settings;
    settings.annotation_settings.resize(rec_template_.annotations.size());
    process_mining_settings(transactions_cfg, settings);
    alg->set_settings(settings);

    mc::case_t new_case;
    size_t     success{0};
    for (const auto& filename : filenames) {
        auto rec_entry_opt = read_rec_entry(filename.string());
        if (!rec_entry_opt.has_value()) {
            my_log::Logger::warning(
                "analyzer",
                fmt::format("Не удалось распарсить файл разметки: {}",
                            filename.string()));
            continue;
        }
        auto current_case = alg->run(rec_entry_opt.value());
        if (!current_case.size()) {
            my_log::Logger::warning(
                "analyzer",
                fmt::format("Не удалось выделить ни одной транзакции: {}",
                            filename.string()));
            continue;
        }
        ++success;
        new_case.insert(new_case.end(), current_case.begin(),
                        current_case.end());
        my_log::Logger::info(
            "analyzer", fmt::format("Из {} выделено {} транзакций",
                                    filename.string(), current_case.size()));
        current_case.clear();
    }
    my_log::Logger::info("analyzer", fmt::format("Всего выделено {} транзакций",
                                                 new_case.size()));
    my_log::Logger::info("analyzer",
                         fmt::format("Успешно обработано файлов {}/{}", success,
                                     filenames.size()));

    std::string ext = process_output_file_extension(transactions_cfg);

    std::string output_filename =
        fmt::format("{}/{}{}_{}_tran.{}", analisys_folder_,
                    my_log::Logger::time(), uniq_num, type, ext);
    bool rv = true;
    if (!write_case(new_case, output_filename)) {
        rv = false;
    } else {
        my_log::Logger::info("analyzer",
                             fmt::format("Выделенные транзакции записаны в {}",
                                         output_filename));
    }

    delete alg;
    return rv;
}

bool Analyzer::process_set(const nlohmann::json& sets_cfg) {
    std::vector<fs::path> filenames = process_files(sets_cfg);

    if (!filenames.size()) {
        my_log::Logger::warning("analyzer",
                                "Не было найдено ни одного входного файла");
        return false;
    }

    mc::case_t new_case;
    for (const auto& filename : filenames) {
        auto cur_case_opt = read_case(filename.string());
        if (!cur_case_opt.has_value()) {
            my_log::Logger::warning(
                "analyzer",
                fmt::format("Не удалось считать {}", filename.string()));
            continue;
        }
        auto& cur_case = cur_case_opt.value();
        my_log::Logger::info("analyzer",
                             fmt::format("Из {} считано {} транзакций",
                                         filename.string(), cur_case.size()));

        if (cur_case.size()) {
            new_case.insert(new_case.end(), cur_case.begin(), cur_case.end());
        }
        cur_case.clear();
    }

    my_log::Logger::info("analyzer", fmt::format("Всего считано {} транзакций",
                                                 new_case.size()));


    mc::assotiation_mining::algorithm::apriori_settings_t settings;
    if (sets_cfg.contains("min_support")) {
        settings.min_support = sets_cfg["min_support"].get<double>();
    }
    if (sets_cfg.contains("max_support")) {
        settings.max_support = sets_cfg["max_support"].get<double>();
    }

    my_log::Logger::info(
        "analyzer",
        fmt::format(
            "Настройки выделения наборов: min_support = {}; max_support = {}",
            settings.min_support, settings.max_support));


    auto ignore = process_ignore(sets_cfg);

    for (Bit_mask* tran : new_case) {
        (*tran) &= ignore;
    }

    auto sets =
        mc::assotiation_mining::algorithm::apriori_sets(settings, new_case);

    if (!sets.size()) {
        my_log::Logger::warning(
            "analyzer",
            "Не удалось выделить ни одного часто встречающегося набора");
        return true;
    }

    my_log::Logger::info("analyzer",
                         fmt::format("Выделено {} наборов", sets.size()));

    std::string ext = process_output_file_extension(sets_cfg);

    std::string output_filename = fmt::format("{}/{}_set.{}", analisys_folder_,
                                              my_log::Logger::time(), ext);
    bool        rv              = true;
    if (!write_case(sets, output_filename)) {
        rv = false;
    } else {
        my_log::Logger::info(
            "analyzer",
            fmt::format("Выделенные часто встречающиеся наборы записаны в {}",
                        output_filename));
    }

    return rv;
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
            fmt::format(
                "Не удалось открыть директорию для поиска файлов {}: {}",
                filename, err.what()));
    } catch (const std::regex_error& err) {
        my_log::Logger::warning(
            "analyzer", fmt::format("Не правильно составлено регулярное "
                                    "выражения для поиска файлов{}: {}",
                                    filename, err.what()));
    } catch (const std::exception& err) {
        my_log::Logger::warning(
            "analyzer", fmt::format("Неожиданная ошибка: {}", err.what()));
    }
    return rv;
}

std::string Analyzer::process_output_file_extension(const nlohmann::json& cfg) {
    std::string ext = "json";
    if (auto ext_json = cfg.find("output_file_extension");
        ext_json != cfg.end()) {
        ext = ext_json.value().get<std::string>();
        if (!ext.size()) {
            ext = "bin";
        }
    }
    return ext;
}

std::vector<fs::path> Analyzer::process_files(const nlohmann::json& cfg) {
    std::vector<fs::path> filenames;
    auto                  input_files_cfg = cfg.find("files");
    if (input_files_cfg == cfg.end()) {
        my_log::Logger::warning("analyzer",
                                "Не указаны исходные файлы (поле files)");
        return filenames;
    }

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

    return filenames;
}

void Analyzer::process_mining_settings(
    const nlohmann ::json&                                     cfg,
    mc::transaction::algorithm::transaction_mining_settings_t& settings) {
    if (auto tiers_json = cfg.find("tiers_settings"); tiers_json != cfg.end()) {
        for (const auto& tier : *tiers_json) {
            auto cur_settings_opt = process_annotation_settings(tier);
            if (!cur_settings_opt.has_value()) {
                continue;
            }
            auto cur_settings = cur_settings_opt.value();
            if (!rec_template_.tiers_map.count(cur_settings.first)) {
                my_log::Logger::warning(
                    "analyzer", fmt::format("Настройка для неизвестного слоя "
                                            "{} будет проигнорирована",
                                            cur_settings.first));
                continue;
            }
            size_t      tier_id = rec_template_.tiers_map[cur_settings.first];
            std::string mode    = "";
            switch (cur_settings.second.type) {
                case mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::ANY:
                    mode = "any";
                    break;
                case mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::FULL:
                    mode = "full";
                    break;
                case mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::PART:
                    mode = "part";
                    break;
                case mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::MOMENT:
                    mode = "moment";
                    break;
            }
            for (size_t i{0}; i < rec_template_.annotations.size(); i++) {
                if (rec_template_.annotations[i].tier != tier_id) {
                    continue;
                }
                settings.annotation_settings[i] = cur_settings.second;
                my_log::Logger::info(
                    "analyzer",
                    fmt::format(
                        "Настройка вхождения {}_{}: mode = {}; value = {};",
                        cur_settings.first, rec_template_.annotations[i].value,
                        mode, cur_settings.second.value));
            }
        }
    }
    if (auto ann_json = cfg.find("annotations_settings");
        ann_json != cfg.end()) {
        for (const auto& ann : *ann_json) {
            auto cur_settings_opt = process_annotation_settings(ann);
            if (!cur_settings_opt.has_value()) {
                continue;
            }
            auto cur_settings = cur_settings_opt.value();
            if (!rec_template_.annotations_map.count(cur_settings.first)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format("Настройка для неизвестной аннотации "
                                "{} будет проигнорирована",
                                cur_settings.first));
                continue;
            }
            settings.annotation_settings
                [rec_template_.annotations_map[cur_settings.first]] =
                cur_settings.second;

            std::string mode = "";
            switch (cur_settings.second.type) {
                case mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::ANY:
                    mode = "any";
                    break;
                case mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::FULL:
                    mode = "full";
                    break;
                case mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::PART:
                    mode = "part";
                    break;
                case mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::MOMENT:
                    mode = "moment";
                    break;
            }
            my_log::Logger::info(
                "analyzer",
                fmt::format("Настройка вхождения {}: mode = {}; value = {};",
                            cur_settings.first, mode,
                            cur_settings.second.value));
        }
    }
}


std::optional<
    std::pair<std::string, mc::transaction::algorithm::annotation_settings_t>>
    Analyzer::process_annotation_settings(const nlohmann ::json& cfg) {
    mc::transaction::algorithm::annotation_settings_t rv;

    std::string name;
    if (auto name_json = cfg.find("name"); name_json == cfg.end()) {
        my_log::Logger::warning("analyzer",
                                "Не указано название слоя/аннотации настройки");
        return {};
    } else {
        name = name_json->get<std::string>();
    }

    if (auto mode_json = cfg.find("mode"); mode_json != cfg.end()) {
        auto mode = mode_json->get<std::string>();
        if (mode == "any") {
            return std::make_pair(name, rv);
        }
        if (mode == "full") {
            rv.type = mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::FULL;
            return std::make_pair(name, rv);
        }

        if (mode == "part") {
            rv.type = mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::PART;
        } else if (mode == "moment") {
            rv.type = mc::transaction::algorithm::ANNOTATION_ENTRY_TYPE::MOMENT;

        } else {
            my_log::Logger::warning(
                "analyzer",
                fmt::format("Указан неизвестный тип вхождения для {}: {}", name,
                            mode));
            return {};
        }

        auto value_json = cfg.find("value");
        if (value_json == cfg.end()) {
            my_log::Logger::warning(
                "analyzer",
                fmt::format("Не указан параметр для типа вхождения {} для {}",
                            name, mode));
            return {};
        }


        return std::make_pair(name, rv);
    }

    my_log::Logger::warning(
        "analyzer", fmt::format("Не указан тип вхождения для {}", name));
    return {};
}

mc::entry_t Analyzer::process_ignore(const nlohmann::json& cfg) {
    mc::entry_t rv(rec_template_.annotations.size());
    if (auto ignore_tiers = cfg.find("ignore_tiers");
        ignore_tiers != cfg.end()) {
        for (const auto& tier_json : *ignore_tiers) {
            auto tier = tier_json.get<std::string>();
            if (!rec_template_.tiers_map.count(tier)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format(
                        "Неизвестный слой для исключения из анализа: {}",
                        tier));
                continue;
            }
            size_t tier_id = rec_template_.tiers_map[tier];
            for (size_t i{0}; i < rec_template_.annotations.size(); i++) {
                if (rec_template_.annotations[i].tier != tier_id) {
                    continue;
                }
                rv.set_bit(i);
                my_log::Logger::info(
                    "analyzer",
                    fmt::format("Исключена из анализа {}_{}", tier,
                                rec_template_.annotations[i].value));
            }
        }
    }

    if (auto ignore_annotations = cfg.find("ignore_annotations");
        ignore_annotations != cfg.end()) {
        for (const auto& ann_json : *ignore_annotations) {
            auto ann = ann_json.get<std::string>();
            if (!rec_template_.annotations_map.count(ann)) {
                my_log::Logger::warning(
                    "analyzer",
                    fmt::format(
                        "Неизвестная аннотация для исключения из анализа: {}",
                        ann));
                continue;
            }
            rv.set_bit(rec_template_.annotations_map[ann]);
            my_log::Logger::info("analyzer",
                                 fmt::format("Исключена из анализа {}", ann));
        }
    }

    rv.invert();

    return rv;
}