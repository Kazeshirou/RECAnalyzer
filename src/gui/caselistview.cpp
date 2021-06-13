#include "caselistview.hpp"

CaseListView::CaseListView(QWidget* parent) : QTableView(parent) {}

void CaseListView::selectFromTable(const QItemSelection& selected,
                                   const QItemSelection& deselected) {
    for (const auto& selection : selected) {
        selectionModel()->select(
            {QItemSelectionRange(
                model()->index(selection.left(), 0),
                model()->index(selection.right(), model()->columnCount() - 1))},
            QItemSelectionModel::SelectionFlags::enum_type::Select);
    }
    for (const auto& selection : deselected) {
        selectionModel()->select(
            {QItemSelectionRange(
                model()->index(selection.left(), 0),
                model()->index(selection.right(), model()->columnCount() - 1))},
            QItemSelectionModel::SelectionFlags::enum_type::Deselect);
    }
    if (selected.size()) {
        scrollTo(model()->index(selected[0].left(), 0));
    }
}
