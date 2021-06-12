#include "casetableview.h"

#include <QHeaderView>

CaseTableView::CaseTableView(QWidget* parent) : QTableView(parent) {
    setShowGrid(false);
    horizontalHeader()->setMinimumSectionSize(1);
    verticalHeader()->setMinimumSectionSize(1);
    horizontalHeader()->show();
    verticalHeader()->show();
}

void CaseTableView::hiddenChange(size_t i, bool value) {
    if (value) {
        hideRow(i);
    } else {
        showRow(i);
    }
}
