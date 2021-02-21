#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "xmlparser.h"
#include "visualizator.h"
#include "transactionbuilder.h"
#include "fileinfo.h"
#include "setanalysis.h"
#include "ruleanalysis.h"


#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _visualizator(new Visualizator()),
    _af(nullptr)
{
    connect(_visualizator, &Visualizator::mouseMoved, this, &MainWindow::cursorMoved);
    ui->setupUi(this);
    connect(ui->files, &QListWidget::currentTextChanged, this, &MainWindow::fileChanged);
    ui->view->setScene(_visualizator);
    setWindowTitle("RECAnalyzer");



    //Выделение различных ранзакций
//    QDir dir("C:\\temp\\RECAnalyzer\\20081229-a2-fumo");
//    dir.setFilter(QDir::Files);
//    dir.setNameFilters(QStringList() << "*.eaf");
//    QFileInfoList list = dir.entryInfoList();
//    QStringList files;
//    for (auto f : list)
//        files.append(f.filePath());

//    QStringList tiers;
//    tiers
////            << "текст"
////            << "текст2"
////            << "текст_стру"
//            << "текст_цель"
////            << "текст_ирония"
////            << "текст_дзл"
////            << "текст2_стру"
////            << "текст2_цель"
////            << "текст2_ирония"
////            << "текст2_дзл"
////            << "остроты"
////            << "микросост"
////            << "фазы_микросост"
//            << "глаза"
//            << "рот"
//            << "руки_способ"
//            << "руки_активн"
//            << "руки_пассивн"
//            << "руки_траект"
//            << "событие"
//            << "функция"
//            << "голова_функ"
//            << "тело_функ"
//            << "руки_функ"
//               ;



//    QString path("..\\rules");
//    QString dirname("textgoal");
//    QDir resdir(path);
//    resdir.mkdir(dirname);

//    int startstep = 100, endstep = 2000, d = 100;
//    int n = (endstep - startstep)/d + 1;
//    QVector<double> overlays;
//    overlays << 0.25 << 0.5 << 0.75 << 1.;
//    for (int i = 0; i < n; i++) {
//        int step = startstep + i*d;
//        for (auto ov : overlays) {
//            SimpleTransactionBuilder b(step, ov, tiers, files);
//            auto ts = b.buildTransactions();
//            QFile f(QString("%1\\%2\\s%3-%4.tran").arg(path).arg(dirname).arg(step).arg(ov));
//            f.open(QIODevice::WriteOnly);
//            QTextStream s(&f);
//            for (auto t : ts)
//                t.print(s);
//            f.close();
//        }
//    }

//    for (auto tier : tiers) {
//        MainTierTransactionBuilder b(tier, false, tiers, files);
//        auto ts = b.buildTransactions();
//        QFile f(QString("%1\\%2\\mt%3.tran").arg(path).arg(dirname).arg(tier));
//        f.open(QIODevice::WriteOnly);
//        QTextStream s(&f);
//        for (auto t : ts)
//            t.print(s);
//        f.close();
//    }

//    TimeOrderTransactionBuilder b(tiers, files);
//    auto ts = b.buildTransactions();
//    QFile f(QString("%1\\%2\\to.tran").arg(path).arg(dirname));
//    f.open(QIODevice::WriteOnly);
//    QTextStream s(&f);
//    for (auto t : ts)
//        t.print(s);
//    f.close();

}

MainWindow::~MainWindow()
{

    delete ui;
    for (auto w : _windows)
        delete w;
    delete _visualizator;
    TransactionBuilder::clear();
}

void MainWindow::setFiles(QStringList filenames)
{
    for (auto filename : filenames) {
        if (!filename.length())
            return;

        _af = TransactionBuilder::addFile(filename);
        if (_af)
            ui->files->addItem(_af->_filename);
    }

}

void MainWindow::on_action_triggered()
{
    QStringList filenames = QFileDialog::getOpenFileNames(
                this, tr("Открыть файл разметки"),
                "../20081229-a2-fumo/", tr("EAF Files (*.eaf)"));

    if (filenames.length())
        setFiles(filenames);
    Annotation::getAverage();
}


void MainWindow::cursorMoved(double x, QString tier)
{

    statusBar()->showMessage(QString("%1 %2; %3").arg(x).arg("ms").arg(tier));
}

void MainWindow::on_action_4_triggered()
{
    on_go_2_clicked();
}

void MainWindow::on_pushButton_clicked()
{
    if (!_af)
        QMessageBox::warning(nullptr, tr("RECAnalyzer"), QString("Не выбран файл!"));
    else {
        _windows.append(new FileInfo(_visualizator->getActiveTiers(), _af));
        _windows.last()->show();
    }
}

void MainWindow::on_open_clicked()
{
    on_action_triggered();
}

void MainWindow::on_go_clicked()
{
    _windows.append(new SetAnalysis(_currentfile));
    _windows.last()->show();
}

void MainWindow::fileChanged(QString filename)
{
    _af = TransactionBuilder::getAFByName(filename);
    _visualizator->setAnnotationFile(_af);
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
     QApplication::closeAllWindows();
     QMainWindow::closeEvent(ev);
}

void MainWindow::on_go_2_clicked()
{
    _windows.append(new RuleAnalysis(_currentfile));
    _windows.last()->show();
}

void MainWindow::on_action_2_triggered()
{
    on_go_clicked();
}

void MainWindow::on_pushButton_2_clicked()
{
    QString filename = QFileDialog::getSaveFileName(
                this, tr("Сохранить в файл"),
                "../transactions/", tr("Files (*.tran)"));
    if (!filename.length())
        return;


    TransactionBuilder *builder = nullptr;
    QStringList tiers = _visualizator->getActiveTiers();
    QStringList files;
    auto selected = ui->files->selectedItems();
    for (auto s : selected)
        files.append(s->text());
    switch (ui->transaction->currentIndex()) {
    case 0:
        builder = new SimpleTransactionBuilder(ui->step->value(), ui->overlay->value(), tiers, files);
        break;
    case 1:
        builder = new MainTierTransactionBuilder(ui->mainti->currentText(), false, tiers, files);
        break;
    case 2:
        builder = new TimeOrderTransactionBuilder(tiers, files);
        break;
    }

    QVector<Transaction> transactions = builder->buildTransactions();
    delete builder;
    if (transactions.length() < 2) {
        QMessageBox::warning(nullptr, tr("RECAnalyzer"), QString("Не удалось выделить транзакции."));
        return;
    }
    _currentfile = filename;
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QTextStream stream(&file);
    for (auto t : transactions) {
        t.print(stream);
    }
    file.close();

}

void MainWindow::on_action_3_triggered()
{
    on_pushButton_2_clicked();
}
