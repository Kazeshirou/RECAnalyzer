#include "mainwindow.hpp"

#include <QtWidgets>

#include <eaf_parser.hpp>
#include <rec_entry.hpp>

#include "casewindow.hpp"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    currentPath = QDir::homePath();

    statusBar()->showMessage("ready!");


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
    if (!fileName.startsWith(":/")) {
        currentPath = fileName;
    }
    auto caseWindow = new CaseWindow(
        currentPath, new rec::rec_entry_t(recTemplate_, eaf_opt.value()), this);
    caseWindow->show();
}


void MainWindow::showAboutBox() {
    QMessageBox::about(this, tr("About the RECAnalyzer"), tr("Desctiprion"));
}
