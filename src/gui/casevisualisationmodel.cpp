#include "casevisualisationmodel.hpp"

#include <QSize>

CaseVisualisationModel::CaseVisualisationModel(
    AnnotationsViewSettings& settings, QObject* parent)
        : QAbstractTableModel(parent), settings_{settings} {
    connect(&settings_, &AnnotationsViewSettings::colorChanged, this,
            &CaseVisualisationModel::colorChange);
}

void CaseVisualisationModel::setCase(mc::case_t* newCase) {
    beginResetModel();
    case_ = newCase;
    if (case_) {
        annotationsCount_ =
            QVector<size_t>(settings_.recTemplate().annotations.size(), 0);
        for (auto mask : *case_) {
            for (size_t i{0}; i < annotationsCount_.size(); i++) {
                if (mask->check_bit(i)) {
                    annotationsCount_[i]++;
                }
            }
        }
    }

    for (size_t i{0}; i < annotationsCount_.size(); i++) {
        settings_.setHidden(i, !annotationsCount_[i]);
    }

    endResetModel();
}

int CaseVisualisationModel::rowCount(const QModelIndex&) const {
    return settings_.recTemplate().annotations.size();
}

int CaseVisualisationModel::columnCount(const QModelIndex&) const {
    if (!case_ || !case_->size()) {
        return 0;
    }
    return case_->size();
}

QVariant CaseVisualisationModel::data(const QModelIndex& index,
                                      int                role) const {
    if (!case_) {
        return QVariant();
    }
    if (!index.isValid()) {
        return QVariant();
    }

    auto& tran = *static_cast<mc::transaction_t*>(case_->at(index.column()));
    if (role == Qt::SizeHintRole) {
        return QVariant();
        // return QSize(tran.ts2 - tran.ts1, 1);
    }

    if (role == Qt::ToolTipRole) {
        return headerData(index.column(), Qt::Orientation::Horizontal,
                          Qt::StatusTipRole);
    }

    if ((role != Qt::DisplayRole) && (role != Qt::UserRole) &&
        (role != Qt::StatusTipRole) && (role != Qt::ForegroundRole) &&
        (role != Qt::CheckStateRole)) {
        return QVariant();
    }

    if (!tran.check_bit(index.row())) {
        return QVariant();
    }

    auto& annotation = settings_.recTemplate().annotations[index.row()];
    auto& tier       = settings_.recTemplate().tiers[annotation.tier];
    switch (role) {
        case Qt::DisplayRole:
            return annotation.value.c_str();
        case Qt::UserRole:
            return int(index.row());
        case Qt::StatusTipRole:
            return tr("\"%1:%2\"")
                .arg(tier.name.c_str())
                .arg(annotation.value.c_str())
                .arg(int(tran.ts1))
                .arg(int(tran.ts2));
        case Qt::ForegroundRole:
            return settings_.color(index.row());
        case Qt::CheckStateRole:
            return !settings_.hidden(index.row());
        default:
            return QVariant();
    }
}

QVariant CaseVisualisationModel::headerData(int i, Qt::Orientation orientation,
                                            int role) const {
    switch (role) {
        case Qt::SizeHintRole:
            if (orientation == Qt::Orientation::Horizontal) {
                return QSize(1, 5);
            } else {
                return QSize(5, 1);
            };
        case Qt::StatusTipRole:
            if (!case_) {
                return QVariant();
            }
            if (orientation == Qt::Orientation::Horizontal) {
                QString     rv    = "";
                const auto& entry = *case_->at(i);
                for (size_t j{0}; j < entry.size(); j++) {
                    if (entry.check_bit(j)) {
                        rv += QString("\n\"%1:%2\"")
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
                switch (case_->at(i)->get_type()) {
                    case mc::ENTRY_TYPE::TRANSACTION: {
                        auto& tran =
                            *static_cast<mc::transaction_t*>(case_->at(i));
                        rv = QString("[%1; %2]:").arg(tran.ts1).arg(tran.ts2) +
                             rv;
                    } break;
                    case mc::ENTRY_TYPE::SET: {
                        auto& set = *static_cast<mc::set_t*>(case_->at(i));
                        rv = QString("[support = %1]:").arg(set.support) + rv;
                    } break;
                    case mc::ENTRY_TYPE::RULE: {
                        auto& rule = *static_cast<mc::rule_t*>(case_->at(i));
                        rv = QString("[support = %1,confidence = %2\n%3]:")
                                 .arg(rule.support)
                                 .arg(rule.confidence)
                                 .arg(headerData(rule.target, Qt::Vertical,
                                                 Qt::StatusTipRole)
                                          .toString()) +
                             rv;
                    } break;
                    case mc::ENTRY_TYPE::CLUSTER: {
                        auto& cluster =
                            *static_cast<mc::cluster_t*>(case_->at(i));
                        rv = QString("[cluster %1]:").arg(cluster.cluster) + rv;
                    } break;
                    default:
                        break;
                }
                return rv;

            } else {
                return QString("\"%1:%2\"")
                    .arg(settings_.recTemplate()
                             .tiers[settings_.recTemplate().annotations[i].tier]
                             .name.c_str())
                    .arg(settings_.recTemplate().annotations[i].value.c_str());
            };
    }
    return QVariant();
}

void CaseVisualisationModel::colorChange(size_t i, QBrush) {
    if (case_) {
        emit dataChanged(createIndex(i, 0), createIndex(i, case_->size()),
                         {Qt::ForegroundRole});
    }
}
