#pragma once

#include <QMainWindow>
#include <rec_entry.hpp>
#include <rec_template.hpp>

class CaseWidget;
QT_BEGIN_NAMESPACE
class QAction;
class QEvent;
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() {
        if (recEntry_) {
            delete recEntry_;
        }
    }

    void openEaf(const QString& fileName);

public slots:
    void chooseEaf();
    void showAboutBox();

private:
    rec::rec_template_t recTemplate_;
    rec::rec_entry_t*   recEntry_{nullptr};
    QAction*            openAction;
    QString             currentPath;
    CaseWidget*         caseWidget;
};
