#pragma once

#include "mc_transaction_mining_interface.hpp"

namespace mc::transaction::algorithm {

class Window_mining : public Transaction_mining_interface {
public:
    struct window_settings_t {
        size_t size{100};
        double step_multiplier{1.};
    };

    virtual ~Window_mining() = default;

    void set_window_settings(const window_settings_t& window_settings);

    case_t run(rec::rec_entry_t& rec_entry) const override;

private:
    window_settings_t window_settings_;
};

}  // namespace mc::transaction::algorithm