#include "ruleanalysis.h"
#include "ui_ruleanalysis.h"

#include "transactionbuilder.h"
#include "doubletablewidgetitem.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCodec>
#include <QTableWidget>
#include <QFileDialog>
#include <QProcess>



int RuleAnalysis::num = 0;
QString RuleAnalysis::name = "RECAnalyzer";

RuleAnalysis::RuleAnalysis(QString filename, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RuleAnalysis),
    _supps(9,0),
    _confs(9,0),
    _view(new QChartView())
{
    ui->setupUi(this);

    ui->filename->setText(filename);
    setWindowTitle("RECAnalyzer");
    ui->res->resizeColumnsToContents();

    _labels << "10%-20%" << "21%-30%" << "31%-40%"
                            << "41%-50%" << "51%-60%" << "61%-70%"
                               << "71%-80%" << "81%-90%" << "91%-100%";




}

void RuleAnalysis::parse()
{
    setWindowTitle(out);
    _cur = out;
    ui->res->setRowCount(0);
    QFile o(out);
    o.open(QIODevice::ReadOnly);
    QTextStream stream(&o);
    stream.setCodec(QTextCodec::codecForLocale());
    int i = 0;
    QVector<double> confs(9, 0);
    QVector<double> supps(9, 0);
    while (!stream.atEnd()) {
        QString t = stream.readLine();
        QStringList list = t.split("(");
        QStringList rule = list[0].split(" <- ");
        QStringList numbers = list[1].remove(")").split(",");
        QStringList items = rule[1].split(" ");
        items.removeAll("");
        ui->res->insertRow(i);
        ui->res->setItem(i, 0, new QTableWidgetItem(rule[0].remove(" ")));
        ui->res->setItem(i, 1, new QTableWidgetItem(rule[1]));
        ui->res->setItem(i, 2, new DoubleTableWidgetItem(QString::number(items.length())));
        ui->res->setItem(i, 3, new DoubleTableWidgetItem(QString::number(numbers[0].toDouble()*numbers[1].toDouble()/100.)));
        ui->res->setItem(i, 4, new DoubleTableWidgetItem(numbers[1]));
        i++;

        double conf = numbers[1].toDouble();
        if (conf >= 90)
            confs[8]++;
        else if  (conf >= 80)
            confs[7]++;
        else if  (conf >= 70)
            confs[6]++;
        else if  (conf >= 60)
            confs[5]++;
        else if  (conf >= 50)
            confs[4]++;
        else if  (conf >= 40)
            confs[3]++;
        else if  (conf >= 30)
            confs[2]++;
        else if  (conf >= 20)
            confs[1]++;
        else if  (conf >= 10)
            confs[0]++;

        double supp = numbers[0].toDouble();
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


    statusBar()->showMessage(QString("Всего: %1 правил").arg(ui->res->rowCount()));

    _supps = supps;
    _confs = confs;
    QFile stat(out+"-stat.txt");
    stat.open(QIODevice::WriteOnly);
    QTextStream statstream(&stat);
    for (int i = 0; i < _confs.length(); i++) {
        _confs[i] /= static_cast<double>(ui->res->rowCount());
        statstream << _confs[i] << "\t";
    }
    statstream << "\n";

    for (int i = 0; i < _supps.length(); i++) {
        _supps[i] /= static_cast<double>(ui->res->rowCount());
        statstream << _supps[i] << "\t";
    };
    statstream << "\n";
    stat.close();

    on_action_2_triggered();
}

void RuleAnalysis::parse(QString filename)
{
    out = filename;
    _cur = filename;
    parse();
//    setWindowTitle(filename);
//    _cur = filename;
//    ui->res->setRowCount(0);
//    hidden.clear();
//    QFile o(filename);
//    o.open(QIODevice::ReadOnly);
//    QTextStream stream(&o);
//    stream.setCodec(QTextCodec::codecForLocale());
//    QVector<double> confs(9, 0);
//    QVector<double> supps(9, 0);
//    int i = 0;
//    while (!stream.atEnd()) {
//        QString t = stream.readLine();
//        QStringList list = t.split("\t");
//        ui->res->insertRow(i);
//        ui->res->setItem(i, 0, new QTableWidgetItem(list[0]));
//        ui->res->setItem(i, 1, new QTableWidgetItem(list[1]));
//        ui->res->setItem(i, 2, new QTableWidgetItem(list[2]));
//        ui->res->setItem(i, 3, new QTableWidgetItem(list[3]));
//        ui->res->setItem(i, 4, new QTableWidgetItem(list[4]));
//        i++;
//        double conf = list[4].toDouble();
//        if (conf >= 90)
//            confs[8]++;
//        else if  (conf >= 80)
//            confs[7]++;
//        else if  (conf >= 70)
//            confs[6]++;
//        else if  (conf >= 60)
//            confs[5]++;
//        else if  (conf >= 50)
//            confs[4]++;
//        else if  (conf >= 40)
//            confs[3]++;
//        else if  (conf >= 30)
//            confs[2]++;
//        else if  (conf >= 20)
//            confs[1]++;
//        else if  (conf >= 10)
//            confs[0]++;

//        double supp = list[3].toDouble()*conf;
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

//    ui->tabWidget->setCurrentIndex(1);
//    ui->res->resizeColumnsToContents();
//    statusBar()->showMessage(QString("Всего: %1 правил").arg(ui->res->rowCount()));

//    _supps = supps;
//    _confs = confs;
//    QFile stat(filename+"-stat.txt");
//    stat.open(QIODevice::WriteOnly);
//    QTextStream statstream(&stat);
//    for (int j = 0; j < _confs.length(); j++) {
//        _confs[j] /= static_cast<double>(i);
//        statstream << _confs[j] << "\t";
//    }
//    statstream << "\n";

//    for (int j = 0; j < _supps.length(); j++) {
//        _supps[j] /= static_cast<double>(i);
//        statstream << _supps[j] << "\t";
//    };
//    statstream << "\n";
//    stat.close();

//    on_action_2_triggered();
}

RuleAnalysis::~RuleAnalysis()
{
    delete ui;
    delete _view;
}

void RuleAnalysis::on_action_triggered()
{
    QString filename = QFileDialog::getSaveFileName(
                this, tr("Сохранить в файл"),
                "../rules/", tr("Files (*.rule)"));
    if (!filename.length())
        return;
    QFile f(filename);
    f.open(QIODevice::WriteOnly);
    QTextStream s(&f);
    for (int i = 0; i < ui->res->rowCount(); i++) {
        if (ui->res->isRowHidden(i))
            continue;
        s << QString("%1\t%2\t%3\t%4\t%5\n")
             .arg(ui->res->item(i, 0)->text())
             .arg(ui->res->item(i, 1)->text())
             .arg(ui->res->item(i, 2)->text())
             .arg(ui->res->item(i, 3)->text())
             .arg(ui->res->item(i, 4)->text());

    }
    f.close();
}

void RuleAnalysis::on_pushButton_3_clicked()
{
    QString s = ui->searchtexthead->text().remove(" ");
    int active = 0;
    for (int i = 0; i < ui->res->rowCount(); i++) {
        if (ui->res->isRowHidden(i))
            continue;
        QString itemtext = ui->res->item(i, 0)->text();
        if (s != itemtext) {
                ui->res->hideRow(i);
                continue;
        }
        else
            active++;
    }
    statusBar()->showMessage(QString("Найдено: %1 правил").arg(active));
}

void RuleAnalysis::on_pushButton_clicked()
{
    QStringList s = ui->searchtextbody->text().split(",");
    int active = 0;
    for (int i = 0; i < ui->res->rowCount(); i++) {
        if (ui->res->isRowHidden(i))
            continue;
        QStringList itemtext = ui->res->item(i, 1)->text().split(" ");
        bool f = true;
        for (auto ts : s)
            if (!itemtext.contains(ts.remove(" "))) {
                ui->res->hideRow(i);
                f = false;
                break;
            }

        if (f)
            active++;
    }
    statusBar()->showMessage(QString("Найдено: %1 правил").arg(active));
}

void RuleAnalysis::on_pushButton_2_clicked()
{
    for (int i = 0; i < ui->res->rowCount(); i++)
        ui->res->showRow(i);
    statusBar()->showMessage(QString("Всего: %1 наборов").arg(ui->res->rowCount()));
}

void RuleAnalysis::on_pushButton_4_clicked()
{
    QStringList filenames = QFileDialog::getOpenFileNames(
                this, tr("Открыть файл транзакций"),
                "../transactions/", tr("Files (*.tran)"));
    for (auto f : filenames)
    if (f.length()) {
        ui->filename->setText(f);
        out = f.replace(".tran", ".rule");

        on_pushButton_5_clicked();
    }
}

void RuleAnalysis::on_pushButton_5_clicked()
{
//    QDir dir("C:\\temp\\RECAnalyzer\\transactions\\hand\\");
//    dir.setFilter(QDir::Files);
//    dir.setNameFilters(QStringList() << "*.tran");
//    QFileInfoList list = dir.entryInfoList();
//    QStringList files;
//    for (auto f : list)
//        files.append(f.filePath());


//    QString p1 = QString("-m%1n%2s%3S%4trc%5")
//            .arg(ui->minn->value())
//            .arg(ui->maxn->value())
//            .arg(ui->minsup->value())
//            .arg(ui->maxsup->value())
//            .arg(ui->minconf->value());

//    QProcess apr1(this);
//    apr1.setProgram("C:\\temp\\RECAnalyzer\\apriori.exe");
//    for (auto f : files) {

//        auto out = f.replace(".tran", ".rule");
//        apr1.setArguments(QStringList() << p1 << f << out);

//        apr1.start();
//        apr1.waitForFinished(-1);

//        QFile o(out);
//        o.open(QIODevice::ReadOnly);
//        QTextStream stream(&o);
//        stream.setCodec(QTextCodec::codecForLocale());

//        QVector<double> confs(9, 0);
//        QVector<double> supps(9, 0);
//        while (!stream.atEnd()) {
//            QString t = stream.readLine();
//            QStringList list = t.split("(");
//            QStringList rule = list[0].split(" <- ");
//            QStringList numbers = list[1].remove(")").split(",");
//            QStringList items = rule[1].split(" ");

//            double conf = numbers[1].toDouble();
//            if (conf >= 90)
//                confs[8]++;
//            else if  (conf >= 80)
//                confs[7]++;
//            else if  (conf >= 70)
//                confs[6]++;
//            else if  (conf >= 60)
//                confs[5]++;
//            else if  (conf >= 50)
//                confs[4]++;
//            else if  (conf >= 40)
//                confs[3]++;
//            else if  (conf >= 30)
//                confs[2]++;
//            else if  (conf >= 20)
//                confs[1]++;
//            else if  (conf >= 10)
//                confs[0]++;

//            double supp = numbers[0].toDouble()*numbers[1].toDouble()/100.;
//            supp = supp*conf/100.;
//            if (supp >= 90)
//                supps[8]++;
//            else if  (supp >= 80)
//                supps[7]++;
//            else if  (supp >= 70)
//                supps[6]++;
//            else if  (supp >= 60)
//                supps[5]++;
//            else if  (supp >= 50)
//                supps[4]++;
//            else if  (supp >= 40)
//                supps[3]++;
//            else if  (supp >= 30)
//                supps[2]++;
//            else if  (supp >= 20)
//                supps[1]++;
//            else if  (supp >= 10)
//                supps[0]++;
//        }

//        o.close();

//        _supps = supps;
//        _confs = confs;
//        QFile stat(out+"-stat.txt");
//        stat.open(QIODevice::WriteOnly);
//        QTextStream statstream(&stat);
//        for (int i = 0; i < _confs.length(); i++) {
//            _confs[i] /= static_cast<double>(ui->res->rowCount());
//            statstream << _confs[i] << "\t";
//        }
//        statstream << "\n";

//        for (int i = 0; i < _supps.length(); i++) {
//            _supps[i] /= static_cast<double>(ui->res->rowCount());
//            statstream << _supps[i] << "\t";
//        };
//        statstream << "\n";
//        stat.close();

//        QChart *chart = new QChart();
//        QFont font("Time", 14);
//        font.setBold(true);
//        chart->setTitle("Распределение правил по поддержке и достоверности");
//        chart->setTitleFont(font);
//        font.setPointSize(12);
//        QBarSeries *series = new QBarSeries(chart);
//        QVector<QBarSet *> sets(2);
//        sets[0] = new QBarSet("Поддержка");
//        sets[1] = new QBarSet("Достоверность");
//        for (int i = 0; i < _supps.length(); i++) {
//             *sets[0] << _supps[i];
//             *sets[1] << _confs[i];
//        }

//        QFont f1("Time", 10);

//        series->append(sets[0]);
//        series->append(sets[1]);
//        chart->addSeries(series);
//        QValueAxis *axisY = new QValueAxis();
//        axisY->setLabelsFont(f1);
//        chart->addAxis(axisY, Qt::AlignLeft);
//        series->attachAxis(axisY);
//        chart->legend()->setVisible(true);
//        chart->legend()->setFont(font);
//        chart->legend()->setAlignment(Qt::AlignRight);
//        axisY->setTitleText("Доля правил, попавших в диапазон");
//        axisY->setTitleFont(font);
//        axisY->applyNiceNumbers();
//        axisY->setMax(1.);

//        QBarCategoryAxis *axisX = new QBarCategoryAxis();
//        axisX->setLabelsFont(f1);
//        axisX->append(_labels);
//        axisX->setTitleText("Диапазоны значений поддержки и достоверности");
//        axisX->setTitleFont(font);
//        chart->addAxis(axisX, Qt::AlignBottom);
//        series->attachAxis(axisX);
//        _view->setChart(chart);
//        _view->setWindowTitle(_cur);
//        QScreen* screen = QApplication::screens().at(0);
//        QSize size = screen->availableSize();
//        _view->resize(size);
//        auto p =_view->grab();
//        p.save(out+".png");
//    }
    QString in = ui->filename->text();
    if (!in.length()) {
        QMessageBox::warning(nullptr, tr("RECAnalyzer"), QString("Не выбран файл транзакций."));
        return;
    }
    if (ui->minn->value() >= ui->maxn->value()) {
        QMessageBox::warning(nullptr, tr("RECAnalyzer"), QString("Минимальное количество элементов должно быть меньше максимального."));
        return;
    }
    if (ui->minsup->value() >= ui->maxsup->value()) {
        QMessageBox::warning(nullptr, tr("RECAnalyzer"), QString("Минимальная поддержка элементов должна быть меньше максимальной."));
        return;
    }

    QString p = QString("-m%1n%2s%3S%4trc%5")
            .arg(ui->minn->value())
            .arg(ui->maxn->value())
            .arg(ui->minsup->value())
            .arg(ui->maxsup->value())
            .arg(ui->minconf->value());

    QProcess apr(this);
    apr.setProgram("./imageformats/apriori.exe");
//    apr.setProgram("C:\\temp\\RECAnalyzer\\apriori.exe");

    apr.setArguments(QStringList() << p << in << out);

    apr.start();

    if (!apr.waitForStarted())
        QMessageBox::warning(nullptr, tr("RECAnalyzer"), QString("Не удалось выявить ассоциативные правила."));
    apr.waitForFinished(-1);

    parse();

}

void RuleAnalysis::on_action_3_triggered()
{
    QString filename = QFileDialog::getOpenFileName(
                this, tr("Открыть файл правил"),
                "../rules/", tr("Files (*.rule)"));
    if (!filename.length())
        return;
    parse(filename);
}

void RuleAnalysis::on_action_2_triggered()
{
    QChart *chart = new QChart();
    QFont f("Time", 14);
    f.setBold(true);
    chart->setTitle("Распределение правил по поддержке и достоверности");
    chart->setTitleFont(f);
    f.setPointSize(12);
    QBarSeries *series = new QBarSeries(chart);
    QVector<QBarSet *> sets(2);
    sets[0] = new QBarSet("Поддержка");
    sets[1] = new QBarSet("Достоверность");
    for (int i = 0; i < _supps.length(); i++) {
         *sets[0] << _supps[i];
         *sets[1] << _confs[i];
    }

    QFont f1("Time", 10);

    series->append(sets[0]);
    series->append(sets[1]);
    chart->addSeries(series);
    QValueAxis *axisY = new QValueAxis();
    axisY->setLabelsFont(f1);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    chart->legend()->setVisible(true);
    chart->legend()->setFont(f);
    chart->legend()->setAlignment(Qt::AlignRight);
    axisY->setTitleText("Доля правил, попавших в диапазон");
    axisY->setTitleFont(f);
    axisY->applyNiceNumbers();
    axisY->setMax(1.);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->setLabelsFont(f1);
    axisX->append(_labels);
    axisX->setTitleText("Диапазоны значений поддержки и достоверности");
    axisX->setTitleFont(f);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    _view->setChart(chart);
    _view->setWindowTitle(_cur);
    _view->show();
    QScreen* screen = QApplication::screens().at(0);
    QSize size = screen->availableSize();
    _view->resize(size);
    auto p =_view->grab();
    p.save(_cur+".png");
}

void RuleAnalysis::on_action_4_triggered()
{
    on_pushButton_4_clicked();
}
