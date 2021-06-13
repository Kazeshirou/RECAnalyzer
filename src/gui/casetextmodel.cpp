#include "casetextmodel.hpp"

CaseTextModel::CaseTextModel(AnnotationsViewSettings& settings, QObject* parent)
        : QAbstractTableModel(parent), settings_{settings} {}

void CaseTextModel::setCase(mc::case_t* newCase) {
    beginResetModel();
    case_ = newCase;
    endResetModel();
}

int CaseTextModel::rowCount(const QModelIndex&) const {
    if (!case_) {
        return 0;
    }
    return case_->size();
}

int CaseTextModel::columnCount(const QModelIndex& index) const {
    if (!case_) {
        return 0;
    }

    return 4;
}

QVariant CaseTextModel::data(const QModelIndex& index, int role) const {
    if (!case_) {
        return QVariant();
    }
    if (!index.isValid()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (index.column() == 0) {
        QString     rv;
        const auto& entry = *case_->at(index.row());
        for (size_t j{0}; j < entry.size(); j++) {
            if (entry.check_bit(j)) {
                rv += QString("\"%1:%2\"\n")
                          .arg(settings_.recTemplate()
                                   .tiers[settings_.recTemplate()
                                              .annotations[j]
                                              .tier]
                                   .name.c_str())
                          .arg(settings_.recTemplate()
                                   .annotations[j]
                                   .value.c_str());
            }
        }
        return rv;
    }

    switch (case_->at(index.row())->get_type()) {
        case mc::ENTRY_TYPE::TRANSACTION: {
            const auto& entry =
                static_cast<const mc::transaction_t&>(*case_->at(index.row()));
            switch (index.column()) {
                case 1:
                    return qsizetype(entry.ts1);
                case 2:
                    return qsizetype(entry.ts2);
                default:
                    return QVariant();
            }
        }
        case mc::ENTRY_TYPE::SET: {
            const auto& entry =
                static_cast<const mc::set_t&>(*case_->at(index.row()));
            switch (index.column()) {
                case 1:
                    return entry.support;
                default:
                    return QVariant();
            }
        }
        case mc::ENTRY_TYPE::RULE: {
            const auto& entry =
                static_cast<const mc::rule_t&>(*case_->at(index.row()));
            switch (index.column()) {
                case 1:
                    return entry.support;
                case 2:
                    return entry.confidence;
                case 3:
                    return QString("\"%1:%2\"")
                        .arg(settings_.recTemplate()
                                 .tiers[settings_.recTemplate()
                                            .annotations[entry.target]
                                            .tier]
                                 .name.c_str())
                        .arg(settings_.recTemplate()
                                 .annotations[entry.target]
                                 .value.c_str());
                default:
                    return QVariant();
            }
        }
        case mc::ENTRY_TYPE::CLUSTER: {
            const auto& entry =
                static_cast<const mc::cluster_t&>(*case_->at(index.row()));
            switch (index.column()) {
                case 1:
                    return qsizetype(entry.cluster);
                default:
                    return QVariant();
            }
        }
        default:
            return QVariant();
    }
}

QVariant CaseTextModel::headerData(int i, Qt::Orientation orientation,
                                   int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation != Qt::Orientation::Vertical) {
        return QVariant();
    }
    return i + 1;
}
