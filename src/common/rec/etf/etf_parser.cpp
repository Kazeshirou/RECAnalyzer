#include "etf_parser.hpp"

#include <fmt/core.h>
#include <pugixml/pugixml.hpp>

#include "logger.hpp"

namespace rec::etf {

using namespace pugi;
using namespace my_log;

std::optional<etf_t> Parser::parse_file(const std::string& filename) {
    xml_document doc;
    auto         result = doc.load_file(filename.c_str());
    if (!result) {
        Logger::critical("rec::etf::Parser", result.description());
        return {};
    }
    return parse(doc);
}

std::optional<etf_t> Parser::parse(const xml_document& doc) {
    etf_t etf;
    if (!parse_annotation_document(doc, etf)) {
    }
    return etf;
}

bool Parser::parse_annotation_document(const ::pugi::xml_node& node,
                                       etf_t&                  etf) {
    auto annotation = node.child("ANNOTATION_DOCUMENT");
    if (!annotation) {
        Logger::critical("rec::etf::Parser",
                         "Can't parse ANNOTATION_DOCUMENT tag");
        return false;
    }

    parse_annotation_document_attributes(annotation, etf.annotation_document);

    auto header = annotation.child("HEADER");
    if (!header) {
        Logger::warning("rec::etf::Parser", "Can't parse HEADER tag");
    }
    parse_header(header, etf.header);


    if (!parse_tiers(annotation, etf.tiers)) {
        return false;
    }

    parse_linguistic_types(annotation, etf.linguistic_types);

    parse_constraints(annotation, etf.constraints);

    parse_vocabularies(annotation, etf.vocabularies);

    return true;
}

bool Parser::parse_annotation_document_attributes(
    const ::pugi::xml_node& node, annotation_document_t& annotation_document) {
    auto attr = node.attribute("AUTHOR");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse AUTHOR attribute of ANNOTATION_DOCUMENT");
    } else {
        annotation_document.author = attr.as_string();
    }

    attr = node.attribute("DATE");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse DATE attribute of ANNOTATION_DOCUMENT");
    } else {
        annotation_document.date = attr.as_string();
    }

    attr = node.attribute("FORMAT");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse FORMAT attribute of ANNOTATION_DOCUMENT");
    } else {
        annotation_document.format = attr.as_string();
    }

    attr = node.attribute("VERSION");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse VERSION attribute of ANNOTATION_DOCUMENT");
    } else {
        annotation_document.version = attr.as_string();
    }

    return true;
}

bool Parser::parse_header(const ::pugi::xml_node& node, header_t& header) {
    auto attr = node.attribute("MEDIA_FILE");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse MEDIA_FILE attribute of HEADER");
    } else {
        header.media_file = attr.as_string();
    }

    attr = node.attribute("TIME_UNITS");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse MEDIA_FILE attribute of HEADER");
    } else {
        header.time_units = attr.as_string();
    }

    return true;
}

bool Parser::parse_tiers(const ::pugi::xml_node&        node,
                         std::map<std::string, tier_t>& tiers) {
    auto tier_nodes = node.children("TIER");
    if (tier_nodes.begin() == tier_nodes.end()) {
        Logger::critical("rec::etf::Parser", "Can't find TIER tags");
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
    auto attr = node.attribute("TIER_ID");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse TIER_ID attribute of TIER");
        return false;
    } else {
        tier.id = attr.as_string();
    }

    attr = node.attribute("PARENT_REF");
    if (attr) {
        tier.parent_ref = attr.as_string();
    }

    attr = node.attribute("LINGUISTIC_TYPE_REF");
    if (!attr) {
        Logger::warning(
            "rec::etf::Parser",
            fmt::format("Can't parse LINGUISTIC_TYPE_REF attribute of {}",
                        tier.id));
    } else {
        tier.linguistic_type_ref = attr.as_string();
    }

    return true;
}

bool Parser::parse_linguistic_types(
    const ::pugi::xml_node&                   node,
    std::map<std::string, linguistic_type_t>& linguistic_types) {
    auto ling_type = node.children("LINGUISTIC_TYPE");
    if (ling_type.begin() == ling_type.end()) {
        Logger::warning("rec::etf::Parser", "Can't find LINGUISTIC_TYPE tags");
        return false;
    }

    for (auto it = ling_type.begin(); it != ling_type.end(); it++) {
        linguistic_type_t ling_type;
        auto              attr = it->attribute("LINGUISTIC_TYPE_ID");
        if (!attr) {
            Logger::warning(
                "rec::etf::Parser",
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

bool Parser::parse_linguistic_type(const ::pugi::xml_node& node,
                                   linguistic_type_t&      linguistic_type) {
    auto attr = node.attribute("CONSTRAINTS");
    if (attr) {
        linguistic_type.constraint_refs.push_back(attr.as_string());
    }

    attr = node.attribute("CONTROLLED_VOCABULARY_REF");
    if (attr) {
        if (linguistic_type.vocabulary_ref.length()) {
            Logger::warning(
                "rec::etf::Parser",
                fmt::format("CONTROLLED_VOCABULARY_REF has already been "
                            "parsed for LINGUISTIC_TYPE {}",
                            linguistic_type.id));
            Logger::warning(
                "rec::etf::Parser",
                fmt::format("Old CONTROLLED_VOCABULARY_REF will be droped ({})",
                            linguistic_type.vocabulary_ref));
        }
        linguistic_type.vocabulary_ref = attr.as_string();
    }
    return true;
}

bool Parser::parse_constraints(
    const ::pugi::xml_node&              node,
    std::map<std::string, constraint_t>& constraints) {
    auto constraint_nodes = node.children("CONSTRAINT");
    if (constraint_nodes.begin() == constraint_nodes.end()) {
        Logger::warning("rec::etf::Parser", "Can't find CONSTRAINT tags");
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

bool Parser::parse_constraint(const ::pugi::xml_node& node,
                              constraint_t&           constraint) {
    auto attr = node.attribute("STEREOTYPE");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse STEREOTYPE attribute of CONSTRAINT");
        return false;
    } else {
        constraint.id = attr.as_string();
    }

    attr = node.attribute("DESCRIPTION");
    if (!attr) {
        Logger::debug("rec::etf::Parser",
                      fmt::format("Can't parse DESCRIPTION attribute of {}",
                                  constraint.id));
    } else {
        constraint.description = attr.as_string();
    }
    return true;
}

bool Parser::parse_vocabularies(
    const ::pugi::xml_node&              node,
    std::map<std::string, vocabulary_t>& vocabularies) {
    auto vocabulary_nodes = node.children("CONTROLLED_VOCABULARY");
    if (vocabulary_nodes.begin() == vocabulary_nodes.end()) {
        Logger::warning("rec::etf::Parser", "Can't find CONSTRAINT tags");
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

bool Parser::parse_vocabulary(const ::pugi::xml_node& node,
                              vocabulary_t&           vocabulary) {
    auto attr = node.attribute("CV_ID");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse CV_ID attribute of CONTROLLED_VOCABULARY");
        return false;
    } else {
        vocabulary.id = attr.as_string();
    }

    auto description = node.child("DESCRIPTION");
    if (!description) {
        auto description_attr = node.attribute("DESCRIPTION");
        if (!description_attr) {
            Logger::debug(
                "rec::etf::Parser",
                fmt::format(
                    "Can't parse DESCRIPTION child node or attribute of "
                    "CONTROLLED_VOCABULARY {}",
                    vocabulary.id));
        } else {
            vocabulary.description = description_attr.as_string();
        }
    } else {
        vocabulary.description = description.text().as_string();
    }

    auto vocabulary_entries = node.children("CV_ENTRY_ML");
    for (auto ve_it = vocabulary_entries.begin();
         ve_it != vocabulary_entries.end(); ve_it++) {
        vocabulary_entry_t ve;
        auto               ve_value = ve_it->child("CVE_VALUE");
        if (!ve_value) {
            Logger::warning(
                "rec::etf::Parser",
                fmt::format("Can't parse CVE_VALUE child node of "
                            "CV_ENTRY_ML of CONTROLLED_VOCABULARY {}",
                            vocabulary.id));
            return false;
        }

        ve.id = ve_value.text().as_string();
        if (!ve.id.length()) {
            Logger::warning("rec::etf::Parser",
                            fmt::format("Can't parse value of "
                                        "CVE_VALUE of CONTROLLED_VOCABULARY {}",
                                        vocabulary.id));
            continue;
        }

        attr = ve_value.attribute("DESCRIPTION");
        if (!attr) {
            Logger::warning(
                "rec::etf::Parser",
                fmt::format("Can't parse DESCRIPTION attribute of "
                            "CVE_VALUE {} of CONTROLLED_VOCABULARY {}",
                            ve.id, vocabulary.id));
        } else {
            ve.description = attr.as_string();
        }

        vocabulary.entries[ve.id] = ve;
    }

    vocabulary_entries = node.children("CV_ENTRY");
    for (auto ve_it = vocabulary_entries.begin();
         ve_it != vocabulary_entries.end(); ve_it++) {
        vocabulary_entry_t ve;

        ve.id = ve_it->text().as_string();
        if (!ve.id.length()) {
            Logger::warning("rec::etf::Parser",
                            fmt::format("Can't parse value of "
                                        "CV_ENTRY of CONTROLLED_VOCABULARY {}",
                                        vocabulary.id));
            continue;
        }
        attr = ve_it->attribute("DESCRIPTION");
        if (!attr) {
            Logger::warning(
                "rec::etf::Parser",
                fmt::format("Can't parse DESCRIPTION attribute of "
                            "CV_ENTRY {} of CONTROLLED_VOCABULARY {}",
                            ve.id, vocabulary.id));
        } else {
            ve.description = attr.as_string();
        }

        vocabulary.entries[ve.id] = ve;
    }

    if (!vocabulary.entries.size()) {
        Logger::warning(
            "rec::etf::Parser",
            fmt::format("Can't find any entry of CONTROLLED_VOCABULARY {}",
                        vocabulary.id));
    }

    return true;
}

}  // namespace rec::etf