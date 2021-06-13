#pragma once

#include <QTableView>

class CaseListView : public QTableView {
    Q_OBJECT
public:
    explicit CaseListView(QWidget* parent = nullptr);

public slots:
    void selectFromTable(const QItemSelection& selected,
                         const QItemSelection& deselected);
};
