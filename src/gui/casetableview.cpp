#include "casetableview.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>

CaseTableView::CaseTableView(QWidget* parent) : QTableView(parent) {
    setShowGrid(false);
    horizontalHeader()->setMinimumSectionSize(1);
    verticalHeader()->setMinimumSectionSize(1);
    horizontalHeader()->show();
    verticalHeader()->show();

    contextMenu_ = new QMenu(this);

    QAction* selectColumnAction = new QAction("Выделить &столбец", this);
    selectColumnAction->setShortcut(Qt::CTRL | Qt::Key_V);
    addAction(selectColumnAction);
    contextMenu_->addAction(selectColumnAction);

    QAction* selectRowAction = new QAction("Выделить ст&року", this);
    selectRowAction->setShortcut(Qt::CTRL | Qt::Key_H);
    addAction(selectRowAction);
    contextMenu_->addAction(selectRowAction);

    connect(selectColumnAction, &QAction::triggered, this,
            &CaseTableView::selectAllColumn);
    connect(selectRowAction, &QAction::triggered, this,
            &CaseTableView::selectAllRow);
}

void CaseTableView::hiddenChange(size_t i, bool value) {
    if (value) {
        hideRow(i);
    } else {
        showRow(i);
    }
}

void CaseTableView::selectAllColumn() {
    selectColumn(selectionModel()->currentIndex().column());
}

void CaseTableView::selectAllRow() {
    selectRow(selectionModel()->currentIndex().row());
}

void CaseTableView::contextMenuEvent(QContextMenuEvent* event) {
    contextMenu_->exec(event->globalPos());
}
