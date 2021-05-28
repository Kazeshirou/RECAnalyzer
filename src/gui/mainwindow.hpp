#pragma once

#include <QMainWindow>

#include <rec_template.hpp>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() {}

    void openEaf(const QString& fileName);

public slots:
    void chooseEaf();
    void showAboutBox();

private:
    rec::rec_template_t recTemplate_;
    QAction*            openAction;
    QString             currentPath;
};
