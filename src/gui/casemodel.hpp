#pragma once

#include <QAbstractTableModel>
#include <mc_entities.hpp>

class CaseModel : public QAbstractTableModel {
    Q_OBJECT

public:
    CaseModel(QObject* parent = nullptr);
    ~CaseModel();

    void setCase(mc::case_t* current_case);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int                role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    mc::case_t* case_{nullptr};
};
