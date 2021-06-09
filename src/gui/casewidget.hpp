#ifndef CASEWIDGET_HPP
#define CASEWIDGET_HPP

#include <QWidget>

class CaseModel;
class CaseListView;
class PixelDelegate;
QT_BEGIN_NAMESPACE
class QTableView;
class QTransposeProxyModel;
class QItemSelectionModel;
QT_END_NAMESPACE

namespace mc {
class case_t;
}  // namespace mc

class CaseWidget : public QWidget {
    Q_OBJECT
public:
    explicit CaseWidget(QWidget* parent = nullptr);
    ~CaseWidget();

public slots:
    void updateView();
    void changePixelSize(int size);
    void setCase(mc::case_t* newCase);
signals:
private:
    CaseModel*            model_;
    PixelDelegate*        delegate_;
    QTransposeProxyModel* transposeProxy_;
    QItemSelectionModel*  selectionModel_;
    QTableView*           view_;
    CaseListView*         secondView_;
};

#endif  // CASEWIDGET_HPP
