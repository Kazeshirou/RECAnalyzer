#include "casewidget.hpp"

#include <QHeaderView>
#include <QTableView>
#include <QVBoxLayout>

#include "casemodel.hpp"
#include "pixeldelegate.hpp"


CaseWidget::CaseWidget(QWidget* parent) : QWidget(parent) {
    model_    = new CaseModel(this);
    delegate_ = new PixelDelegate(this);

    view_ = new QTableView(this);
    view_->setShowGrid(false);
    view_->horizontalHeader()->setMinimumSectionSize(1);
    view_->verticalHeader()->setMinimumSectionSize(1);
    view_->setModel(model_);
    view_->horizontalHeader()->show();
    view_->verticalHeader()->show();
    view_->setItemDelegate(delegate_);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(view_);
    setLayout(mainLayout);
    resize(640, 480);
}

void CaseWidget::updateView() {
    view_->resizeColumnsToContents();
    view_->resizeRowsToContents();
}

void CaseWidget::changePixelSize(int size) {
    delegate_->setPixelSize(size);
}

void CaseWidget::setCase(mc::case_t* newCase) {
    model_->setCase(newCase);
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
