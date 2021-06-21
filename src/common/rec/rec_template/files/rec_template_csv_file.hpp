#pragma once

#include "rec_template.hpp"

namespace rec::files::csv {

bool to_file(const rec_template_t& rec, const std::string& filename);

}  // namespace rec::files::csv