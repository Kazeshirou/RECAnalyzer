#pragma once

#include "mc_entities.hpp"

namespace mc::files::csv {

bool to_file(const case_t& cur_case, const std::string& filename);

}  // namespace mc::files::csv