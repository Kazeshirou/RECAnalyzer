#include "rec_template.hpp"

#include <fmt/core.h>

#include "logger.hpp"

namespace rec {

rec_template_t::rec_template_t(const etf::etf_t& etf)
        : author{etf.annotation.author},
          date{etf.annotation.date},
          format{etf.annotation.format},
          version{etf.annotation.version},
          time_units{etf.header.time_units},
          tiers(etf.tiers.size()) {
    size_t tiers_i = 0;
    for (auto& [id, etf_tier] : etf.tiers) {
        tier_t tier{id, {}, CONSTRAINT::NONE};

        auto lt = etf.linguistic_types.find(etf_tier.linguistic_type_ref);
        if (lt == etf.linguistic_types.end()) {
            my_log::Logger::debug(
                "rec::rec_template_t",
                fmt::format("Tier {} without linguistic_type", id));
        } else {
            if (lt->second.constraint_refs.size()) {
                auto constr_it =
                    etf.constraints.find(lt->second.constraint_refs[0]);
                if (constr_it != etf.constraints.end()) {
                    const auto& constr_name = constr_it->second.id;
                    if (constr_name == "Time_Subdivision") {
                        tier.constraint = CONSTRAINT::TIME_SUBDIVISION;
                    } else if (constr_name == "Symbolic_Subdivision") {
                        tier.constraint = CONSTRAINT::SYMBOLIC_SUBDIVISION;
                    } else if (constr_name == "Symbolic_Association") {
                        tier.constraint = CONSTRAINT::SYMBOLIC_ASSOCIATION;
                    } else if (constr_name == "Included_In") {
                        tier.constraint = CONSTRAINT::INCLUDED_IN;
                    }
                }
            }

            auto cv_it = etf.vocabularies.find(lt->second.vocabulary_ref);
            if (cv_it != etf.vocabularies.end()) {
                for (auto& [annotation_id, etf_annotation] :
                     cv_it->second.entries) {
                    annotation_t annotation{
                        annotation_id,
                        etf_annotation.description,
                        tiers_i,
                    };
                    annotations_map[fmt::format("{}_{}", tier.name,
                                                annotation_id)] =
                        annotations.size();
                    annotations.push_back(annotation);
                }
            }
        }

        tiers_map[id]    = tiers_i;
        tiers[tiers_i++] = tier;
    }

    for (auto& [id, etf_tier] : etf.tiers) {
        if (!etf_tier.parent_ref.size()) {
            continue;
        }

        auto tier_it = tiers_map.find(id);
        if (tier_it == tiers_map.end()) {
            my_log::Logger::warning(
                "rec::rec_template_t",
                fmt::format("When resolving tier's parent it was "
                            "unknown tier id"));
            continue;
        }

        auto parent_it = tiers_map.find(etf_tier.parent_ref);
        if (parent_it == tiers_map.end()) {
            my_log::Logger::warning(
                "rec::rec_template_t",
                fmt::format("When resolving tier's parent it was "
                            "unknown parent id"));
            continue;
        }

        tiers[tier_it->second].parent = parent_it->second;
    }
}

rec_template_t& rec_template_t::operator=(const etf::etf_t& etf) {
    *this = rec_template_t{etf};
    return *this;
}

bool rec_template_t::operator==(const rec_template_t& other) const {
    if (author != other.author) {
        return false;
    }
    if (date != other.date) {
        return false;
    }
    if (format != other.format) {
        return false;
    }
    if (version != other.version) {
        return false;
    }
    if (time_units != other.time_units) {
        return false;
    }
    if (tiers.size() != other.tiers.size()) {
        return false;
    }
    if (annotations.size() != other.annotations.size()) {
        return false;
    }
    if (tiers_map.size() != other.tiers_map.size()) {
        return false;
    }
    if (annotations_map.size() != other.annotations_map.size()) {
        return false;
    }
    for (size_t i = 0; i < tiers.size(); i++) {
        if (tiers[i].name != other.tiers[i].name) {
            return false;
        }
        if (tiers[i].constraint != other.tiers[i].constraint) {
            return false;
        }
        if (tiers[i].parent != other.tiers[i].parent) {
            return false;
        }
    }

    for (size_t i = 0; i < annotations.size(); i++) {
        if (annotations[i].value != other.annotations[i].value) {
            return false;
        }
        if (annotations[i].description != other.annotations[i].description) {
            return false;
        }
        if (annotations[i].tier != other.annotations[i].tier) {
            return false;
        }
    }

    for (const auto& [first, second] : tiers_map) {
        auto it = other.tiers_map.find(first);
        if (it == other.tiers_map.end()) {
            return false;
        }
        if (second != it->second)
            return false;
    }

    for (const auto& [first, second] : annotations_map) {
        auto it = other.annotations_map.find(first);
        if (it == other.annotations_map.end()) {
            return false;
        }
        if (second != it->second)
            return false;
    }
    return true;
}


}  // namespace rec