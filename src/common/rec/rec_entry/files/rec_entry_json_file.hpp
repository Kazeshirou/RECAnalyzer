#pragma once

#include "rec_entry.hpp"

namespace rec::files::json {

bool to_file(const rec_entry_t& rec, const std::string& filename);
bool from_file(const std::string& filename, rec_entry_t& rec);

}  // namespace rec::files::json