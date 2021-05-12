#include "rec_entry_json_file.hpp"

#include <fmt/core.h>
#include <fstream>
#include <nlohmann/nlohmann.hpp>

#include "logger.hpp"

namespace rec::files::json {

bool to_file(const rec_entry_t& rec, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        my_log::Logger::critical("rec::files::json::to_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    nlohmann::basic_json rec_entry;
    rec_entry["author"]     = rec.author;
    rec_entry["date"]       = rec.date;
    rec_entry["format"]     = rec.format;
    rec_entry["version"]    = rec.version;
    rec_entry["time_units"] = rec.time_units;

    nlohmann::basic_json time_slots;
    for (size_t i = 0; i < rec.time_slots.size(); i++) {
        nlohmann::basic_json ts;
        ts["id"]    = i;
        ts["name"]  = rec.time_slots[i].id;
        ts["value"] = rec.time_slots[i].value;
        time_slots.push_back(ts);
    }
    rec_entry["time_slots"] = time_slots;

    nlohmann::basic_json events;
    for (size_t i = 0; i < rec.annotations.size(); i++) {
        nlohmann::basic_json event;
        event["id"] = i;
        auto& annotation =
            rec.rec_template.annotations[rec.annotations[i].annotation_id];
        event["annotation"] = annotation.value;
        event["tier"]       = rec.rec_template.tiers[annotation.tier].name;
        event["ts1"]        = rec.time_slots[rec.annotations[i].ts1].id;
        event["ts2"]        = rec.time_slots[rec.annotations[i].ts2].id;
        event["ts1_value"]  = rec.time_slots[rec.annotations[i].ts1].value;
        event["ts2_value"]  = rec.time_slots[rec.annotations[i].ts2].value;

        events.push_back(event);
    }
    rec_entry["events"] = events;

    file << rec_entry;
    return true;
}

bool from_file(const std::string& filename, rec_entry_t& rec) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        my_log::Logger::critical("rec::files::json::from_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    nlohmann::json rec_entry;
    file >> rec_entry;

    if (rec_entry.contains("author")) {
        rec.author = rec_entry["author"];
    }

    if (rec_entry.contains("date")) {
        rec.date = rec_entry["date"];
    }

    if (rec_entry.contains("format")) {
        rec.format = rec_entry["format"];
    }

    if (rec_entry.contains("version")) {
        rec.version = rec_entry["version"];
    }

    if (rec_entry.contains("time_units")) {
        rec.time_units = rec_entry["time_units"];
    }

    if (!rec_entry.contains("time_slots")) {
        return false;
    }

    for (const auto& ts : rec_entry["time_slots"]) {
        if (!ts.contains("id")) {
            my_log::Logger::critical("rec::files::json::from_file",
                                     "time_slot doesn't contain id field");
            continue;
        }

        size_t id = ts["id"];

        if (!ts.contains("value")) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Time_slot with id {} doesn't contain value field",
                            id));
            continue;
        }

        if (!ts.contains("name")) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Time_slot with id {} doesn't contain name field",
                            id));
            continue;
        }

        if (id >= rec.time_slots.size()) {
            rec.time_slots.resize(id + 1);
        }

        rec.time_slots[id].value = ts["value"];
        rec.time_slots[id].id    = ts["name"];

        rec.time_slots_map[rec.time_slots[id].id] = id;
    }

    for (size_t i{0}; i < rec.time_slots.size(); i++) {
        rec.time_slots_map[rec.time_slots[i].id] = i;
    }

    if (!rec_entry.contains("events")) {
        return false;
    }

    for (const auto& event : rec_entry["events"]) {
        if (!event.contains("id")) {
            my_log::Logger::critical("rec::files::json::from_file",
                                     "Event doesn't contain id field");
            continue;
        }

        size_t id = event["id"];

        if (!event.contains("annotation")) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Event with id {} doesn't contain annotation field",
                            id));
            continue;
        }

        if (!event.contains("tier")) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Event with id {} doesn't contain tier field", id));
            continue;
        }

        if (!event.contains("ts1")) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Event with id {} doesn't contain ts1 field", id));
            continue;
        }

        if (!event.contains("ts2")) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Event with id {} doesn't contain ts2 field", id));
            continue;
        }


        std::string annotation_id =
            fmt::format("{}_{}", event["tier"], event["annotation"]);
        if (!rec.rec_template.annotations_map.count(annotation_id)) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format(
                    "Event with id {} has unknown annotation unique name {}",
                    id, annotation_id));
            continue;
        }
        auto& ts1 = event["ts1"];
        if (!rec.time_slots_map.count(ts1)) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Event with id {} has unknown ts1 {}", id, ts1));
            continue;
        }
        auto& ts2 = event["ts2"];
        if (!rec.time_slots_map.count(ts2)) {
            my_log::Logger::critical(
                "rec::files::json::from_file",
                fmt::format("Event with id {} has unknown ts2 {}", id, ts2));
            continue;
        }

        if (id >= rec.annotations.size()) {
            rec.annotations.resize(id + 1);
        }

        rec.annotations[id].annotation_id =
            rec.rec_template.annotations_map[annotation_id];
        rec.annotations[id].ts1 = rec.time_slots_map[ts1];
        rec.annotations[id].ts2 = rec.time_slots_map[ts2];
    }

    return true;
}

}  // namespace rec::files::json