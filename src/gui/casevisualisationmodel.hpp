#pragma once

#include <QAbstractTableModel>
#include <QVector>
#include <mc_entities.hpp>

#include "annotationsviewsettings.h"

class CaseVisualisationModel : public QAbstractTableModel {
    Q_OBJECT

public:
    CaseVisualisationModel(AnnotationsViewSettings& settings,
                           QObject*                 parent = nullptr);
    ~CaseVisualisationModel();

    void setCase(mc::case_t* current_case);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int                role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

protected slots:
    void colorChange(size_t i, QBrush);

private:
    AnnotationsViewSettings& settings_;
    mc::case_t*              case_{nullptr};
    QVector<size_t>          annotationsCount_;
};
