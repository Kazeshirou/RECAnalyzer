#include "mc_entities_binary_file.hpp"

#include <fmt/core.h>
#include <fstream>

#include "logger.hpp"

namespace mc::files::binary {

bool to_file(const case_t& cur_case, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        my_log::Logger::critical("mc::files::binary::to_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    auto write_number = [&](auto number) {
        file.write(reinterpret_cast<const char*>(&number), sizeof(number));
    };

    write_number(cur_case.size());
    for (const auto entry : cur_case) {
        write_number(entry->size());
        write_number(entry->data().size());
        for (size_t i{0}; i < entry->data().size(); i++) {
            write_number(entry->data()[i]);
        }
        write_number(entry->get_type());
        switch (entry->get_type()) {
            case mc::ENTRY_TYPE::TRANSACTION: {
                auto tran = static_cast<mc::transaction_t*>(entry);
                write_number(tran->ts1);
                write_number(tran->ts2);
                break;
            }
            case mc::ENTRY_TYPE::SET: {
                auto set = static_cast<mc::set_t*>(entry);
                write_number(set->support);
                break;
            }
            case mc::ENTRY_TYPE::RULE: {
                auto rule = static_cast<mc::rule_t*>(entry);
                write_number(rule->support);
                write_number(rule->confidence);
                write_number(rule->target);
                break;
            }
            case mc::ENTRY_TYPE::CLUSTER: {
                auto cluster = static_cast<mc::cluster_t*>(entry);
                write_number(cluster->cluster);
                break;
            }
            default:
                break;
        }
    }

    return true;
}

bool from_file(const std::string& filename, case_t& cur_case) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        my_log::Logger::critical("mc::files::binary::from_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    auto read_number = [&](auto& number) {
        file.read(reinterpret_cast<char*>(&number), sizeof(number));
    };

    size_t size;
    read_number(size);
    cur_case.resize(size);
    for (size_t i{0}; i < cur_case.size(); i++) {
        read_number(size);
        Bit_mask mask(size);
        read_number(size);
        mask.data().resize(size);
        for (size_t j{0}; j < mask.data().size(); j++) {
            read_number(mask.data()[j]);
        }

        mc::ENTRY_TYPE type;
        read_number(type);
        mc::entry_t* cur_entry;
        switch (type) {
            case mc::ENTRY_TYPE::TRANSACTION: {
                auto tran = new mc::transaction_t(mask, 0, 0);
                read_number(tran->ts1);
                read_number(tran->ts2);
                cur_entry = tran;
                break;
            }
            case mc::ENTRY_TYPE::SET: {
                auto set = new mc::set_t(mask, 0);
                read_number(set->support);
                cur_entry = set;
                break;
            }
            case mc::ENTRY_TYPE::RULE: {
                auto rule = new mc::rule_t(mask, 0, 0, 0);
                read_number(rule->support);
                read_number(rule->confidence);
                read_number(rule->target);
                cur_entry = rule;
                break;
            }
            case mc::ENTRY_TYPE::CLUSTER: {
                auto cluster = new mc::cluster_t(mask, 0);
                read_number(cluster->cluster);
                cur_entry = cluster;
                break;
            }
            default:
                cur_entry = new mc::entry_t(mask);
        }
        cur_case[i] = cur_entry;
    }

    return true;
}

}  // namespace mc::files::binary