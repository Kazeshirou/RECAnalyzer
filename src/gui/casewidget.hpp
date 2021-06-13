#ifndef CASEWIDGET_HPP
#define CASEWIDGET_HPP

#include <QWidget>

#include <annotationsviewsettings.hpp>

class CaseVisualisationModel;
class PixelDelegate;
QT_BEGIN_NAMESPACE
class QTransposeProxyModel;
class QItemSelectionModel;
QT_END_NAMESPACE

namespace mc {
class case_t;
}  // namespace mc

namespace rec {
struct rec_template_t;
}  // namespace rec

class CaseTableView;
class CaseListView;
class CaseTextModel;

class CaseWidget : public QWidget {
    Q_OBJECT
public:
    explicit CaseWidget(rec::rec_template_t& recTemplate,
                        QWidget*             parent = nullptr);
    ~CaseWidget();

public slots:
    void updateView();
    void changePixelSize(int size);
    void setCase(mc::case_t* newCase);
signals:
private:
    AnnotationsViewSettings settings_;
    CaseVisualisationModel* model_;
    CaseTextModel*          textModel_;
    PixelDelegate*          delegate_;
    QTransposeProxyModel*   transposeProxy_;
    QItemSelectionModel*    selectionModel_;
    CaseTableView*          view_;
    CaseListView*           listView_;
    mc::case_t*             case_{nullptr};
};

#endif  // CASEWIDGET_HPP
