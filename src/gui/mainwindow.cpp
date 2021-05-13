#include "mainwindow.hpp"

#include <QtWidgets>
#include <eaf_parser.hpp>

#include "pixeldelegate.hpp"
#include "recentrymodel.hpp"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    currentPath = QDir::homePath();
    model       = new RecEntryModel(this);

    QWidget* centralWidget = new QWidget;
    statusBar()->showMessage("ready!");

    view = new QTableView;
    view->setShowGrid(false);
    view->horizontalHeader()->hide();
    view->verticalHeader()->hide();
    view->horizontalHeader()->setMinimumSectionSize(1);
    view->verticalHeader()->setMinimumSectionSize(1);
    view->setModel(model);

    PixelDelegate* delegate = new PixelDelegate(this);
    view->setItemDelegate(delegate);

    QLabel*   pixelSizeLabel   = new QLabel(tr("Pixel size:"));
    QSpinBox* pixelSizeSpinBox = new QSpinBox;
    pixelSizeSpinBox->setMinimum(4);
    pixelSizeSpinBox->setMaximum(32);
    pixelSizeSpinBox->setValue(12);

    QLabel*   oneTimeSlotSizeLabel = new QLabel(tr("One time slot size:"));
    QSpinBox* oneTimeSlotSizeBox   = new QSpinBox;
    oneTimeSlotSizeBox->setMinimum(1);
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

    connect(pixelSizeSpinBox, &QSpinBox::valueChanged, delegate,
            &PixelDelegate::setPixelSize);
    connect(pixelSizeSpinBox, &QSpinBox::valueChanged, this,
            &MainWindow::updateView);
    connect(oneTimeSlotSizeBox, &QSpinBox::valueChanged, delegate,
            &PixelDelegate::setOneTimeSlotSize);
    connect(oneTimeSlotSizeBox, &QSpinBox::valueChanged, this,
            &MainWindow::updateView);

    connect(delegate, &PixelDelegate::setStatusTip, statusBar(),
            &QStatusBar::showMessage);


    QHBoxLayout* controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(pixelSizeLabel);
    controlsLayout->addWidget(pixelSizeSpinBox);
    controlsLayout->addWidget(oneTimeSlotSizeLabel);
    controlsLayout->addWidget(oneTimeSlotSizeBox);
    controlsLayout->addStretch(1);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(view);
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

    model->setRecEntry(new rec::rec_entry_t(recTemplate_, eaf_opt.value()));
    if (!fileName.startsWith(":/")) {
        currentPath = fileName;
        setWindowTitle(tr("%1 - RECAnalyzer").arg(currentPath));
    }
    updateView();
}


void MainWindow::showAboutBox() {
    QMessageBox::about(this, tr("About the RECAnalyzer"), tr("Desctiprion"));
}

void MainWindow::updateView() {
    view->resizeColumnsToContents();
    view->resizeRowsToContents();
}
