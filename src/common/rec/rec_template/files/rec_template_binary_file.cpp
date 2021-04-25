#include "rec_template_binary_file.hpp"

#include <fmt/core.h>
#include <fstream>

#include "logger.hpp"

namespace rec::files::binary {

bool to_file(const rec_template_t& rec, const std::string& filename) {
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
    write_number(rec.tiers.size());
    for (const auto& tier : rec.tiers) {
        write_string(tier.name);
        if (tier.parent.has_value()) {
            write_number(tier.parent.value());
        } else {
            write_number(size_t(-1));
        }
        write_number(tier.constraint);
    }
    write_number(rec.annotations.size());
    for (const auto& annotation : rec.annotations) {
        write_string(annotation.value);
        write_string(annotation.description);
        write_number(annotation.tier);
    }

    return true;
}

bool from_file(const std::string& filename, rec_template_t& rec) {
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
    rec.tiers.resize(size);
    for (size_t i = 0; i < rec.tiers.size(); i++) {
        read_string(rec.tiers[i].name);
        read_number(size);
        if (size != size_t(-1)) {
            rec.tiers[i].parent = size;
        }
        read_number(rec.tiers[i].constraint);

        rec.tiers_map[rec.tiers[i].name] = i;
    }
    read_number(size);
    rec.annotations.resize(size);
    for (size_t i = 0; i < rec.annotations.size(); i++) {
        read_string(rec.annotations[i].value);
        read_string(rec.annotations[i].description);
        read_number(rec.annotations[i].tier);

        rec.annotations_map[fmt::format("{}_{}",
                                        rec.tiers[rec.annotations[i].tier].name,
                                        rec.annotations[i].value)] = i;
    }

    return true;
}

}  // namespace rec::files::binary