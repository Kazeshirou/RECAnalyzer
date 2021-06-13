#ifndef CASETABLEVIEW_H
#define CASETABLEVIEW_H

#include <QTableView>

#include <vector>

#include <mc_entities.hpp>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

class CaseTableView : public QTableView {
    Q_OBJECT
public:
    explicit CaseTableView(QWidget* parent = nullptr);

public slots:
    void hiddenChange(size_t i, bool value);
    void selectAllColumn();
    void selectAllRow();
    void find();
    void next();
    void previous();
    void selectColumn(int column);
    void selectRow(int row);
    void copy();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

    QMenu*              contextMenu_;
    std::vector<size_t> occurrences_;
    size_t              currentOccurrenceIndex_{0};
    mc::entry_t         currentMask_;
};

#endif  // CASETABLEVIEW_H
