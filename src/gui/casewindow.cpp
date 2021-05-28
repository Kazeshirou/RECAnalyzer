#include "casewindow.hpp"

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>

#include <mc_by_time_slots_mining.hpp>
#include <rec_entry.hpp>

#include "casewidget.hpp"

CaseWindow::CaseWindow(QString filename, rec::rec_entry_t* recEntry,
                       QWidget* parent)
        : QMainWindow(parent), recEntry_{recEntry} {
    QWidget* centralWidget = new QWidget(this);
    statusBar()->showMessage("", 1);

    caseWidget_ = new CaseWidget(this);
    mc::transaction::algorithm::By_time_slots_mining alg;
    caseWidget_->setCase(new mc::case_t(alg.run(*recEntry)));
    caseWidget_->changePixelSize(zoom_);

    QMenu*   viewMenu = new QMenu(tr("&View"), this);
    QAction* zoomIn   = viewMenu->addAction(tr("Zoom +"));
    zoomIn->setShortcuts(QKeySequence::ZoomIn);
    QAction* zoomOut = viewMenu->addAction(tr("Zoom -"));
    zoomOut->setShortcuts(QKeySequence::ZoomOut);

    menuBar()->addMenu(viewMenu);

    connect(zoomIn, &QAction::triggered, this, &CaseWindow::zoomIn);
    connect(zoomOut, &QAction::triggered, this, &CaseWindow::zoomOut);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(caseWidget_);
    centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);
    setWindowTitle(filename);
    resize(640, 480);
}

CaseWindow::~CaseWindow() {
    delete recEntry_;
}

void CaseWindow::zoomIn() {
    if (zoom_ < 300) {
        zoom_++;
        caseWidget_->changePixelSize(zoom_);
    }
}

void CaseWindow::zoomOut() {
    if (zoom_ > 4) {
        zoom_--;
        caseWidget_->changePixelSize(zoom_);
    }
}
