#pragma once

#include "rec_template.hpp"

namespace rec::files::binary {

bool to_file(const rec_template_t& rec, const std::string& filename);
bool from_file(const std::string& filename, rec_template_t& rec);

}  // namespace rec::files::binary