#include "casetableview.hpp"

#include <QAction>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QGuiApplication>
#include <QHeaderView>
#include <QItemSelection>
#include <QItemSelectionRange>
#include <QMenu>
#include <QStatusBar>

#include "casevisualisationmodel.hpp"

CaseTableView::CaseTableView(QWidget* parent) : QTableView(parent) {
    setShowGrid(false);
    horizontalHeader()->setMinimumSectionSize(1);
    verticalHeader()->setMinimumSectionSize(1);
    horizontalHeader()->show();
    verticalHeader()->show();
    setMouseTracking(true);

    contextMenu_ = new QMenu(this);

    QAction* selectColumnAction = new QAction("Выделить &столбец", this);
    selectColumnAction->setShortcut(Qt::CTRL | Qt::Key_V);
    addAction(selectColumnAction);
    contextMenu_->addAction(selectColumnAction);

    QAction* selectRowAction = new QAction("Выделить ст&року", this);
    selectRowAction->setShortcut(Qt::CTRL | Qt::Key_H);
    addAction(selectRowAction);
    contextMenu_->addAction(selectRowAction);

    QAction* findAction = new QAction("&Найти все вхождения", this);
    findAction->setShortcut(QKeySequence::Find);
    addAction(findAction);
    contextMenu_->addAction(findAction);

    QAction* toNextAction = new QAction("&Следующее вхождение", this);
    toNextAction->setShortcut(Qt::CTRL | Qt::Key_N);
    addAction(toNextAction);
    contextMenu_->addAction(toNextAction);

    QAction* toPrevAction = new QAction("&Предыдущее вхождение", this);
    toPrevAction->setShortcut(Qt::CTRL | Qt::Key_P);
    addAction(toPrevAction);
    contextMenu_->addAction(toPrevAction);

    QAction* copyAction =
        new QAction("С&копировать текущий столбец (текст)", this);
    copyAction->setShortcut(Qt::CTRL | Qt::Key_C);
    addAction(copyAction);
    contextMenu_->addAction(copyAction);

    connect(selectColumnAction, &QAction::triggered, this,
            &CaseTableView::selectAllColumn);
    connect(selectRowAction, &QAction::triggered, this,
            &CaseTableView::selectAllRow);
    connect(findAction, &QAction::triggered, this, &CaseTableView::find);
    connect(toNextAction, &QAction::triggered, this, &CaseTableView::next);
    connect(toPrevAction, &QAction::triggered, this, &CaseTableView::previous);
    connect(copyAction, &QAction::triggered, this, &CaseTableView::copy);
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

void CaseTableView::find() {
    auto        selection = selectionModel()->selectedIndexes();
    mc::entry_t mask;
    for (auto& index : selection) {
        if (!index.data().isNull()) {
            mask.set_bit(index.row());
        }
    }

    currentMask_ = mask;
    if (!mask.ones()) {
        return;
    }

    occurrences_ = static_cast<CaseVisualisationModel*>(model())
                       ->getCase()
                       ->findAllOccurrences(mask);
    currentOccurrenceIndex_ = 0;

    auto currentIndex = selectionModel()->currentIndex();
    clearSelection();

    if (!occurrences_.size()) {
        return;
    }

    for (auto occur : occurrences_) {
        selectColumn(occur);
    }

    scrollTo(currentIndex);
}

void CaseTableView::next() {
    if (!occurrences_.size()) {
        return;
    }
    currentOccurrenceIndex_++;
    if (currentOccurrenceIndex_ >= occurrences_.size()) {
        currentOccurrenceIndex_ = 0;
    }
    clearSelection();
    selectColumn(occurrences_[currentOccurrenceIndex_]);
}

void CaseTableView::previous() {
    if (!occurrences_.size()) {
        return;
    }

    currentOccurrenceIndex_--;
    if (currentOccurrenceIndex_ >= occurrences_.size()) {
        currentOccurrenceIndex_ = occurrences_.size() - 1;
    }
    clearSelection();
    selectColumn(occurrences_[currentOccurrenceIndex_]);
}

void CaseTableView::selectColumn(int column) {
    QTableView::selectColumn(column);
}

void CaseTableView::selectRow(int row) {
    QTableView::selectRow(row);
}

void CaseTableView::copy() {
    QGuiApplication::clipboard()->setText(
        model()
            ->headerData(currentIndex().column(), Qt::Orientation::Horizontal,
                         Qt::StatusTipRole)
            .toString());
}

void CaseTableView::selectFromList(const QItemSelection& selected,
                                   const QItemSelection& deselected) {
    for (const auto& selection : selected) {
        if (selection.right() != 0) {
            continue;
        }
        selectionModel()->select(
            {QItemSelectionRange(
                model()->index(0, selection.top()),
                model()->index(model()->rowCount() - 1, selection.bottom()))},
            QItemSelectionModel::SelectionFlags::enum_type::Select);
    }
    for (const auto& selection : deselected) {
        selectionModel()->select(
            {QItemSelectionRange(
                model()->index(0, selection.top()),
                model()->index(model()->rowCount() - 1, selection.bottom()))},
            QItemSelectionModel::SelectionFlags::enum_type::Deselect);
    }
    if (selected.size()) {
        scrollTo(model()->index(selectionModel()->currentIndex().row(),
                                selected[0].top()));
    }
}

void CaseTableView::contextMenuEvent(QContextMenuEvent* event) {
    contextMenu_->exec(event->globalPos());
}
