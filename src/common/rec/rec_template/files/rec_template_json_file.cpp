#include "rec_template_json_file.hpp"

#include <fmt/core.h>
#include <fstream>
#include <nlohmann/nlohmann.hpp>

#include "logger.hpp"

namespace rec::files::json {

bool to_file(const rec_template_t& rec, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        my_log::Logger::critical("rec::files::json::to_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    nlohmann::basic_json rec_template;
    rec_template["author"]     = rec.author;
    rec_template["date"]       = rec.date;
    rec_template["format"]     = rec.format;
    rec_template["version"]    = rec.version;
    rec_template["time_units"] = rec.time_units;

    nlohmann::basic_json tiers;
    for (size_t i = 0; i < rec.tiers.size(); i++) {
        nlohmann::basic_json tier;
        tier["id"]   = i;
        tier["name"] = rec.tiers[i].name;
        if (rec.tiers[i].parent.has_value()) {
            tier["parent"]      = rec.tiers[i].parent.value();
            tier["parent_name"] = rec.tiers[rec.tiers[i].parent.value()].name;
        }
        switch (rec.tiers[i].constraint) {
            case CONSTRAINT::TIME_SUBDIVISION:
                tier["constraint"] = "Time_Subdivision";
                break;
            case CONSTRAINT::SYMBOLIC_SUBDIVISION:
                tier["constraint"] = "Symbolic_Subdivision";
                break;
            case CONSTRAINT::SYMBOLIC_ASSOCIATION:
                tier["constraint"] = "Symbolic_Association";
                break;
            case CONSTRAINT::INCLUDED_IN:
                tier["constraint"] = "Included_In";
                break;
            default:
                break;
        }
        tiers.push_back(tier);
    }
    rec_template["tiers"] = tiers;

    nlohmann::basic_json annotations;
    for (size_t i = 0; i < rec.annotations.size(); i++) {
        nlohmann::basic_json annotation;
        annotation["id"]          = i;
        annotation["value"]       = rec.annotations[i].value;
        annotation["description"] = rec.annotations[i].description;
        annotation["tier"]        = rec.annotations[i].tier;
        annotation["tier_name"]   = rec.tiers[rec.annotations[i].tier].name;

        annotations.push_back(annotation);
    }
    rec_template["annotations"] = annotations;

    file << rec_template;
    return true;
}

bool from_file(const std::string& filename, rec_template_t& rec) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        my_log::Logger::critical("rec::files::json::from_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    nlohmann::json rec_template;
    file >> rec_template;

    if (rec_template.contains("author")) {
        rec.author = rec_template["author"];
    }

    if (rec_template.contains("date")) {
        rec.date = rec_template["date"];
    }

    if (rec_template.contains("format")) {
        rec.format = rec_template["format"];
    }

    if (rec_template.contains("version")) {
        rec.version = rec_template["version"];
    }

    if (rec_template.contains("time_units")) {
        rec.time_units = rec_template["time_units"];
    }

    if (!rec_template.contains("tiers")) {
        return false;
    }

    for (const auto& tier : rec_template["tiers"]) {
        if (!tier.contains("id")) {
            my_log::Logger::critical("rec::files::json::from_file",
                                     "Tier doesn't contain id field");
            continue;
        }

        size_t id = tier["id"];

        if (!tier.contains("name")) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Tier with id {} doesn't contain name field", id));
            continue;
        }

        if (id >= rec.tiers.size()) {
            rec.tiers.resize(id + 1);
        }

        rec.tiers[id].name = tier["name"];

        if (tier.contains("parent")) {
            rec.tiers[id].parent = tier["parent"];
        }

        rec.tiers[id].constraint = CONSTRAINT::NONE;
        if (tier.contains("constraint")) {
            std::string constraint = tier["constraint"];

            if (constraint == "Time_Subdivision") {
                rec.tiers[id].constraint = CONSTRAINT::TIME_SUBDIVISION;
            } else if (constraint == "Symbolic_Subdivision") {
                rec.tiers[id].constraint = CONSTRAINT::SYMBOLIC_SUBDIVISION;
            } else if (constraint == "Symbolic_Association") {
                rec.tiers[id].constraint = CONSTRAINT::SYMBOLIC_ASSOCIATION;
            } else if (constraint == "Included_In") {
                rec.tiers[id].constraint = CONSTRAINT::INCLUDED_IN;
            }
        }

        rec.tiers_map[rec.tiers[id].name] = id;
    }

    if (!rec_template.contains("annotations")) {
        return false;
    }

    for (const auto& annotation : rec_template["annotations"]) {
        if (!annotation.contains("id")) {
            my_log::Logger::critical("rec::files::json::from_file",
                                     "Annotation doesn't contain id field");
            continue;
        }

        size_t id = annotation["id"];

        if (!annotation.contains("value")) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Annotation with id {} doesn't contain value field",
                            id));
            continue;
        }

        if (!annotation.contains("tier")) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Annotation with id {} doesn't contain tier field",
                            id));
            continue;
        }


        if (id >= rec.annotations.size()) {
            rec.annotations.resize(id + 1);
        }

        rec.annotations[id].value = annotation["value"];
        rec.annotations[id].tier  = annotation["tier"];

        if (annotation.contains("description")) {
            rec.annotations[id].description = annotation["description"];
        }
        rec.annotations_map[fmt::format(
            "{}_{}", rec.tiers[rec.annotations[id].tier].name,
            rec.annotations[id].value)] = id;
    }

    return true;
}

}  // namespace rec::files::json