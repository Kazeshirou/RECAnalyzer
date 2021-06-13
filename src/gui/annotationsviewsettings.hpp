#pragma once

#include <QBrush>
#include <QObject>
#include <QVector>
#include <rec_template.hpp>

class AnnotationsViewSettings : public QObject {
    Q_OBJECT
public:
    explicit AnnotationsViewSettings(rec::rec_template_t& recTemplate,
                                     QObject*             parent = nullptr);


    rec::rec_template_t& recTemplate() {
        return recTemplate_;
    }

    const QVector<QBrush>& colors() const {
        return colors_;
    }


    const QBrush& color(size_t i) const {
        return colors_[i];
    }

    const QVector<bool>& hidden() const {
        return hidden_;
    }

    bool hidden(size_t i) const {
        return hidden_[i];
    }

    void setHidden(size_t i, bool value) {
        if (i < hidden_.size()) {
            hidden_[i] = value;
            emit hiddenChanged(i, value);
        }
    }

    void setColor(size_t i, QBrush color) {
        if (i < colors_.size()) {
            colors_[i] = color;
            emit colorChanged(i, color);
        }
    }

signals:
    void hiddenChanged(size_t, bool);
    void colorChanged(size_t, QBrush&);

protected:
    void generateColorPalette();

private:
    rec::rec_template_t& recTemplate_;
    QVector<bool>        hidden_;
    QVector<QBrush>      colors_;
    QVector<QBrush>      colorPalette_;
};
