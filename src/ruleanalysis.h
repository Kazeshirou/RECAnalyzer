#ifndef RULEANALYSIS_H
#define RULEANALYSIS_H

#include <QMainWindow>

#include <QtCharts>
#include <QChart>

namespace Ui {
class RuleAnalysis;
}

class RuleAnalysis : public QMainWindow
{
    Q_OBJECT

public:
    explicit RuleAnalysis(QString filename, QWidget *parent = nullptr);
    void parse();
    void parse(QString filename);
    ~RuleAnalysis();

private slots:

    void on_action_triggered();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_action_3_triggered();

    void on_action_2_triggered();

    void on_action_4_triggered();

private:
    Ui::RuleAnalysis *ui;
    QString out;
    QVector<double> _supps;
    QVector<double> _confs;
    QStringList _labels;
    QString _cur;
    QChartView *_view;
    static int num;
    static QString name;
};

#endif // RULEANALYSIS_H
