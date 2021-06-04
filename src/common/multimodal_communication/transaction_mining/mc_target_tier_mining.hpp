#pragma once

#include "mc_transaction_mining_interface.hpp"

namespace mc::transaction::algorithm {

class Target_tier_mining : public Transaction_mining_interface {
public:
    struct tier_settings_t {
        std::string target_tier{""};
        bool        ignore_intervals_without_target_tier{false};
    };

    virtual ~Target_tier_mining() = default;

    void set_tier_settings(const tier_settings_t& tier_settings);

    case_t run(rec::rec_entry_t& rec_entry) const override;

private:
    tier_settings_t tier_settings_;
};

}  // namespace mc::transaction::algorithm