#include "annotationsviewsettings.h"

AnnotationsViewSettings::AnnotationsViewSettings(
    rec::rec_template_t& recTemplate, QObject* parent)
        : QObject(parent), recTemplate_{recTemplate} {
    generateColorPalette();
    hidden_ = QVector<bool>(recTemplate.annotations.size(), false);
    colors_.resize(recTemplate.annotations.size());
    for (size_t i{0}; i < recTemplate.annotations.size(); i++) {
        colors_[i] = colorPalette_[recTemplate.annotations[i].tier];
    }
}

void AnnotationsViewSettings::generateColorPalette() {
    colorPalette_.clear();
    double h           = 0;
    double goldenRatio = 0.618033988749895;
    size_t colorCount  = recTemplate_.tiers.size();
    for (size_t i{0}; i < colorCount; i++) {
        h = goldenRatio * 360 / colorCount * i;
        colorPalette_.append(QBrush(QColor::fromHsv(int(h), 245, 245, 255)));
    }
}
