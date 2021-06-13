#include "casewidget.hpp"

#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QSplitter>

#include "caselistview.hpp"
#include "casetableview.hpp"
#include "casetextmodel.hpp"
#include "casevisualisationmodel.hpp"
#include "pixeldelegate.hpp"

CaseWidget::CaseWidget(rec::rec_template_t& recTemplate, QWidget* parent)
        : QWidget(parent), settings_(recTemplate, this) {
    model_     = new CaseVisualisationModel(settings_, this);
    textModel_ = new CaseTextModel(settings_, this);

    view_ = new CaseTableView(this);
    view_->setModel(model_);
    delegate_ = new PixelDelegate(this);
    view_->setItemDelegate(delegate_);

    listView_ = new CaseListView(this);
    listView_->setModel(textModel_);

    connect(&settings_, &AnnotationsViewSettings::hiddenChanged, view_,
            &CaseTableView::hiddenChange);
    connect(listView_->selectionModel(), &QItemSelectionModel::selectionChanged,
            view_, &CaseTableView::selectFromList);
    connect(view_->selectionModel(), &QItemSelectionModel::selectionChanged,
            listView_, &CaseListView::selectFromTable);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    QSplitter*   splitter   = new QSplitter(this);
    splitter->addWidget(listView_);
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
    listView_->resizeColumnsToContents();
    listView_->resizeRowsToContents();
}

void CaseWidget::changePixelSize(int size) {
    delegate_->setPixelSize(size);
    updateView();
}

void CaseWidget::setCase(mc::case_t* newCase) {
    if (case_) {
        delete case_;
    }
    case_ = newCase;
    model_->setCase(case_);
    textModel_->setCase(case_);
    updateView();
}

CaseWidget::~CaseWidget() {
    if (view_) {
        delete view_;
    }
    if (model_) {
        delete model_;
    }
    if (delegate_) {
        delete delegate_;
    }
    if (case_) {
        delete case_;
        case_ = nullptr;
    }
}
