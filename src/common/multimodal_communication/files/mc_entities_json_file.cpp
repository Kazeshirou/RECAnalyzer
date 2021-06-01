#include "mc_entities_json_file.hpp"

namespace mc::files::json {

bool to_file([[maybe_unused]] const case_t&      rec,
             [[maybe_unused]] const std::string& filename) {
    return false;
}

bool from_file([[maybe_unused]] const std::string& filename,
               [[maybe_unused]] case_t&            rec) {
    return false;
}

}  // namespace mc::files::json