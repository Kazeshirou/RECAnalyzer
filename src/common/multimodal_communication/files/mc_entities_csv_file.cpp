#include "mc_entities_csv_file.hpp"

#include <fmt/format.h>
#include <fstream>

#include "logger.hpp"

namespace mc::files::csv {

bool to_file(const case_t& cur_case, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        my_log::Logger::critical("mc::files::csv::to_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    if (!cur_case.size()) {
        my_log::Logger::critical("mc::files::csv::to_file",
                                 "Передан пустой эксперимент");
        return true;
    }

    for (size_t i{0}; i < cur_case[0]->size(); i++) {
        for (const auto entry : cur_case) {
            if (entry->check_bit(i)) {
                file << "1 | ";
            } else {
                file << "0 | ";
            }
        }
        file << std::endl;
    }
    for (const auto entry : cur_case) {
        switch (entry->get_type()) {
            case mc::ENTRY_TYPE::TRANSACTION: {
                file << " tran |";
                break;
            }
            case mc::ENTRY_TYPE::SET: {
                file << " set |";
                break;
            }
            case mc::ENTRY_TYPE::RULE: {
                file << " rule |";
                break;
            }
            case mc::ENTRY_TYPE::CLUSTER: {
                file << " cluster |";
                break;
            }
            default:
                file << " |";
                break;
        }
    }
    file << std::endl;

    for (const auto entry : cur_case) {
        switch (entry->get_type()) {
            case mc::ENTRY_TYPE::TRANSACTION: {
                auto tran = static_cast<mc::transaction_t*>(entry);
                file << fmt::format(" {} |", tran->ts1);
                break;
            }
            case mc::ENTRY_TYPE::SET: {
                auto set = static_cast<mc::set_t*>(entry);
                file << fmt::format(" {} |", set->support);
                break;
            }
            case mc::ENTRY_TYPE::RULE: {
                auto rule = static_cast<mc::rule_t*>(entry);
                file << fmt::format(" {} |", rule->support);
                break;
            }
            case mc::ENTRY_TYPE::CLUSTER: {
                auto cluster = static_cast<mc::cluster_t*>(entry);
                file << fmt::format(" {} |", cluster->cluster);
                break;
            }
            default:
                file << " |";
                break;
        }
    }
    file << std::endl;
    for (const auto entry : cur_case) {
        switch (entry->get_type()) {
            case mc::ENTRY_TYPE::TRANSACTION: {
                auto tran = static_cast<mc::transaction_t*>(entry);
                file << fmt::format(" {} |", tran->ts2);
                break;
            }
            case mc::ENTRY_TYPE::RULE: {
                auto rule = static_cast<mc::rule_t*>(entry);
                file << fmt::format(" {} |", rule->confidence);
                break;
            }
            default:
                file << " |";
                break;
        }
    }

    file << std::endl;
    for (const auto entry : cur_case) {
        switch (entry->get_type()) {
            case mc::ENTRY_TYPE::RULE: {
                auto rule = static_cast<mc::rule_t*>(entry);
                file << fmt::format("{} |", rule->target);
                break;
            }
            default:
                file << " |";
                break;
        }
    }
    file << std::endl;
    return true;
}

}  // namespace mc::files::csv