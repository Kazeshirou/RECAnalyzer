#include "pixeldelegate.hpp"

#include <QHelpEvent>
#include <QPainter>
#include <QToolTip>
#include <QWhatsThis>

PixelDelegate::PixelDelegate(QObject* parent) : QAbstractItemDelegate(parent) {}

void PixelDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const {
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }


    if (!index.model()->data(index, Qt::UserRole).toInt()) {
        return;
    }
    const double width  = option.rect.width();
    const double height = option.rect.height();

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setPen(Qt::NoPen);
    if (option.state & QStyle::State_Selected) {
        painter->setBrush(option.palette.highlightedText());
    } else {
        painter->setBrush(option.palette.text());
    }

    int xMargin = 1;
    int yMargin = 1;
    painter->drawRect(option.rect.x() + xMargin, option.rect.y() + yMargin,
                      width - 2 * xMargin, height - 2 * yMargin);
    painter->restore();
}

bool PixelDelegate::helpEvent(QHelpEvent* event, QAbstractItemView* view,
                              const QStyleOptionViewItem& option,
                              const QModelIndex&          index) {
    if (event->type() == QEvent::ToolTip) {
        if (event->type() == QEvent::ToolTip) {
            if (!index.data(Qt::StatusTipRole).isNull()) {
                emit setStatusTip(index.data(Qt::StatusTipRole).toString());
            }
            if (!index.data(Qt::ToolTipRole).isNull()) {
                QToolTip::showText(event->globalPos(),
                                   index.data(Qt::ToolTipRole).toString());
                return true;
            }
        }
    }

    return QAbstractItemDelegate::helpEvent(event, view, option, index);
}

QSize PixelDelegate::sizeHint(const QStyleOptionViewItem&,
                              const QModelIndex& index) const {
    //    return QSize(index.data(Qt::SizeHintRole).toSize().width() /
    //                     oneTimeSlotSize_ * pixelSize_,
    //                 pixelSize_);
    return QSize(pixelSize_, pixelSize_);
}

void PixelDelegate::setPixelSize(int size) {
    pixelSize_ = size;
}

void PixelDelegate::setOneTimeSlotSize(int size) {
    oneTimeSlotSize_ = size;
}
