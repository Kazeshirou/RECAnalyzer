#pragma once

#include "mc_entities.hpp"

namespace mc::files::binary {

bool to_file(const case_t& rec, const std::string& filename);
bool from_file(const std::string& filename, case_t& rec);

}  // namespace mc::files::binary