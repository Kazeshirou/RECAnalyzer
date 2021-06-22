#include "rec_template.hpp"

#include <fmt/core.h>

#include "logger.hpp"

namespace rec {

rec_template_t::rec_template_t(const etf::etf_t& etf)
        : author{etf.annotation_document.author},
          date{etf.annotation_document.date},
          format{etf.annotation_document.format},
          version{etf.annotation_document.version},
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


bool rec_template_t::reduce_by_tiers(std::vector<std::string> ignore_tiers) {
    auto rv = false;
    for (const auto& tier : ignore_tiers) {
        std::vector<std::string> ignore_annotations;
        if (!tiers_map.count(tier)) {
            my_log::Logger::warning(
                "rec::rec_template_t",
                fmt::format("Попытка удалить из шаблона неизвестный слой {}",
                            tier));
            continue;
        }

        auto tier_id = tiers_map.at(tier);
        tiers.erase(tiers.begin() + tier_id);
        tiers_map.erase(tier);
        for (auto& [first, second] : tiers_map) {
            (void)first;
            if (second > tier_id) {
                --second;
            }
        }
        for (const auto& annotation : annotations) {
            if (annotation.tier == tier_id) {
                ignore_annotations.push_back(
                    fmt::format("{}_{}", tier, annotation.value));
            }
        }
        rv |= reduce_by_annotations(ignore_annotations);
        for (auto& annotation : annotations) {
            if (annotation.tier > tier_id) {
                --annotation.tier;
            }
        }
    }
    return rv;
}

bool rec_template_t::reduce_by_annotations(
    std::vector<std::string> ignore_annotations) {
    auto rv = false;
    for (const auto& ignore : ignore_annotations) {
        rv |= reduce_by_annotation(ignore);
    }
    return rv;
}

bool rec_template_t::reduce_by_annotation(
    const std::string& ignore_annotation) {
    if (!annotations_map.count(ignore_annotation)) {
        my_log::Logger::warning(
            "rec::rec_template_t",
            fmt::format("Попытка удалить из шаблона неизвестную метку {}",
                        ignore_annotation));
        return false;
    }

    auto id = annotations_map.at(ignore_annotation);
    annotations.erase(annotations.begin() + id);
    annotations_map.erase(ignore_annotation);
    for (auto& [first, second] : annotations_map) {
        (void)first;
        if (second > id) {
            --second;
        }
    }
    return true;
}


}  // namespace rec