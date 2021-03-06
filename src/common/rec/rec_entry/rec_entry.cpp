#include "rec_entry.hpp"


#include <fmt/core.h>
#include <algorithm>

#include "logger.hpp"

namespace rec {

rec_entry_t::rec_entry_t(rec_template_t& rec_template_ref)
        : rec_template{rec_template_ref} {}

rec_entry_t::rec_entry_t(rec_template_t&   rec_template_ref,
                         const eaf::eaf_t& eaf)
        : rec_template{rec_template_ref},
          author{eaf.annotation_document.author},
          date{eaf.annotation_document.date},
          format{eaf.annotation_document.format},
          version{eaf.annotation_document.version},
          time_units{eaf.header.time_units},
          filename{eaf.filename} {
    add_time_slots(eaf);

    for (auto& [id, eaf_tier] : eaf.tiers) {
        if (!rec_template.tiers_map.count(id)) {
            add_tier(eaf, id, eaf_tier);
        }
    }
    resolve_tier_parents(eaf);

    for (auto& [id, eaf_tier] : eaf.tiers) {
        size_t current_tier_i = rec_template.tiers_map[id];
        if (rec_template.tiers[current_tier_i].parent.has_value()) {
            continue;
        }

        for (auto& eaf_annotation : eaf_tier.annotations) {
            add_annotation(current_tier_i, eaf_annotation);
        }
    }


    for (auto& [id, eaf_tier] : eaf.tiers) {
        size_t current_tier_i = rec_template.tiers_map[id];
        if (!rec_template.tiers[current_tier_i].parent.has_value()) {
            continue;
        }

        for (auto& eaf_annotation : eaf_tier.annotations) {
            add_annotation(current_tier_i, eaf_annotation);
        }
    }

    std::sort(annotations.begin(), annotations.end(), compare_annotations);
}

bool rec_entry_t::operator==(const rec_entry_t& other) const {
    if (rec_template != other.rec_template) {
        return false;
    }

    if (annotations.size() != other.annotations.size()) {
        return false;
    }

    if (time_slots.size() != other.time_slots.size()) {
        return false;
    }

    if (time_slots_map.size() != other.time_slots_map.size()) {
        return false;
    }

    for (size_t i{0}; i < annotations.size(); i++) {
        if (annotations[i].annotation_id !=
            other.annotations[i].annotation_id) {
            return false;
        }
        if (annotations[i].ts1 != other.annotations[i].ts1) {
            return false;
        }
        if (annotations[i].ts2 != other.annotations[i].ts2) {
            return false;
        }
    }

    for (size_t i{0}; i < time_slots.size(); i++) {
        if (time_slots[i].id != other.time_slots[i].id) {
            return false;
        }
        if (time_slots[i].value != other.time_slots[i].value) {
            return false;
        }
    }

    for (const auto& [first, second] : time_slots_map) {
        auto it = other.time_slots_map.find(first);
        if (it == other.time_slots_map.end()) {
            return false;
        }
        if (second != it->second)
            return false;
    }

    return true;
}

void rec_entry_t::add_time_slots(const eaf::eaf_t& eaf) {
    for (auto& [id, ts] : eaf.time_slots) {
        time_slots.push_back({id, ts.value});
    }
    std::sort(time_slots.begin(), time_slots.end());
    for (size_t i{0}; i < time_slots.size(); i++) {
        time_slots_map[time_slots[i].id] = i;
    }
}

void rec_entry_t::add_tier(const eaf::eaf_t& eaf, const std::string& id,
                           const eaf::tier_t& eaf_tier) {
    size_t                 tier_i = rec_template.tiers.size();
    rec_template_t::tier_t tier{id, {}, CONSTRAINT::NONE};
    auto lt = eaf.linguistic_types.find(eaf_tier.linguistic_type_ref);
    if (lt == eaf.linguistic_types.end()) {
        my_log::Logger::debug(
            "rec::rec_entry_t",
            fmt::format("{}: Tier {} without linguistic_type", filename, id));
        return;
    }

    if (lt->second.constraint_refs.size()) {
        auto constr_it = eaf.constraints.find(lt->second.constraint_refs[0]);
        if (constr_it != eaf.constraints.end()) {
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

    auto cv_it = eaf.vocabularies.find(lt->second.vocabulary_ref);
    if (cv_it != eaf.vocabularies.end()) {
        for (auto& [annotation_id, etf_annotation] : cv_it->second.entries) {
            rec_template_t::annotation_t annotation{
                annotation_id,
                etf_annotation.description,
                tier_i,
            };
            rec_template.annotations_map[fmt::format("{}_{}", tier.name,
                                                     annotation_id)] =
                rec_template.annotations.size();
            rec_template.annotations.push_back(annotation);
        }
    }


    rec_template.tiers_map[id] = tier_i;
    rec_template.tiers.push_back(tier);
}

void rec_entry_t::add_annotation(size_t                   current_tier_i,
                                 const eaf::annotation_t& eaf_annotation) {
    auto& current_tier = rec_template.tiers[current_tier_i];

    std::string annotation_uniq_name =
        fmt::format("{}_{}", current_tier.name, eaf_annotation.value);
    if (!rec_template.annotations_map.count(annotation_uniq_name)) {
        rec_template_t::annotation_t new_annotation{
            eaf_annotation.value, "не в словаре", current_tier_i};
        rec_template.annotations_map[annotation_uniq_name] =
            rec_template.annotations.size();
        rec_template.annotations.push_back(new_annotation);
    }

    annotation_t annotation{rec_template.annotations_map[annotation_uniq_name],
                            0, 0};
    if (!eaf_annotation.is_ref) {
        if (!time_slots_map.count(eaf_annotation.ts1_ref)) {
            my_log::Logger::debug(
                "rec::rec_entry_t",
                fmt::format(
                    "{}: Annotation {} on tier {} without time_slot1_ref",
                    filename, eaf_annotation.value, current_tier.name));
            return;
        }

        if (!time_slots_map.count(eaf_annotation.ts2_ref)) {
            my_log::Logger::debug(
                "rec::rec_entry_t",
                fmt::format(
                    "{}: Annotation {} on tier {} without time_slot2_ref",
                    filename, eaf_annotation.value, current_tier.name));
            return;
        }

        annotation.ts1 = time_slots_map[eaf_annotation.ts1_ref];
        annotation.ts2 = time_slots_map[eaf_annotation.ts2_ref];
    } else {
        if (!annotation_id_map.count(eaf_annotation.ref)) {
            my_log::Logger::debug(
                "rec::rec_entry_t",
                fmt::format("{}:Ref annotation {} on tier {} refer to unknown "
                            "annotation {}",
                            filename, eaf_annotation.value, current_tier.name,
                            eaf_annotation.ref));
            return;
        }

        auto& ref_annotation =
            annotations[annotation_id_map[eaf_annotation.ref]];
        annotation.ts1 = ref_annotation.ts1;
        annotation.ts2 = ref_annotation.ts2;

        if (!current_tier.parent.has_value()) {
            my_log::Logger::debug(
                "rec::rec_entry_t",
                fmt::format(
                    "{}: Ref annotation {} on tier {} without parent tier",
                    filename, eaf_annotation.value, current_tier.name));
        } else if (current_tier.parent.value() !=
                   rec_template.annotations[ref_annotation.annotation_id]
                       .tier) {
            my_log::Logger::debug(
                "rec::rec_entry_t",
                fmt::format(
                    "{}: Ref annotation {} on tier {} parent tier {} not "
                    "eq tier of refered annotation {}",
                    filename,
                    rec_template.annotations[annotation.annotation_id].value,
                    rec_template
                        .tiers[rec_template
                                   .annotations[annotation.annotation_id]
                                   .tier]
                        .name,
                    rec_template.tiers[current_tier.parent.value()].name,
                    rec_template
                        .tiers[rec_template
                                   .annotations[ref_annotation.annotation_id]
                                   .tier]
                        .name));
        }
    }
    annotation_id_map[eaf_annotation.id] = annotations.size();
    annotations.push_back(annotation);
}

void rec_entry_t::resolve_tier_parents(const eaf::eaf_t& eaf) {
    for (auto& [id, eaf_tier] : eaf.tiers) {
        if (!eaf_tier.parent_ref.size()) {
            continue;
        }

        auto tier_it = rec_template.tiers_map.find(id);
        if (tier_it == rec_template.tiers_map.end()) {
            my_log::Logger::warning(
                "rec::rec_entry_t",
                fmt::format("{}: When resolving tier's parent it was "
                            "unknown tier id {}",
                            filename, id));
            continue;
        }

        auto& tier = rec_template.tiers[tier_it->second];
        if (tier.parent.has_value() &&
            (rec_template.tiers[tier.parent.value()].name !=
             eaf_tier.parent_ref)) {
            my_log::Logger::warning(
                "rec::rec_entry_t",
                fmt::format(
                    "{}: When resolving tier's parent tier {} has already "
                    "parent {} not eq new {}",
                    filename, tier.name,
                    rec_template.tiers[tier.parent.value()].name,
                    eaf_tier.parent_ref));
            continue;
        }
        auto parent_it = rec_template.tiers_map.find(eaf_tier.parent_ref);
        if (parent_it == rec_template.tiers_map.end()) {
            my_log::Logger::warning(
                "rec::rec_entry_t",
                fmt::format("{}: When resolving tier's parent tier {} was "
                            "unknown parent id {}",
                            filename, tier.name, eaf_tier.parent_ref));
            continue;
        }

        rec_template.tiers[tier_it->second].parent = parent_it->second;
    }
}


bool rec_entry_t::reduce_by_tiers(std::vector<std::string> ignore_tiers) {
    auto rv = false;
    for (const auto& tier : ignore_tiers) {
        std::vector<std::string> ignore_annotations;
        if (!rec_template.tiers_map.count(tier)) {
            my_log::Logger::warning(
                "rec::rec_template_t",
                fmt::format("Попытка удалить из разметки события с "
                            "неизвестного слоя {}",
                            tier));
            continue;
        }

        auto tier_id = rec_template.tiers_map.at(tier);
        for (const auto& annotation : rec_template.annotations) {
            if (annotation.tier == tier_id) {
                ignore_annotations.push_back(
                    fmt::format("{}_{}", tier, annotation.value));
            }
        }
        rv |= reduce_by_annotations(ignore_annotations);
    }
    return rv;
}


bool rec_entry_t::reduce_by_annotations(
    std::vector<std::string> ignore_annotations) {
    bool rv = false;
    for (const auto& annotation : ignore_annotations) {
        rv |= reduce_by_annotation(annotation);
    }
    return rv;
}


bool rec_entry_t::reduce_by_annotation(const std::string& ignore_annotation) {
    if (!rec_template.annotations_map.count(ignore_annotation)) {
        my_log::Logger::warning(
            "rec::rec_template_t",
            fmt::format(
                "Попытка удалить из разметки события с неизвестной меткой {}",
                ignore_annotation));
        return false;
    }

    size_t old_size = annotations.size();
    size_t id       = rec_template.annotations_map.at(ignore_annotation);
    for (auto it = annotations.begin(); it != annotations.end(); it++) {
        if ((*it).annotation_id == id) {
            annotations.erase(it--);
        }
    }
    return old_size != annotations.size();
}

}  // namespace rec