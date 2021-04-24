#include "etf_parser.hpp"

#include <fmt/core.h>
#include <iostream>
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
    if (!parse_annotation(doc, etf)) {
    }
    return etf;
}

bool Parser::parse_annotation(const ::pugi::xml_node& node, etf_t& etf) {
    auto annotation = node.child("ANNOTATION_DOCUMENT");
    if (!annotation) {
        Logger::critical("rec::etf::Parser",
                         "Can't parse ANNOTATION_DOCUMENT tag");
        return false;
    }
    auto attr = annotation.attribute("AUTHOR");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse AUTHOR attribute of ANNOTATION_DOCUMENT");
    } else {
        etf.annotation.author = attr.as_string();
    }

    attr = annotation.attribute("DATE");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse DATE attribute of ANNOTATION_DOCUMENT");
    } else {
        etf.annotation.date = attr.as_string();
    }

    attr = annotation.attribute("FORMAT");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse FORMAT attribute of ANNOTATION_DOCUMENT");
    } else {
        etf.annotation.format = attr.as_string();
    }

    attr = annotation.attribute("VERSION");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse VERSION attribute of ANNOTATION_DOCUMENT");
    } else {
        etf.annotation.version = attr.as_string();
    }

    parse_header(annotation, etf);

    if (!parse_tiers(annotation, etf)) {
        return false;
    }

    parse_linguistic_types(annotation, etf);

    parse_constraints(annotation, etf);

    parse_vocabularies(annotation, etf);

    return true;
}

bool Parser::parse_header(const ::pugi::xml_node& annotation, etf_t& etf) {
    auto header = annotation.child("HEADER");
    if (!header) {
        Logger::warning("rec::etf::Parser", "Can't parse HEADER tag");
        return false;
    }

    auto attr = header.attribute("MEDIA_FILE");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse MEDIA_FILE attribute of HEADER");
    } else {
        etf.header.media_file = attr.as_string();
    }

    attr = header.attribute("TIME_UNITS");
    if (!attr) {
        Logger::warning("rec::etf::Parser",
                        "Can't parse MEDIA_FILE attribute of HEADER");
    } else {
        etf.header.time_units = attr.as_string();
    }

    return true;
}

bool Parser::parse_tiers(const ::pugi::xml_node& annotation, etf_t& etf) {
    auto tiers = annotation.children("TIER");
    if (tiers.begin() == tiers.end()) {
        Logger::critical("rec::etf::Parser", "Can't find TIER tags");
        return false;
    }

    for (auto it = tiers.begin(); it != tiers.end(); it++) {
        tier_t tier;
        auto   attr = it->attribute("TIER_ID");
        if (!attr) {
            Logger::warning("rec::etf::Parser",
                            "Can't parse TIER_ID attribute of TIER");
            continue;
        } else {
            tier.id = attr.as_string();
        }

        attr = it->attribute("PARENT_REF");
        if (attr) {
            tier.parent_ref = attr.as_string();
        }

        attr = it->attribute("LINGUISTIC_TYPE_REF");
        if (!attr) {
            Logger::warning(
                "rec::etf::Parser",
                fmt::format("Can't parse LINGUISTIC_TYPE_REF attribute of {}",
                            tier.id));
        } else {
            tier.linguistic_type_ref = attr.as_string();
        }

        etf.tiers[tier.id] = tier;
    }

    return true;
}

bool Parser::parse_linguistic_types(const ::pugi::xml_node& annotation,
                                    etf_t&                  etf) {
    auto ling_type = annotation.children("LINGUISTIC_TYPE");
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

        if (auto it = etf.linguistic_types.find(ling_type.id);
            it != etf.linguistic_types.end()) {
            ling_type = it->second;
        }

        attr = it->attribute("CONSTRAINTS");
        if (attr) {
            ling_type.constraint_refs.push_back(attr.as_string());
        }

        attr = it->attribute("CONTROLLED_VOCABULARY_REF");
        if (attr) {
            if (ling_type.vocabulary_ref.length()) {
                Logger::warning(
                    "rec::etf::Parser",
                    fmt::format("CONTROLLED_VOCABULARY_REF has already been "
                                "parsed for LINGUISTIC_TYPE {}",
                                ling_type.id));
                Logger::warning(
                    "rec::etf::Parser",
                    fmt::format(
                        "Old CONTROLLED_VOCABULARY_REF will be droped ({})",
                        ling_type.vocabulary_ref));
            }
            ling_type.vocabulary_ref = attr.as_string();
        }

        etf.linguistic_types[ling_type.id] = ling_type;
    }

    return true;
}

bool Parser::parse_constraints(const ::pugi::xml_node& annotation, etf_t& etf) {
    auto constraints = annotation.children("CONSTRAINT");
    if (constraints.begin() == constraints.end()) {
        Logger::warning("rec::etf::Parser", "Can't find CONSTRAINT tags");
        return false;
    }

    for (auto it = constraints.begin(); it != constraints.end(); it++) {
        constraint_t constraint;
        auto         attr = it->attribute("STEREOTYPE");
        if (!attr) {
            Logger::warning("rec::etf::Parser",
                            "Can't parse STEREOTYPE attribute of CONSTRAINT");
            continue;
        } else {
            constraint.id = attr.as_string();
        }

        attr = it->attribute("DESCRIPTION");
        if (!attr) {
            Logger::debug("rec::etf::Parser",
                          fmt::format("Can't parse DESCRIPTION attribute of {}",
                                      constraint.id));
        } else {
            constraint.description = attr.as_string();
        }

        etf.constraints[constraint.id] = constraint;
    }

    return true;
}

bool Parser::parse_vocabularies(const ::pugi::xml_node& annotation,
                                etf_t&                  etf) {
    auto vocabularies = annotation.children("CONTROLLED_VOCABULARY");
    if (vocabularies.begin() == vocabularies.end()) {
        Logger::warning("rec::etf::Parser", "Can't find CONSTRAINT tags");
        return false;
    }

    for (auto it = vocabularies.begin(); it != vocabularies.end(); it++) {
        vocabulary_t vocabulary;
        auto         attr = it->attribute("CV_ID");
        if (!attr) {
            Logger::warning(
                "rec::etf::Parser",
                "Can't parse CV_ID attribute of CONTROLLED_VOCABULARY");
            continue;
        } else {
            vocabulary.id = attr.as_string();
        }

        auto description = it->child("DESCRIPTION");
        if (!description) {
            Logger::debug("rec::etf::Parser",
                          fmt::format("Can't parse DESCRIPTION child node of "
                                      "CONTROLLED_VOCABULARY {}",
                                      vocabulary.id));
        } else {
            vocabulary.description = description.text().as_string();
        }

        auto vocabulary_entries = it->children("CV_ENTRY_ML");
        if (vocabulary_entries.begin() == vocabulary_entries.end()) {
            Logger::debug("rec::etf::Parser",
                          fmt::format("Can't find CV_ENTRY_ML children of  "
                                      "CONTROLLED_VOCABULARY {}",
                                      vocabulary.id));
            continue;
        }

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
                continue;
            }

            ve.id = ve_value.text().as_string();

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

        etf.vocabularies[vocabulary.id] = vocabulary;
    }

    return true;
}

}  // namespace rec::etf