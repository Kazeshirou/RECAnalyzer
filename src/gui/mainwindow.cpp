#include "mainwindow.hpp"

#include <QtWidgets>
#include <eaf_parser.hpp>
#include <mc_by_time_slots_mining.hpp>

#include "casewidget.hpp"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    currentPath = QDir::homePath();

    QWidget* centralWidget = new QWidget;
    statusBar()->showMessage("ready!");

    caseWidget = new CaseWidget(this);

    QLabel*   pixelSizeLabel   = new QLabel(tr("Pixel size:"));
    QSpinBox* pixelSizeSpinBox = new QSpinBox;
    pixelSizeSpinBox->setMinimum(4);
    pixelSizeSpinBox->setMaximum(32);
    pixelSizeSpinBox->setValue(12);

    QLabel*   oneTimeSlotSizeLabel = new QLabel(tr("One time slot size:"));
    QSpinBox* oneTimeSlotSizeBox   = new QSpinBox;
    oneTimeSlotSizeBox->setMinimum(1);
    oneTimeSlotSizeBox->setMaximum(1000);
    oneTimeSlotSizeBox->setValue(10);

    QMenu*   fileMenu   = new QMenu(tr("&File"), this);
    QAction* openAction = fileMenu->addAction(tr("&Open..."));
    openAction->setShortcuts(QKeySequence::Open);

    QAction* quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcuts(QKeySequence::Quit);

    QMenu*   helpMenu    = new QMenu(tr("&Help"), this);
    QAction* aboutAction = helpMenu->addAction(tr("&About"));

    menuBar()->addMenu(fileMenu);
    menuBar()->addSeparator();
    menuBar()->addMenu(helpMenu);

    connect(openAction, &QAction::triggered, this, &MainWindow::chooseEaf);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutBox);

    connect(pixelSizeSpinBox, &QSpinBox::valueChanged, caseWidget,
            &CaseWidget::changePixelSize);
    connect(pixelSizeSpinBox, &QSpinBox::valueChanged, caseWidget,
            &CaseWidget::updateView);
    //    connect(oneTimeSlotSizeBox, &QSpinBox::valueChanged, delegate,
    //            &PixelDelegate::setOneTimeSlotSize);
    connect(oneTimeSlotSizeBox, &QSpinBox::valueChanged, caseWidget,
            &CaseWidget::updateView);

    //    connect(delegate, &PixelDelegate::setStatusTip, statusBar(),
    //            &QStatusBar::showMessage);


    QHBoxLayout* controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(pixelSizeLabel);
    controlsLayout->addWidget(pixelSizeSpinBox);
    controlsLayout->addWidget(oneTimeSlotSizeLabel);
    controlsLayout->addWidget(oneTimeSlotSizeBox);
    controlsLayout->addStretch(1);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(caseWidget);
    mainLayout->addLayout(controlsLayout);
    centralWidget->setLayout(mainLayout);

    setCentralWidget(centralWidget);
    setWindowTitle(tr("RECAnalyzer"));
    resize(640, 480);
}

void MainWindow::chooseEaf() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose an image"),
                                                    currentPath, "*.eaf");

    if (!fileName.isEmpty()) {
        openEaf(fileName);
    }
}

void MainWindow::openEaf(const QString& fileName) {
    auto eaf_opt = rec::eaf::Parser::parse_file(fileName.toStdString());
    if (!eaf_opt.has_value()) {
        QMessageBox::warning(this, tr("Open eaf"),
                             tr("Cant't parse file {}").arg(fileName));
        return;
    }

    recEntry_ = new rec::rec_entry_t(recTemplate_, eaf_opt.value());
    mc::transaction::algorithm::By_time_slots_mining alg;
    caseWidget->setCase(new mc::case_t(alg.run(*recEntry_)));
    if (!fileName.startsWith(":/")) {
        currentPath = fileName;
        setWindowTitle(tr("%1 - RECAnalyzer").arg(currentPath));
    }
    caseWidget->updateView();
}


void MainWindow::showAboutBox() {
    QMessageBox::about(this, tr("About the RECAnalyzer"), tr("Desctiprion"));
}
