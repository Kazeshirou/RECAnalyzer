#include "casewidget.hpp"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QSplitter>
#include <QTableView>


#include "caselistview.hpp"
#include "casemodel.hpp"
#include "pixeldelegate.hpp"


CaseWidget::CaseWidget(QWidget* parent) : QWidget(parent) {
    model_ = new CaseModel(this);

    view_ = new QTableView(this);
    view_->setShowGrid(false);
    view_->horizontalHeader()->setMinimumSectionSize(1);
    view_->verticalHeader()->setMinimumSectionSize(1);
    view_->setModel(model_);
    view_->horizontalHeader()->show();
    view_->verticalHeader()->show();
    delegate_ = new PixelDelegate(this);
    view_->setItemDelegate(delegate_);

    secondView_ = new CaseListView(this);
    secondView_->setModel(model_);

    selectionModel_ = new QItemSelectionModel(model_);
    view_->setSelectionModel(selectionModel_);
    secondView_->setSelectionModel(selectionModel_);


    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    QSplitter*   splitter   = new QSplitter(this);
    splitter->addWidget(secondView_);
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
    if (secondView_) {
        delete secondView_;
    }
}
