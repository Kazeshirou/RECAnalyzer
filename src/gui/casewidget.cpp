#include "casewidget.hpp"
#include <QHBoxLayout>
#include <QSplitter>

#include "casetableview.h"
#include "casevisualisationmodel.hpp"
#include "pixeldelegate.hpp"

CaseWidget::CaseWidget(rec::rec_template_t& recTemplate, QWidget* parent)
        : QWidget(parent), settins_(recTemplate, this) {
    model_ = new CaseVisualisationModel(settins_, this);

    view_ = new CaseTableView(this);
    view_->setModel(model_);
    delegate_ = new PixelDelegate(this);
    view_->setItemDelegate(delegate_);

    connect(&settins_, &AnnotationsViewSettings::hiddenChanged, view_,
            &CaseTableView::hiddenChange);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    QSplitter*   splitter   = new QSplitter(this);
    splitter->addWidget(view_);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    mainLayout->addWidget(splitter);

    setLayout(mainLayout);
    resize(640, 480);
}

void CaseWidget::updateView() {
    view_->resizeColumnsToContents();
    view_->resizeRowsToContents();
}

void CaseWidget::changePixelSize(int size) {
    delegate_->setPixelSize(size);
    updateView();
}

void CaseWidget::setCase(mc::case_t* newCase) {
    model_->setCase(newCase);
    updateView();
}

CaseWidget::~CaseWidget() {
    if (model_) {
        delete model_;
    }
    if (delegate_) {
        delete delegate_;
    }
    if (view_) {
        delete view_;
    }
}
