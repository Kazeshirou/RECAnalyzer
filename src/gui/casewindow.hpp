#ifndef CASEWINDOW_HPP
#define CASEWINDOW_HPP

#include <QMainWindow>

class CaseWidget;

namespace rec {
class rec_entry_t;
class rec_template_t;
}  // namespace rec

namespace mc {
class case_t;
}

class CaseWindow : public QMainWindow {
    Q_OBJECT

public:
    CaseWindow(QString filename, rec::rec_entry_t* recEntry,
               QWidget* parent = nullptr);
    CaseWindow(QString filename, rec::rec_template_t& recTemplate,
               mc::case_t* newCase, QWidget* parent = nullptr);

signals:
    void will_close(CaseWindow*);

public slots:
    void zoomIn();
    void zoomOut();

protected:
    void closeEvent(QCloseEvent* ev) override {
        QMainWindow::closeEvent(ev);
        emit will_close(this);
    }

private:
    CaseWidget* caseWidget_;
    int         zoom_{12};
};

#endif  // CASEWINDOW_HPP
