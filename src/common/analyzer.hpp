#pragma once

#include <functional>
#include <nlohmann/nlohmann.hpp>
#include <string>

#include "mc_entities.hpp"
#include "rec_entry.hpp"
#include "rec_template.hpp"

enum class FILE_EXTENSION { UNKNOWN = 0, ETF = 1, EAF = 2, JSON = 3, CSV = 4 };

class Analyzer {
public:
    bool run(const std::string& cfg_filename);
    void set_progress_update_handler(
        const std::function<void(double)>& progress_update_handler) {
        progress_update_handler_ = progress_update_handler;
    }

    bool process_rec_template(const std::string& filename);
    bool process_converts(const nlohmann::json& convert_cfg);
    bool process_transactions(const nlohmann::json& transactions_cfg);
    bool process_sets(const nlohmann::json& sets_cfg);
    bool process_rules(const nlohmann::json& rules_cfg);
    bool process_clusterings(const nlohmann::json& clustering_cfg);

    static FILE_EXTENSION file_extension(const std::string& filename);

    std::optional<rec::rec_template_t> read_rec_template(
        const std::string& filename);
    bool write_rec_template(const rec::rec_template_t& rec_template,
                            const std::string&         filename);

    std::optional<rec::rec_entry_t> read_rec_entry(const std::string& filename);
    bool write_rec_entry(const rec::rec_entry_t& rec_entry,
                         const std::string&      filename);

    std::optional<mc::case_t> read_case(const std::string& filename);
    bool write_case(const mc::case_t& clusters, const std::string& filename);

protected:
    bool process_convert(const nlohmann::json& convert_cfg);
    bool process_transaction(const nlohmann::json& transactions_cfg);
    bool process_set(const nlohmann::json& sets_cfg);
    bool process_rule(const nlohmann::json& rules_cfg);
    bool process_clustering(const nlohmann::json& clustering_cfg);

    void update_progress(double current_progress) {
        if (progress_update_handler_.has_value()) {
            progress_update_handler_.value()(current_progress);
        }
    }

private:
    std::optional<std::function<void(double)>> progress_update_handler_;
    std::string                                analisys_folder_;
    rec::rec_template_t                        rec_template_;
};