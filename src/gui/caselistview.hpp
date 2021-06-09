#ifndef CASELISTVIEW_H
#define CASELISTVIEW_H

#include <QListView>


class CaseListView : public QListView {
    Q_OBJECT

public:
    CaseListView(QWidget* parent = nullptr);
};

#endif  // CASELISTVIEW_H
