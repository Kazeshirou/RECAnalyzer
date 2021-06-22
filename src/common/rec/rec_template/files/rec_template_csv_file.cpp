#include "rec_template_csv_file.hpp"

#include <fmt/format.h>
#include <fstream>

#include "logger.hpp"

namespace rec::files::csv {

bool to_file(const rec_template_t& rec, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        my_log::Logger::critical("mc::files::csv::to_file",
                                 fmt::format("Can't open file {}", filename));
        return false;
    }

    for (size_t i{0}; i < rec.annotations.size(); i++) {
        file << fmt::format("{}:{} | ", rec.tiers[rec.annotations[i].tier].name,
                            rec.annotations[i].value);
    }
    file << std::endl;

    return true;
}

}  // namespace rec::files::csv
