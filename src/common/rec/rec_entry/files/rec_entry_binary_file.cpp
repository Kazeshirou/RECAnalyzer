#include "rec_entry_binary_file.hpp"

#include <fmt/core.h>
#include <fstream>

#include "logger.hpp"

namespace rec::files::binary {

bool to_file(const rec_entry_t& rec, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        my_log::Logger::critical("rec::files::binary::to_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    auto write_string = [&](const std::string& string) {
        char buffer[256];
        if (string.size() > 255) {
            *reinterpret_cast<uint8_t*>(buffer) = 255;
            memcpy(buffer + 1, string.c_str(), 255);
        } else {
            buffer[0] = string.size();
            memcpy(buffer + 1, string.c_str(), string.size());
        }
        file.write(buffer, string.size() + 1);
    };

    auto write_number = [&](auto number) {
        file.write(reinterpret_cast<const char*>(&number), sizeof(number));
    };

    write_string(rec.author);
    write_string(rec.date);
    write_string(rec.format);
    write_string(rec.version);
    write_string(rec.time_units);
    write_number(rec.time_slots.size());
    for (const auto& ts : rec.time_slots) {
        write_string(ts.id);
        write_number(ts.value);
    }
    write_number(rec.annotations.size());
    for (const auto& annotation : rec.annotations) {
        write_number(annotation.annotation_id);
        write_number(annotation.ts1);
        write_number(annotation.ts2);
    }

    return true;
}

bool from_file(const std::string& filename, rec_entry_t& rec) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        my_log::Logger::critical("rec::files::binary::from_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    auto read_string = [&](std::string& string) {
        char buffer[257] = {0};
        file.read(buffer, 1);
        file.read(buffer + 1, *reinterpret_cast<uint8_t*>(buffer));
        string = std::string(buffer + 1);
    };

    auto read_number = [&](auto& number) {
        file.read(reinterpret_cast<char*>(&number), sizeof(number));
    };

    read_string(rec.author);
    read_string(rec.date);
    read_string(rec.format);
    read_string(rec.version);
    read_string(rec.time_units);
    size_t size;
    read_number(size);
    rec.time_slots.resize(size);
    for (size_t i = 0; i < rec.time_slots.size(); i++) {
        read_string(rec.time_slots[i].id);
        read_number(size);
        if (size != size_t(-1)) {
            rec.time_slots[i].value = size;
        }

        rec.time_slots_map[rec.time_slots[i].id] = i;
    }
    read_number(size);
    rec.annotations.resize(size);
    for (size_t i = 0; i < rec.annotations.size(); i++) {
        read_number(rec.annotations[i].annotation_id);
        read_number(rec.annotations[i].ts1);
        read_number(rec.annotations[i].ts2);
    }

    return true;
}

}  // namespace rec::files::binary