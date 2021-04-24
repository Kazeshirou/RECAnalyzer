#include <fmt/core.h>
#include <iostream>
#include <set>

#include "args_parser.hpp"
#include "etf_parser.hpp"

int main(int argc, char* argv[]) {
    auto args = args_parser(argc, argv);

    auto etf_optional =
        rec::etf::Parser::parse_file(args["etf"].as<std::string>());
    if (!etf_optional.has_value()) {
        std::cerr << "Не удалось распарсить файл шаблона разметки: "
                  << args["etf"].as<std::string>() << std::endl;

        return 1;
    }

    auto etf = etf_optional.value();
    std::cout << "Файл шаблона разметки - " << args["etf"].as<std::string>()
              << " - успешно разобран." << std::endl;
    std::cout << fmt::format("Слои (всего {}):\n", etf.tiers.size());
    size_t                uniq_annotations_count{0};
    size_t                annotations_count{0};
    std::set<std::string> ling_types;
    for (auto& [id, tier] : etf.tiers) {
        std::cout << fmt::format("\t{}:\n\t\tродительский слой - "
                                 "{}\n\t\tлингв.тип - {}\n",
                                 id, tier.parent_ref, tier.linguistic_type_ref);

        auto lt_it = etf.linguistic_types.find(tier.linguistic_type_ref);
        if (lt_it == etf.linguistic_types.end()) {
            continue;
        }

        if (lt_it->second.constraint_refs.size()) {
            auto constr_it =
                etf.constraints.find(lt_it->second.constraint_refs[0]);
            if (constr_it != etf.constraints.end()) {
                std::cout << fmt::format("\t\tограничение - {} ({})\n",
                                         constr_it->second.id,
                                         constr_it->second.description);
            }
        }
        if (lt_it->second.vocabulary_ref.size() == 0) {
            continue;
        }

        std::cout << "\t\tаннотации:\n";
        auto cv_it = etf.vocabularies.find(lt_it->second.vocabulary_ref);
        if (cv_it == etf.vocabularies.end()) {
            continue;
        }

        bool lt_was_already_counted = true;
        if (ling_types.find(tier.linguistic_type_ref) == ling_types.end()) {
            lt_was_already_counted = false;
            ling_types.emplace(tier.linguistic_type_ref);
        }
        for (auto& [cve_id, cve] : cv_it->second.entries) {
            if (!lt_was_already_counted) {
                ++uniq_annotations_count;
            }
            ++annotations_count;
            std::cout << fmt::format("\t\t\t{} - {}\n", cve_id,
                                     cve.description);
        }
    }

    std::cout << fmt::format("[Всего|уникальных] аннотаций: [ {} | {} ]\n",
                             annotations_count, uniq_annotations_count);
    return 0;
}