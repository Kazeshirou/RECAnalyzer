#ifndef ANALISISRESULT_H
#define ANALISISRESULT_H

#include <QMainWindow>
#include <QtCharts>
#include <QChart>

namespace Ui {
class AnalisisResult;
}

class TransactionBuilder;

class SetAnalysis : public QMainWindow
{
    Q_OBJECT

public:
    explicit SetAnalysis(QString filename, QWidget *parent = nullptr);
    void parse();
    void parse(QString filename);
    ~SetAnalysis();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_action_triggered();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_action_4_triggered();

private:
    Ui::AnalisisResult *ui;
    QString out;
    QString in;
    QVector<double> _supps;
    QStringList _labels;
    QString _cur;
    QChartView *_view;
    static int num;
    static QString name;
};

#endif // ANALISISRESULT_H
