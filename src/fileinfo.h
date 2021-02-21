#ifndef FILEINFO_H
#define FILEINFO_H

#include <QMainWindow>

class Visualizator;
class AnnotationFile;

namespace Ui {
class FileInfo;
}

class FileInfo : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileInfo(QStringList tiers, AnnotationFile *af, QWidget *parent = nullptr);
    ~FileInfo();

private slots:
    void cursorMoved(double x, QString tier);

private:
    Ui::FileInfo *ui;
    Visualizator *_visualizator;
};

#endif // FILEINFO_H
