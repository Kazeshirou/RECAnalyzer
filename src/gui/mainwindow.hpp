#pragma once

#include <QList>
#include <QMainWindow>
#include <analyzer.hpp>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

class CaseWindow;


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() {}

    void openEaf(const QString& fileName);
    void openTemplate(const QString& fileName);
    void openTransactions(const QString& fileName);
    void openSets(const QString& fileName);
    void openRules(const QString& fileName);
    void openClustering(const QString& fileName);

    void runAnalysis(const QString& filename);

public slots:
    void chooseEaf();
    void chooseTemplate();
    void chooseTransactions();
    void chooseSets();
    void chooseRules();
    void chooseClustering();
    void chooseCfg();
    void showAboutBox();

private:
    Analyzer           analyzer_;
    QAction*           openAction;
    QString            currentPath_;
    QList<CaseWindow*> windows_;
};
