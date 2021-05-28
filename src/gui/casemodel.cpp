#include "casemodel.hpp"

#include <QSize>

CaseModel::CaseModel(QObject* parent) : QAbstractTableModel(parent) {}

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
    endResetModel();
}

int CaseModel::rowCount(const QModelIndex&) const {
    if (!case_) {
        return 0;
    }
    return case_->at(0)->size();
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
        return QSize(tran.rec_entry.time_slots[tran.ts2].value -
                         tran.rec_entry.time_slots[tran.ts1].value,
                     1);
    }

    if ((role != Qt::DisplayRole) && (role != Qt::UserRole) &&
        (role != Qt::StatusTipRole) && (role != Qt::ToolTipRole)) {
        return QVariant();
    }

    if (!tran.check_bit(index.row())) {
        return QVariant();
    }

    auto& annotation = tran.rec_entry.rec_template.annotations[index.row()];
    auto& tier       = tran.rec_entry.rec_template.tiers[annotation.tier];
    switch (role) {
        case Qt::DisplayRole:
            return annotation.value.c_str();
        case Qt::ToolTipRole:
            return tr("%1: %2  [ %3; %4 ]")
                .arg(tier.name.c_str())
                .arg(annotation.value.c_str())
                .arg(int(tran.rec_entry.time_slots[tran.ts1].value))
                .arg(int(tran.rec_entry.time_slots[tran.ts2].value));
        case Qt::UserRole:
            return int(index.row());
        case Qt::StatusTipRole:
            return tr("%1: %2  [ %3; %4 ]")
                .arg(tier.name.c_str())
                .arg(annotation.value.c_str())
                .arg(int(tran.rec_entry.time_slots[tran.ts1].value))
                .arg(int(tran.rec_entry.time_slots[tran.ts2].value));
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
