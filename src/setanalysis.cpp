#include "setanalysis.h"
#include "ui_analisisresult.h"

#include "transactionbuilder.h"
#include "doubletablewidgetitem.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCodec>
#include <QTableWidget>
#include <QFileDialog>

int SetAnalysis::num = 0;
QString SetAnalysis::name = "RECAnalyzer";

SetAnalysis::SetAnalysis(QString filename, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AnalisisResult),
    _supps(9,0),
    _view(new QChartView())
{
    num++;
    ui->setupUi(this);
    if (filename.length()) {
        ui->filename->setText(filename);
        in = filename;
        out = filename.replace(".tran", ".set");

    }

    setWindowTitle("RECAnalyzer");
    ui->res->resizeColumnsToContents();

    _labels << "10%-20%" << "21%-30%" << "31%-40%"
                            << "41%-50%" << "51%-60%" << "61%-70%"
                               << "71%-80%" << "81%-90%" << "91%-100%";
}

void SetAnalysis::parse()
{
    setWindowTitle(out);
    _cur = out;
    ui->res->setRowCount(0);
    QFile o(out);
    o.open(QIODevice::ReadOnly);
    QTextStream stream(&o);
    stream.setCodec(QTextCodec::codecForLocale());
    int i = 0;
    QVector<double> supps(9, 0);
    while (!stream.atEnd()) {
        QString t = stream.readLine();
        QStringList list = t.split("(");
        QStringList items = list[0].split(" ");
        items.removeAll("");
        QString support = list[1].remove(")");
        ui->res->insertRow(i);
        ui->res->setItem(i, 0, new QTableWidgetItem(list[0]));
        ui->res->setItem(i, 1, new DoubleTableWidgetItem(QString::number(items.length())));
        ui->res->setItem(i, 2, new DoubleTableWidgetItem(support));
        i++;
        double supp = support.toDouble();
        if (supp >= 90)
            supps[8]++;
        else if  (supp >= 80)
            supps[7]++;
        else if  (supp >= 70)
            supps[6]++;
        else if  (supp >= 60)
            supps[5]++;
        else if  (supp >= 50)
            supps[4]++;
        else if  (supp >= 40)
            supps[3]++;
        else if  (supp >= 30)
            supps[2]++;
        else if  (supp >= 20)
            supps[1]++;
        else if  (supp >= 10)
            supps[0]++;
    }

    //auto t = QString::fromLocal8Bit(o.readAll());
    o.close();
    ui->tabWidget->setCurrentIndex(1);
    ui->res->resizeColumnsToContents();

    QFile stat(out+"-stat.txt");
    stat.open(QIODevice::WriteOnly);
    QTextStream statstream(&stat);

    _supps = supps;
    for (int i = 0; i < _supps.length(); i++) {
        _supps[i] /= static_cast<double>(ui->res->rowCount());
        statstream << _supps[i] << "\t";
    }
    statstream << "\n";
    stat.close();


    statusBar()->showMessage(QString("Всего: %1 наборов").arg(ui->res->rowCount()));

    on_action_3_triggered();
}

void SetAnalysis::parse(QString filename)
{
    out = filename;
    parse();

//    setWindowTitle(filename);
//    _cur = filename;
//    ui->res->setRowCount(0);
//    hidden.clear();
//    QFile o(filename);
//    o.open(QIODevice::ReadOnly);
//    QTextStream stream(&o);
//    stream.setCodec(QTextCodec::codecForLocale());
//    int i = 0;
//    QVector<double> supps(9, 0);
//    while (!stream.atEnd()) {
//        QString t = stream.readLine();
//        QStringList list = t.split("\t");
//        ui->res->insertRow(i);
//        ui->res->setItem(i, 0, new QTableWidgetItem(list[0]));
//        ui->res->setItem(i, 1, new QTableWidgetItem(list[1]));
//        ui->res->setItem(i, 2, new QTableWidgetItem(list[2]));
//        i++;
//        double supp = list[2].toDouble();
//        if (supp >= 90)
//            supps[8]++;
//        else if  (supp >= 80)
//            supps[7]++;
//        else if  (supp >= 70)
//            supps[6]++;
//        else if  (supp >= 60)
//            supps[5]++;
//        else if  (supp >= 50)
//            supps[4]++;
//        else if  (supp >= 40)
//            supps[3]++;
//        else if  (supp >= 30)
//            supps[2]++;
//        else if  (supp >= 20)
//            supps[1]++;
//        else if  (supp >= 10)
//            supps[0]++;
//    }
//    //auto t = QString::fromLocal8Bit(o.readAll());
//    o.close();

//    QFile stat(filename+"-stat.txt");
//    stat.open(QIODevice::WriteOnly);
//    QTextStream statstream(&stat);

//    _supps = supps;
//    for (int i = 0; i < _supps.length(); i++) {
//        _supps[i] /= static_cast<double>(ui->res->rowCount());
//        statstream << _supps[i] << "\t";
//    }
//    statstream << "\n";
//    stat.close();

//    ui->tabWidget->setCurrentIndex(1);
//    ui->res->resizeColumnsToContents();
//    statusBar()->showMessage(QString("Всего: %1 наборов").arg(ui->res->rowCount()));
//    on_action_3_triggered();
}

SetAnalysis::~SetAnalysis()
{
    delete ui;
    delete _view;
}

void SetAnalysis::on_pushButton_clicked()
{
    QStringList s = ui->searchtext->text().split(",");
    int active = 0;
    for (int i = 0; i < ui->res->rowCount(); i++) {
        if (ui->res->isRowHidden(i))
            continue;
        QString itemtext = ui->res->item(i, 0)->text();
        bool f = true;
        for (auto ts : s)
            if (!itemtext.contains(ts)) {
                ui->res->hideRow(i);
                f = false;
                break;
            }
        if (f)
            active++;
    }
    statusBar()->showMessage(QString("Найдено: %1 наборов").arg(active));
}

void SetAnalysis::on_pushButton_2_clicked()
{
    for (int i = 0; i < ui->res->rowCount(); i++)
        ui->res->showRow(i);
    statusBar()->showMessage(QString("Всего: %1 наборов").arg(ui->res->rowCount()));
}

void SetAnalysis::on_action_triggered()
{
    QString filename = QFileDialog::getSaveFileName(
                this, tr("Сохранить в файл"),
                "../sets/", tr("Files (*.set)"));

    if (!filename.length())
        return;
    QFile f(filename);
    f.open(QIODevice::WriteOnly);
    QTextStream s(&f);
    for (int i = 0; i < ui->res->rowCount(); i++) {
        if (ui->res->isRowHidden(i))
            continue;
        s << QString("%1\t%2\t%3\n")
             .arg(ui->res->item(i, 0)->text())
             .arg(ui->res->item(i, 1)->text())
             .arg(ui->res->item(i, 2)->text());

    }
    f.close();
}

void SetAnalysis::on_pushButton_3_clicked()
{
    QStringList filenames = QFileDialog::getOpenFileNames(
                this, tr("Открыть файл транзакций"),
                "../transactions/", tr("Files (*.tran)"));
    for (auto filename : filenames)
        if (filename.length()) {
            ui->filename->setText(filename);
            in = filename;
            out = filename.replace(".tran", ".set");

            on_pushButton_4_clicked();
        }
}

void SetAnalysis::on_pushButton_4_clicked()
{
    if (!in.length()) {
        QMessageBox::warning(nullptr, tr("RECAnalyzer"), QString("Не выбран файл транзакций."));
        return;
    }

    if (ui->minn->value() >= ui->maxn->value()) {
        QMessageBox::warning(nullptr, tr("RECAnalyzer"), QString("Минимальное количество элементов должно быть меньше максимального."));
        return;
    }

    QString p = QString("-m%1n%2s%3S%4")
            .arg(ui->minn->value())
            .arg(ui->maxn->value())
            .arg(ui->minsup->value())
            .arg(ui->maxsup->value());

    QProcess apr(this);
    apr.setProgram("./imageformats/apriori.exe");
//    apr.setProgram("C:\\temp\\RECAnalyzer\\apriori.exe");

    apr.setArguments(QStringList() << p << in << out);

    apr.start();

    if (!apr.waitForStarted())
        QMessageBox::warning(nullptr, tr("RECAnalyzer"), QString("Не удалось выявить наборы."));
    apr.waitForFinished(-1);

    parse();
}

void SetAnalysis::on_action_2_triggered()
{
    QString filename = QFileDialog::getOpenFileName(
                this, tr("Открыть файл наборов"),
                "../sets/", tr("Files (*.set)"));
    if (!filename.length())
        return;
    parse(filename);


}

void SetAnalysis::on_action_3_triggered()
{
    QChart *chart = new QChart();
    QFont f("Time", 14);
    f.setBold(true);
    chart->setTitle("Распределение наборов по поддержке");
    chart->setTitleFont(f);
    f.setPointSize(12);

    QFont f1("Time", 10);

    QBarSeries *series = new QBarSeries(chart);
    QBarSet * set;
    set = new QBarSet("Поддержка");
    for (auto supp : _supps)
        set->append(supp);
    series->append(set);
    chart->addSeries(series);
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    axisY->setLabelsFont(f1);
    series->attachAxis(axisY);
    chart->legend()->setVisible(true);
    chart->legend()->setFont(f);
    chart->legend()->setAlignment(Qt::AlignRight);
    axisY->setTitleText("Доля наборов, попавших в диапазон");
    axisY->setTitleFont(f);
    axisY->applyNiceNumbers();
    axisY->setMax(1.);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->setLabelsFont(f1);
    axisX->append(_labels);
    axisX->setTitleText("Диапазоны значений поддержки");
    axisX->setTitleFont(f);
    chart->addAxis(axisX, Qt::AlignBottom);
    axisY->setLabelsFont(f1);
    series->attachAxis(axisX);
    _view->setChart(chart);
    _view->setWindowTitle(_cur);
    QScreen* screen = QApplication::screens().at(0);
    QSize size = screen->availableSize();
    _view->resize(size);
    _view->show();
    auto p =_view->grab();
    p.save(_cur+".png");
}

void SetAnalysis::on_action_4_triggered()
{
    on_pushButton_3_clicked();
}
