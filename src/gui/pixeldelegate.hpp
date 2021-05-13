#pragma once

#include <QAbstractItemDelegate>
#include <QModelIndex>
#include <QSize>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QObject;
class QPainter;
QT_END_NAMESPACE

static constexpr int ItemSize = 256;


class PixelDelegate : public QAbstractItemDelegate {
    Q_OBJECT

public:
    PixelDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex&          index) const override;

signals:
    void setStatusTip(const QString& tip, int timeout = 5000);

public slots:
    void setOneTimeSlotSize(int size);
    void setPixelSize(int size);

protected:
    bool helpEvent(QHelpEvent* event, QAbstractItemView* view,
                   const QStyleOptionViewItem& option,
                   const QModelIndex&          index) override;

private:
    int pixelSize_{12};
    int oneTimeSlotSize_{10};
};
