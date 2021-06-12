#include "casemodel.hpp"

#include <QSize>

CaseModel::CaseModel(AnnotationsViewSettings& settings, QObject* parent)
        : QAbstractTableModel(parent), settings_{settings} {
    connect(&settings_, &AnnotationsViewSettings::colorChanged, this,
            &CaseModel::colorChange);
}

CaseModel::~CaseModel() {
    if (case_) {
        delete case_;
    }
}

void CaseModel::setCase(mc::case_t* newCase) {
    beginResetModel();
    if (case_) {
        delete case_;
    }
    case_ = newCase;
    if (newCase) {
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

int CaseModel::rowCount(const QModelIndex&) const {
    return settings_.recTemplate().annotations.size();
}

int CaseModel::columnCount(const QModelIndex&) const {
    if (!case_ || !case_->size()) {
        return 0;
    }
    return case_->size();
}

QVariant CaseModel::data(const QModelIndex& index, int role) const {
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

    if ((role != Qt::DisplayRole) && (role != Qt::UserRole) &&
        (role != Qt::StatusTipRole) && (role != Qt::ToolTipRole) &&
        (role != Qt::ForegroundRole) && (role != Qt::CheckStateRole)) {
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
        case Qt::ToolTipRole:
            return tr("%1: %2  [ %3; %4 ]")
                .arg(tier.name.c_str())
                .arg(annotation.value.c_str())
                .arg(int(tran.ts1))
                .arg(int(tran.ts2));
        case Qt::UserRole:
            return int(index.row());
        case Qt::StatusTipRole:
            return tr("%1: %2  [ %3; %4 ]")
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

QVariant CaseModel::headerData(int i, Qt::Orientation orientation,
                               int role) const {
    switch (role) {
        case Qt::SizeHintRole:
            if (orientation == Qt::Orientation::Horizontal) {
                return QSize(1, 5);
            } else {
                return QSize(5, 1);
            };
    }
    return QVariant();
}

void CaseModel::colorChange(size_t i, QBrush) {
    if (case_) {
        emit dataChanged(createIndex(i, 0), createIndex(i, case_->size()),
                         {Qt::ForegroundRole});
    }
}
