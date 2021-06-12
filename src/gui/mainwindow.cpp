#include "mainwindow.hpp"

#include <QKeySequence>
#include <QtWidgets>

#include <eaf_parser.hpp>
#include <rec_entry.hpp>

#include "casewindow.hpp"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    currentPath_ = QDir::homePath();

    QMenu*   fileMenu = new QMenu(tr("&Файл"), this);
    QAction* openAction = fileMenu->addAction(tr("&Открыть файл разметки..."));
    openAction->setShortcuts(QKeySequence::Open);
    QAction* openTemplateAction =
        fileMenu->addAction(tr("Открыть &шаблон разметки..."));
    openTemplateAction->setShortcut(
        QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_P)));
    QAction* openTransactionsAction =
        fileMenu->addAction(tr("Открыть файл &транзакций..."));
    openTransactionsAction->setShortcut(
        QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_T)));
    QAction* openSetsAction =
        fileMenu->addAction(tr("Открыть файл &наборов..."));
    openSetsAction->setShortcut(
        QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_S)));
    QAction* openRulesAction =
        fileMenu->addAction(tr("Открыть файл &правил..."));
    openRulesAction->setShortcut(
        QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_R)));
    QAction* openClusteringAction =
        fileMenu->addAction(tr("Открыть файл &кластеризации..."));
    openClusteringAction->setShortcut(
        QKeySequence(QKeyCombination(Qt::CTRL, Qt::Key_C)));


    QAction* quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcuts(QKeySequence::Quit);

    QMenu*   helpMenu    = new QMenu(tr("&Help"), this);
    QAction* aboutAction = helpMenu->addAction(tr("&About"));

    menuBar()->addMenu(fileMenu);
    menuBar()->addSeparator();
    menuBar()->addMenu(helpMenu);


    connect(openAction, &QAction::triggered, this, &MainWindow::chooseEaf);
    connect(openTemplateAction, &QAction::triggered, this,
            &MainWindow::chooseTemplate);
    connect(openTransactionsAction, &QAction::triggered, this,
            &MainWindow::chooseTransactions);
    connect(openSetsAction, &QAction::triggered, this, &MainWindow::chooseSets);
    connect(openRulesAction, &QAction::triggered, this,
            &MainWindow::chooseRules);
    connect(openClusteringAction, &QAction::triggered, this,
            &MainWindow::chooseClustering);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutBox);

    setWindowTitle(tr("RECAnalyzer"));
    resize(640, 480);
}

void MainWindow::chooseEaf() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Выберите файл разметки"), currentPath_, "*");

    if (!fileName.isEmpty()) {
        openEaf(fileName);
    }
}

void MainWindow::chooseTemplate() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Выберите шаблон разметки"), currentPath_, "*");

    if (!fileName.isEmpty()) {
        openTemplate(fileName);
    }
}

void MainWindow::chooseTransactions() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Выберите файл транзакций"), currentPath_, "*");

    if (!fileName.isEmpty()) {
        openTransactions(fileName);
    }
}

void MainWindow::chooseSets() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Выберите файл наборов"), currentPath_, "*");

    if (!fileName.isEmpty()) {
        openSets(fileName);
    }
}

void MainWindow::chooseRules() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Выберите файл правил"), currentPath_, "*");

    if (!fileName.isEmpty()) {
        openRules(fileName);
    }
}

void MainWindow::chooseClustering() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Выберите файл кластеризации"), currentPath_, "*");

    if (!fileName.isEmpty()) {
        openClustering(fileName);
    }
}

void MainWindow::openEaf(const QString& fileName) {
    auto eaf_opt = analyzer_.read_rec_entry(fileName.toStdString());
    if (!eaf_opt.has_value()) {
        QMessageBox::warning(this, tr("Открытие файла разметки"),
                             tr("Не удалось открыть файл {}").arg(fileName));
        return;
    }

    currentPath_ = fileName.first(fileName.lastIndexOf("/"));

    auto caseWindow = new CaseWindow(
        currentPath_, new rec::rec_entry_t(eaf_opt.value()), this);
    caseWindow->show();
    windows_.append(caseWindow);
    connect(caseWindow, &CaseWindow::will_close, this, [this](CaseWindow* ptr) {
        windows_.removeOne(ptr);
        delete ptr;
    });
}

void MainWindow::openTemplate(const QString& fileName) {
    auto template_opt = analyzer_.read_rec_template(fileName.toStdString());
    if (template_opt.has_value()) {
        currentPath_ = fileName.first(fileName.lastIndexOf("/"));
        analyzer_.set_template(template_opt.value());
        for (auto& window : windows_) {
            window->close();
        }
    } else {
        QMessageBox::warning(this, tr("Открытие шаблона разметки"),
                             tr("Не удалось разобрать файл {}").arg(fileName));
    }
}

void MainWindow::openTransactions(const QString& fileName) {
    auto opt = analyzer_.read_case(fileName.toStdString());
    if (!opt.has_value()) {
        QMessageBox::warning(this, tr("Открытие файла транзакций"),
                             tr("Не удалось открыть файл {}").arg(fileName));
        return;
    }

    currentPath_ = fileName.first(fileName.lastIndexOf("/"));

    auto caseWindow =
        new CaseWindow(currentPath_, analyzer_.get_template(),
                       new mc::case_t(std::move(opt.value())), this);
    caseWindow->show();
    windows_.append(caseWindow);
    connect(caseWindow, &CaseWindow::will_close, this, [this](CaseWindow* ptr) {
        windows_.removeOne(ptr);
        delete ptr;
    });
}

void MainWindow::openSets(const QString& fileName) {
    auto opt = analyzer_.read_case(fileName.toStdString());
    if (!opt.has_value()) {
        QMessageBox::warning(this, tr("Открытие файла наборов"),
                             tr("Не удалось открыть файл {}").arg(fileName));
        return;
    }

    currentPath_ = fileName.first(fileName.lastIndexOf("/"));

    auto caseWindow =
        new CaseWindow(currentPath_, analyzer_.get_template(),
                       new mc::case_t(std::move(opt.value())), this);
    caseWindow->show();
    windows_.append(caseWindow);
    connect(caseWindow, &CaseWindow::will_close, this, [this](CaseWindow* ptr) {
        windows_.removeOne(ptr);
        delete ptr;
    });
}

void MainWindow::openRules(const QString& fileName) {
    auto opt = analyzer_.read_case(fileName.toStdString());
    if (!opt.has_value()) {
        QMessageBox::warning(this, tr("Открытие файла правил"),
                             tr("Не удалось открыть файл {}").arg(fileName));
        return;
    }

    currentPath_ = fileName.first(fileName.lastIndexOf("/"));

    auto caseWindow =
        new CaseWindow(currentPath_, analyzer_.get_template(),
                       new mc::case_t(std::move(opt.value())), this);
    caseWindow->show();
    windows_.append(caseWindow);
    connect(caseWindow, &CaseWindow::will_close, this, [this](CaseWindow* ptr) {
        windows_.removeOne(ptr);
        delete ptr;
    });
}

void MainWindow::openClustering(const QString& fileName) {
    auto opt = analyzer_.read_case(fileName.toStdString());
    if (!opt.has_value()) {
        QMessageBox::warning(this, tr("Открытие файла кластеризации"),
                             tr("Не удалось открыть файл {}").arg(fileName));
        return;
    }

    currentPath_ = fileName.first(fileName.lastIndexOf("/"));

    auto caseWindow =
        new CaseWindow(currentPath_, analyzer_.get_template(),
                       new mc::case_t(std::move(opt.value())), this);
    caseWindow->show();
    windows_.append(caseWindow);
    connect(caseWindow, &CaseWindow::will_close, this, [this](CaseWindow* ptr) {
        windows_.removeOne(ptr);
        delete ptr;
    });
}


void MainWindow::showAboutBox() {
    QMessageBox::about(this, tr("About the RECAnalyzer"), tr("Desctiprion"));
}
