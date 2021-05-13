#pragma once

#include <QAbstractTableModel>
#include <rec_entry.hpp>

class RecEntryModel : public QAbstractTableModel {
    Q_OBJECT

public:
    RecEntryModel(QObject* parent = nullptr);
    ~RecEntryModel();

    void setRecEntry(rec::rec_entry_t* rec);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int                role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    rec::rec_entry_t* rec_{nullptr};
};
