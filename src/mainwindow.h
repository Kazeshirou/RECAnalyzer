#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class Visualizator;
class AnnotationDB;
class AnnotationFile;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setFiles(QStringList filenames);

private slots:
    void on_action_triggered();

//    void on_action_2_triggered();
//    void DBStatusChanged(bool c);

//    void on_action_3_triggered();
    void cursorMoved(double x, QString tier);

    void on_action_4_triggered();

    void on_pushButton_clicked();

    void on_open_clicked();

    void on_go_clicked();

    void fileChanged(QString filename);

    void closeEvent(QCloseEvent *ev);

    void on_go_2_clicked();

    void on_action_2_triggered();

    void on_pushButton_2_clicked();

    void on_action_3_triggered();

private:
    Ui::MainWindow *ui;
    Visualizator *_visualizator;
//    AnnotationDB *_db;
//    QLabel *_dbdisconnected, *_dbconnected;
//    bool _dbconnect;
    AnnotationFile *_af;
    QVector<QWidget *> _windows;
    QString _currentfile;
};

#endif // MAINWINDOW_H
