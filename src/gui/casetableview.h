#ifndef CASETABLEVIEW_H
#define CASETABLEVIEW_H

#include <QTableView>

class CaseTableView : public QTableView {
    Q_OBJECT
public:
    explicit CaseTableView(QWidget* parent = nullptr);

public slots:
    void hiddenChange(size_t i, bool value);

protected:
};

#endif  // CASETABLEVIEW_H
