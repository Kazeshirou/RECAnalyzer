#ifndef CASEWINDOW_HPP
#define CASEWINDOW_HPP

#include <QMainWindow>

class CaseWidget;

namespace rec {
class rec_entry_t;
}  // namespace rec

class CaseWindow : public QMainWindow {
    Q_OBJECT

public:
    CaseWindow(QString filename, rec::rec_entry_t* recEntry,
               QWidget* parent = nullptr);
    ~CaseWindow();

public slots:
    void zoomIn();
    void zoomOut();

private:
    rec::rec_entry_t* recEntry_;
    CaseWidget*       caseWidget_;
    int               zoom_{12};
};

#endif  // CASEWINDOW_HPP
