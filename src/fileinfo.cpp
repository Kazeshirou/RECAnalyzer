#include "fileinfo.h"
#include "ui_fileinfo.h"

#include "visualizator.h"

FileInfo::FileInfo(QStringList tiers, AnnotationFile *af, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FileInfo),
    _visualizator(new Visualizator())
{
    connect(_visualizator, &Visualizator::mouseMoved, this, &FileInfo::cursorMoved);
    ui->setupUi(this);
    setWindowTitle(af->_filename);
    ui->view->setScene(_visualizator);
    _visualizator->setAnnotationFile(af);
    _visualizator->setActiveTiers(tiers);

}

FileInfo::~FileInfo()
{
    delete ui;
}

void FileInfo::cursorMoved(double x, QString tier)
{
    statusBar()->showMessage(QString("%1 %2; %3").arg(x).arg("ms").arg(tier));
}
