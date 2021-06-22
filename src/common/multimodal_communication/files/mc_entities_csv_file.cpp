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

    for (const auto entry : cur_case) {
        for (size_t i{0}; i < entry->size(); i++) {
            if (entry->check_bit(i)) {
                file << "1 | ";
            } else {
                file << "0 | ";
            }
        }

        switch (entry->get_type()) {
            case mc::ENTRY_TYPE::TRANSACTION: {
                auto tran = static_cast<mc::transaction_t*>(entry);
                file << fmt::format("tran | {} | {}\n", tran->ts1, tran->ts2);
                break;
            }
            case mc::ENTRY_TYPE::SET: {
                auto set = static_cast<mc::set_t*>(entry);
                file << fmt::format("set | {}\n", set->support);
                break;
            }
            case mc::ENTRY_TYPE::RULE: {
                auto rule = static_cast<mc::rule_t*>(entry);
                file << fmt::format("rule | {} | {}\n", rule->support,
                                    rule->confidence);
                break;
            }
            case mc::ENTRY_TYPE::CLUSTER: {
                auto cluster = static_cast<mc::cluster_t*>(entry);
                file << fmt::format("cluster | {}\n", cluster->cluster);
                break;
            }
            default:
                file << "\n";
                break;
        }
    }

    return true;
}

}  // namespace mc::files::csv