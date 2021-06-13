#ifndef CASEWIDGET_HPP
#define CASEWIDGET_HPP

#include <QWidget>

#include <annotationsviewsettings.h>

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
    AnnotationsViewSettings settins_;
    CaseVisualisationModel*              model_;
    PixelDelegate*          delegate_;
    QTransposeProxyModel*   transposeProxy_;
    QItemSelectionModel*    selectionModel_;
    CaseTableView*          view_;
};

#endif  // CASEWIDGET_HPP
