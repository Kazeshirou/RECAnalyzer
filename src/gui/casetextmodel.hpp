#pragma once

#include <QAbstractTableModel>
#include <QVector>
#include <mc_entities.hpp>

#include "annotationsviewsettings.hpp"

class CaseTextModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit CaseTextModel(AnnotationsViewSettings& settings,
                           QObject*                 parent = nullptr);

    void        setCase(mc::case_t* current_case);
    mc::case_t* getCase() {
        return case_;
    }

    AnnotationsViewSettings& getSettings() {
        return settings_;
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int                role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    AnnotationsViewSettings& settings_;
    mc::case_t*              case_{nullptr};
};
