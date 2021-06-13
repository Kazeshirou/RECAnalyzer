#ifndef CASETABLEVIEW_H
#define CASETABLEVIEW_H

#include <QTableView>

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

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

    QMenu* contextMenu_;
};

#endif  // CASETABLEVIEW_H
