#pragma once

#include <QMainWindow>
#include <rec_template.hpp>

class RecEntryModel;
QT_BEGIN_NAMESPACE
class QAction;
class QTableView;
class QEvent;
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

    void openEaf(const QString& fileName);

public slots:
    void chooseEaf();
    void showAboutBox();
    void updateView();

private:
    rec::rec_template_t recTemplate_;
    RecEntryModel*      model;
    QAction*            openAction;
    QString             currentPath;
    QTableView*         view;
};
