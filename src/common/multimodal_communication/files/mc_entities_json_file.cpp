#include "mc_entities_json_file.hpp"

#include <fmt/core.h>
#include <fstream>
#include <nlohmann/nlohmann.hpp>

#include "logger.hpp"

namespace mc::files::json {

bool to_file(const case_t& cur_case, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        my_log::Logger::critical("mc::files::json::to_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    nlohmann::basic_json case_json;
    for (size_t i{0}; i < cur_case.size(); i++) {
        nlohmann::basic_json entry;
        nlohmann::basic_json annotations;
        for (size_t j{0}; j < cur_case[i]->size(); j++) {
            if (cur_case[i]->check_bit(j)) {
                nlohmann::basic_json annotation;
                annotation["id"] = j;
                annotations.push_back(annotation);
            }
        }
        entry["annotations"] = annotations;
        entry["size"]        = cur_case[i]->size();
        switch (cur_case[i]->get_type()) {
            case mc::ENTRY_TYPE::TRANSACTION: {
                auto tran     = static_cast<transaction_t*>(cur_case[i]);
                entry["type"] = "transaction";
                entry["ts1"]  = tran->ts1;
                entry["ts2"]  = tran->ts2;
                break;
            }
            case mc::ENTRY_TYPE::SET: {
                auto set         = static_cast<set_t*>(cur_case[i]);
                entry["type"]    = "set";
                entry["support"] = set->support;
                break;
            }
            case mc::ENTRY_TYPE::RULE: {
                auto rule           = static_cast<rule_t*>(cur_case[i]);
                entry["type"]       = "rule";
                entry["support"]    = rule->support;
                entry["confidence"] = rule->confidence;
                entry["target"]     = rule->target;
                break;
            }
            case mc::ENTRY_TYPE::CLUSTER: {
                auto cluster     = static_cast<cluster_t*>(cur_case[i]);
                entry["type"]    = "cluster";
                entry["cluster"] = cluster->cluster;
                break;
            }
            default:
                break;
        }
        case_json.push_back(entry);
    }
    file << case_json;
    return true;
}

bool from_file(const std::string& filename, case_t& cur_case) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        my_log::Logger::critical("mc::files::json::from_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    nlohmann::json case_json;
    file >> case_json;

    for (const auto& entry : case_json) {
        if (!entry.contains("size")) {
            continue;
        }
        Bit_mask mask(entry["size"].get<size_t>());
        if (!entry.contains("annotations")) {
            continue;
        }
        for (const auto& ann : entry["annotations"]) {
            if (!ann.contains("id")) {
                continue;
            }
            mask.set_bit(ann["id"].get<size_t>());
        }

        mc::entry_t* cur_entry;
        if (entry.contains("type")) {
            auto type_string = entry["type"].get<std::string>();
            if (type_string == "transaction") {
                auto tran = new mc::transaction_t(mask, 0, 0);
                if (entry.contains("ts1")) {
                    tran->ts1 = entry["ts1"].get<size_t>();
                }
                if (entry.contains("ts2")) {
                    tran->ts1 = entry["ts2"].get<size_t>();
                }
                cur_entry = tran;
            } else if (type_string == "set") {
                auto set = new mc::set_t(mask, 0);
                if (entry.contains("support")) {
                    set->support = entry["support"].get<double>();
                }
                cur_entry = set;
            } else if (type_string == "rule") {
                auto rule = new mc::rule_t(mask, 0, 0, 0);
                if (entry.contains("support")) {
                    rule->support = entry["support"].get<double>();
                }
                if (entry.contains("support")) {
                    rule->confidence = entry["confidence"].get<double>();
                }
                if (entry.contains("target")) {
                    rule->target = entry["target"].get<size_t>();
                }
                cur_entry = rule;
            } else if (type_string == "cluster") {
                auto cluster = new mc::cluster_t(mask, 0);
                if (entry.contains("cluster")) {
                    cluster->cluster = entry["cluster"].get<size_t>();
                }
                cur_entry = cluster;
            } else {
                cur_entry = new mc::entry_t(mask);
            }
        } else {
            cur_entry = new mc::entry_t(mask);
        }
        cur_case.push_back(cur_entry);
    }
    return true;
}

}  // namespace mc::files::json