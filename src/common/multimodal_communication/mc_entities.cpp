#include "mc_entities.hpp"

namespace mc {

case_t::iterator case_t::find(const entry_t& value) {
    auto cur_begin = begin();
    auto cur_end   = end();
    while (true) {
        if (cur_begin == cur_end) {
            break;
        }
        auto  it      = cur_begin + std::distance(cur_begin, cur_end) / 2;
        auto& cur_val = **it;
        if (cur_val == value) {
            return it;
        }
        if (value < cur_val) {
            if (cur_end == it) {
                break;
            }
            cur_end = it;
        } else {
            if (cur_begin == it) {
                break;
            }
            cur_begin = it;
        }
    }
    return end();
}

case_t::const_iterator case_t::find(const entry_t& value) const {
    auto cur_begin = begin();
    auto cur_end   = end();
    while (true) {
        if (cur_begin == cur_end) {
            break;
        }
        auto  it      = cur_begin + std::distance(cur_begin, cur_end) / 2;
        auto& cur_val = **it;
        if (cur_val == value) {
            return it;
        }
        if (value < cur_val) {
            if (cur_end == it) {
                break;
            }
            cur_end = it;
        } else {
            if (cur_begin == it) {
                break;
            }
            cur_begin = it;
        }
    }
    return end();
}

}  // namespace mc