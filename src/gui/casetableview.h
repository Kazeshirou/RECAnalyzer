#ifndef CASETABLEVIEW_H
#define CASETABLEVIEW_H

#include <QTableView>

#include <vector>

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

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

    QMenu*              contextMenu_;
    std::vector<size_t> occurrences_;
    size_t              currentOccurrenceIndex_{0};
};

#endif  // CASETABLEVIEW_H
