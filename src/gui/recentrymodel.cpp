#include "recentrymodel.hpp"

#include <QSize>

RecEntryModel::RecEntryModel(QObject* parent) : QAbstractTableModel(parent) {}

RecEntryModel::~RecEntryModel() {
    if (rec_) {
        delete rec_;
    }
}

void RecEntryModel::setRecEntry(rec::rec_entry_t* rec) {
    beginResetModel();
    if (rec_) {
        delete rec_;
    }
    rec_ = rec;
    endResetModel();
}

int RecEntryModel::rowCount(const QModelIndex&) const {
    if (!rec_) {
        return 0;
    }
    return rec_->rec_template.annotations.size();
}

int RecEntryModel::columnCount(const QModelIndex&) const {
    if (!rec_ || !rec_->time_slots.size()) {
        return 0;
    }
    return rec_->time_slots.size() - 1;
}

QVariant RecEntryModel::data(const QModelIndex& index, int role) const {
    if (!rec_) {
        return QVariant();
    }
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::SizeHintRole) {
        size_t ts1 = index.column();
        size_t ts2 = index.column() + 1;
        return QSize(rec_->time_slots[ts2].value - rec_->time_slots[ts1].value,
                     1);
    }

    if ((role != Qt::DisplayRole) && (role != Qt::UserRole) &&
        (role != Qt::StatusTipRole) && (role != Qt::ToolTipRole)) {
        return QVariant();
    }


    size_t ts1          = index.column();
    size_t ts2          = index.column() + 1;
    size_t annotationId = index.row();
    bool   annotationInTimeSlot{false};
    for (const auto& event : rec_->annotations) {
        if (event.ts1 >= ts2) {
            break;
        }
        if (event.annotation_id != annotationId) {
            continue;
        }

        if ((event.ts1 <= ts1) && (event.ts2 >= ts2)) {
            annotationInTimeSlot = true;
            break;
        }
    }

    if (!annotationInTimeSlot) {
        return QVariant();
    }

    auto& annotation = rec_->rec_template.annotations[annotationId];
    auto& tier       = rec_->rec_template.tiers[annotation.tier];
    switch (role) {
        case Qt::DisplayRole:
            return annotation.value.c_str();
        case Qt::ToolTipRole:
            return tr("%1: %2  [ %3; %4 ]")
                .arg(tier.name.c_str())
                .arg(annotation.value.c_str())
                .arg(int(rec_->time_slots[ts1].value))
                .arg(int(rec_->time_slots[ts2].value));
        case Qt::UserRole:
            return int(annotationId);
        case Qt::StatusTipRole:
            return tr("%1: %2  [ %3; %4 ]")
                .arg(tier.name.c_str())
                .arg(annotation.value.c_str())
                .arg(int(rec_->time_slots[ts1].value))
                .arg(int(rec_->time_slots[ts2].value));
        default:
            return QVariant();
    }
}

QVariant RecEntryModel::headerData(int /* section */,
                                   Qt::Orientation /* orientation */,
                                   int role) const {
    if (role == Qt::SizeHintRole)
        return QSize(1, 1);
    return QVariant();
}
