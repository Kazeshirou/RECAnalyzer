#pragma once

#include <algorithm>
#include <vector>

#include "bit_mask.hpp"
#include "rec_entry.hpp"

namespace mc {

enum class ENTRY_TYPE {
    NONE        = 0,
    TRANSACTION = 1,
    SET         = 2,
    RULE        = 3,
    CLUSTER     = 4
};

struct entry_t : public Bit_mask {
    entry_t(size_t size = 1) : Bit_mask(size) {}
    entry_t(const Bit_mask& mask) : Bit_mask(mask) {}
    entry_t(Bit_mask&& mask) : Bit_mask(mask) {}
    entry_t(const entry_t&) = default;
    entry_t(entry_t&&)      = default;

    entry_t& operator=(const Bit_mask& mask) {
        Bit_mask::operator=(mask);
        return *this;
    }
    entry_t& operator=(Bit_mask&& mask) {
        Bit_mask::operator=(mask);
        return *this;
    }
    entry_t& operator=(const entry_t&) = default;
    entry_t& operator=(entry_t&&) = default;

    virtual ~entry_t() = default;

    virtual ENTRY_TYPE get_type() const {
        return ENTRY_TYPE::NONE;
    };
};

struct set_t : public entry_t {
    set_t(const Bit_mask& mask, double new_support)
            : entry_t(mask), support{new_support} {}

    ENTRY_TYPE get_type() const override {
        return type;
    }

    double                      support{0.};
    static constexpr ENTRY_TYPE type{ENTRY_TYPE::SET};
};

struct rule_t : public set_t {
    rule_t(const Bit_mask& mask, double new_support, double new_confidence,
           size_t new_target)
            : set_t(mask, new_support),
              confidence{new_confidence},
              target{new_target} {}

    ENTRY_TYPE get_type() const override {
        return type;
    }

    double                      confidence{0.};
    size_t                      target;
    static constexpr ENTRY_TYPE type{ENTRY_TYPE::RULE};
};

struct transaction_t : public entry_t {
    transaction_t(const Bit_mask& mask, rec::rec_entry_t& new_rec_entry,
                  size_t new_ts1, size_t new_ts2)
            : entry_t(mask),
              rec_entry{new_rec_entry},
              ts1{new_ts1},
              ts2{new_ts2} {}

    transaction_t(rec::rec_entry_t& new_rec_entry, size_t new_ts1,
                  size_t new_ts2)
            : entry_t(new_rec_entry.rec_template.annotations.size()),
              rec_entry{new_rec_entry},
              ts1{new_ts1},
              ts2{new_ts2} {}

    ENTRY_TYPE get_type() const override {
        return type;
    }

    rec::rec_entry_t&           rec_entry;
    size_t                      ts1;
    size_t                      ts2;
    static constexpr ENTRY_TYPE type{ENTRY_TYPE::TRANSACTION};
};

struct cluster_t : public entry_t {
    cluster_t(const Bit_mask& mask, size_t new_cluster)
            : entry_t(mask), cluster{new_cluster} {}

    ENTRY_TYPE get_type() const override {
        return type;
    }

    size_t                      cluster;
    static constexpr ENTRY_TYPE type{ENTRY_TYPE::CLUSTER};
};

struct case_t : public std::vector<entry_t*> {
    ~case_t() {
        for (auto& entry : *this) {
            delete entry;
        }
    }

    void sort() {
        std::sort(begin(), end(),
                  [](auto& one, auto& other) { return *one < *other; });
    }

    case_t::iterator       find(const entry_t& value);
    case_t::const_iterator find(const entry_t& value) const;
};

}  // namespace mc