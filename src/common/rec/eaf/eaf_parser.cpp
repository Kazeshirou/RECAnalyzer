#include "eaf_parser.hpp"

#include <fmt/core.h>
#include <pugixml/pugixml.hpp>

#include "logger.hpp"

namespace rec::eaf {

using namespace pugi;
using namespace my_log;

std::optional<eaf_t> Parser::parse_file(const std::string& filename) {
    xml_document doc;
    auto         result = doc.load_file(filename.c_str());
    if (!result) {
        Logger::critical("rec::eaf::Parser", result.description());
        return {};
    }
    Logger::debug("rec::eaf::Parser", filename);
    auto rv = parse(doc);
    if (rv.has_value()) {
        rv.value().filename = filename;
    }
    return rv;
}

std::optional<eaf_t> Parser::parse(const xml_document& doc) {
    eaf_t eaf;
    if (!parse_annotation_document(doc, eaf)) {
    }
    return eaf;
}

bool Parser::parse_annotation_document(const ::pugi::xml_node& node,
                                       eaf_t&                  eaf) {
    auto annotation = node.child("ANNOTATION_DOCUMENT");
    if (!annotation) {
        Logger::critical("rec::eaf::Parser",
                         "Can't parse ANNOTATION_DOCUMENT tag");
        return false;
    }

    parse_annotation_document_attributes(annotation, eaf.annotation_document);

    auto header = annotation.child("HEADER");
    if (!header) {
        Logger::warning("rec::eaf::Parser", "Can't parse HEADER tag");
    } else {
        parse_header(header, eaf.header);
    }

    auto time_order = annotation.child("TIME_ORDER");
    if (!time_order) {
        Logger::critical("rec::eaf::Parser", "Can't parse TIME_ORDER tag");
        return false;
    } else {
        if (!parse_time_slots(time_order, eaf.time_slots)) {
            return false;
        }
    }

    if (!parse_tiers(annotation, eaf.tiers)) {
        return false;
    }

    parse_linguistic_types(annotation, eaf.linguistic_types);

    parse_constraints(annotation, eaf.constraints);

    parse_vocabularies(annotation, eaf.vocabularies);

    return true;
}

bool Parser::parse_header(const ::pugi::xml_node& node, header_t& header) {
    Template_parser::parse_header(node, header);

    auto media_descriptor_nodes = node.children("MEDIA_DESCRIPTOR");
    for (auto it = media_descriptor_nodes.begin();
         it != media_descriptor_nodes.end(); it++) {
        tag_t tag;
        for (auto attr_it = it->attributes_begin();
             attr_it != it->attributes_end(); attr_it++) {
            tag.attributes[attr_it->name()] = attr_it->as_string();
        }
        tag.value = it->value();
        header.media_descriptors.push_back(tag);
    }

    auto property_nodes = node.children("PROPERTY");
    for (auto it = property_nodes.begin(); it != property_nodes.end(); it++) {
        tag_t tag;
        for (auto attr_it = it->attributes_begin();
             attr_it != it->attributes_end(); attr_it++) {
            tag.attributes[attr_it->name()] = attr_it->as_string();
        }
        tag.value = it->text().as_string();
        header.properties.push_back(tag);
    }

    return true;
}


bool Parser::parse_time_slots(const ::pugi::xml_node&             node,
                              std::map<std::string, time_slot_t>& time_slots) {
    auto time_slot_nodes = node.children("TIME_SLOT");
    if (time_slot_nodes.begin() == time_slot_nodes.end()) {
        Logger::critical("rec::eaf::Parser", "Can't find TIME_SLOT tags");
        return false;
    }

    for (auto it = time_slot_nodes.begin(); it != time_slot_nodes.end(); it++) {
        time_slot_t ts;
        auto        attr = it->attribute("TIME_SLOT_ID");
        if (!attr) {
            Logger::warning("rec::eaf::Parser",
                            "Can't find TIME_SLOT_ID attribute of TIME_SLOT");
            continue;
        }
        ts.id = attr.as_string();
        attr  = it->attribute("TIME_VALUE");
        if (!attr) {
            Logger::warning("rec::eaf::Parser",
                            "Can't find TIME_VALUE attribute of TIME_SLOT");
            continue;
        }
        ts.value = attr.as_ullong();

        time_slots[ts.id] = ts;
    }

    if (!time_slots.size()) {
        Logger::critical("rec::eaf::Parser",
                         "Can't find any valid TIME_SLOT tag");
        return false;
    }

    return true;
}

bool Parser::parse_tiers(const ::pugi::xml_node&        node,
                         std::map<std::string, tier_t>& tiers) {
    auto tier_nodes = node.children("TIER");
    if (tier_nodes.begin() == tier_nodes.end()) {
        Logger::critical("rec::eaf::Parser", "Can't find TIER tags");
        return false;
    }

    for (auto it = tier_nodes.begin(); it != tier_nodes.end(); it++) {
        tier_t tier;
        if (!parse_tier(*it, tier)) {
            continue;
        }

        tiers[tier.id] = tier;
    }

    return true;
}

bool Parser::parse_tier(const ::pugi::xml_node& node, tier_t& tier) {
    if (!Template_parser::parse_tier(node, tier)) {
        return false;
    }

    auto annotation_nodes = node.children("ANNOTATION");
    if (annotation_nodes.begin() == annotation_nodes.end()) {
        Logger::debug(
            "rec::eaf::Parser",
            fmt::format("Can't find ANNOTATION child tag of TIER {}", tier.id));
        return true;
    }

    for (auto it = annotation_nodes.begin(); it != annotation_nodes.end();
         it++) {
        annotation_t annotation;
        if (!parse_annotation(*it, annotation)) {
            continue;
        }

        tier.annotations.push_back(annotation);
    }

    return true;
}

bool Parser::parse_annotation(const ::pugi::xml_node& node,
                              annotation_t&           annotation) {
    auto alignable_annotation_node = node.child("ALIGNABLE_ANNOTATION");
    if (alignable_annotation_node) {
        return parse_alignable_annotation(alignable_annotation_node,
                                          annotation);
    }

    auto ref_annotation_node = node.child("REF_ANNOTATION");
    if (ref_annotation_node) {
        return parse_ref_annotation(ref_annotation_node, annotation);
    }

    Logger::warning(
        "rec::eaf::Parser",
        "Can't find ALIGNABLE_ANNOTATION or REF_ANNOTATION of ANNOTATION");
    return false;
}


bool Parser::parse_alignable_annotation(
    const ::pugi::xml_node& alignable_annotation_node,
    annotation_t&           annotation) {
    auto attr = alignable_annotation_node.attribute("ANNOTATION_ID");
    if (!attr) {
        Logger::warning(
            "rec::eaf::Parser",
            "Can't find ANNOTATION_ID attribute of ALIGNABLE_ANNOTATION");
        return false;
    }
    annotation.id = attr.as_string();


    attr = alignable_annotation_node.attribute("TIME_SLOT_REF1");
    if (!attr) {
        Logger::warning("rec::eaf::Parser",
                        fmt::format("Can't find TIME_SLOT_REF1 attribute of "
                                    "ALIGNABLE_ANNOTATION {}",
                                    annotation.id));
    } else {
        annotation.ts1_ref = attr.as_string();
    }

    attr = alignable_annotation_node.attribute("TIME_SLOT_REF2");
    if (!attr) {
        Logger::warning("rec::eaf::Parser",
                        fmt::format("Can't find TIME_SLOT_REF2 attribute of "
                                    "ALIGNABLE_ANNOTATION {}",
                                    annotation.id));
    } else {
        annotation.ts2_ref = attr.as_string();
    }

    auto annotation_value_node =
        alignable_annotation_node.child("ANNOTATION_VALUE");
    if (!annotation_value_node) {
        Logger::warning("rec::eaf::Parser",
                        fmt::format("Can't find ANNOTATION_VALUE child node of "
                                    "ALIGNABLE_ANNOTATION {}",
                                    annotation.id));
        return false;
    }
    annotation.value = annotation_value_node.text().as_string();

    return true;
}


bool Parser::parse_ref_annotation(const ::pugi::xml_node& node,
                                  annotation_t&           annotation) {
    auto attr = node.attribute("ANNOTATION_ID");
    if (!attr) {
        Logger::warning("rec::eaf::Parser",
                        "Can't find ANNOTATION_ID attribute of REF_ANNOTATION");
        return false;
    }
    annotation.id = attr.as_string();


    attr = node.attribute("ANNOTATION_REF");
    if (!attr) {
        Logger::warning("rec::eaf::Parser",
                        fmt::format("Can't find ANNOTATION_REF child node of "
                                    "REF_ANNOTATION {}",
                                    annotation.id));
        return false;
    }
    annotation.is_ref = true;
    annotation.ref    = attr.as_string();

    auto annotation_value_node = node.child("ANNOTATION_VALUE");
    if (!annotation_value_node) {
        Logger::warning("rec::eaf::Parser",
                        fmt::format("Can't find ANNOTATION_VALUE child node of "
                                    "ALIGNABLE_ANNOTATION {}",
                                    annotation.id));
        return false;
    }
    annotation.value = annotation_value_node.text().as_string();

    return true;
}

bool Parser::parse_linguistic_types(
    const ::pugi::xml_node&                   node,
    std::map<std::string, linguistic_type_t>& linguistic_types) {
    auto ling_type = node.children("LINGUISTIC_TYPE");
    if (ling_type.begin() == ling_type.end()) {
        Logger::warning("rec::eaf::Parser", "Can't find LINGUISTIC_TYPE tags");
        return false;
    }

    for (auto it = ling_type.begin(); it != ling_type.end(); it++) {
        linguistic_type_t ling_type;
        auto              attr = it->attribute("LINGUISTIC_TYPE_ID");
        if (!attr) {
            Logger::warning(
                "rec::eaf::Parser",
                "Can't parse LINGUISTIC_TYPE_ID attribute of LINGUISTIC_TYPE");
            continue;
        } else {
            ling_type.id = attr.as_string();
        }

        if (auto it = linguistic_types.find(ling_type.id);
            it != linguistic_types.end()) {
            ling_type = it->second;
        }

        parse_linguistic_type(*it, ling_type);

        linguistic_types[ling_type.id] = ling_type;
    }

    return true;
}

bool Parser::parse_constraints(
    const ::pugi::xml_node&              node,
    std::map<std::string, constraint_t>& constraints) {
    auto constraint_nodes = node.children("CONSTRAINT");
    if (constraint_nodes.begin() == constraint_nodes.end()) {
        Logger::warning("rec::eaf::Parser", "Can't find CONSTRAINT tags");
        return false;
    }

    for (auto it = constraint_nodes.begin(); it != constraint_nodes.end();
         it++) {
        constraint_t constraint;
        if (!parse_constraint(*it, constraint)) {
            continue;
        }

        constraints[constraint.id] = constraint;
    }

    return true;
}

bool Parser::parse_vocabularies(
    const ::pugi::xml_node&              node,
    std::map<std::string, vocabulary_t>& vocabularies) {
    auto vocabulary_nodes = node.children("CONTROLLED_VOCABULARY");
    if (vocabulary_nodes.begin() == vocabulary_nodes.end()) {
        Logger::debug("rec::eaf::Parser",
                      "Can't find CONTROLLED_VOCABULARY tags");
        return false;
    }

    for (auto it = vocabulary_nodes.begin(); it != vocabulary_nodes.end();
         it++) {
        vocabulary_t vocabulary;
        if (!parse_vocabulary(*it, vocabulary)) {
            continue;
        }

        vocabularies[vocabulary.id] = vocabulary;
    }

    return true;
}

}  // namespace rec::eaf