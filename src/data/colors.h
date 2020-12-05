#ifndef COLORS_H
#define COLORS_H

#include <QColor>

#include <array>
#include <limits>

namespace colors {

constexpr Q_DECL_UNUSED QColor Neutral     = QColor{180, 180, 180};
constexpr Q_DECL_UNUSED QColor Green       = QColor{112, 199, 120};
constexpr Q_DECL_UNUSED QColor LightGreen  = QColor{200, 200, 120};
constexpr Q_DECL_UNUSED QColor Yellow      = QColor{255, 200, 120};
constexpr Q_DECL_UNUSED QColor Orange      = QColor{255, 171, 0};
constexpr Q_DECL_UNUSED QColor LightRed    = QColor{238, 90, 24};
constexpr Q_DECL_UNUSED QColor Red         = QColor{183, 55, 68};
constexpr Q_DECL_UNUSED QColor DarkRed     = QColor{112, 39, 23};
constexpr Q_DECL_UNUSED QColor Magenta     = QColor{255, 0, 255};

struct ColorRange {
    float min;
    float max;
    QColor color;
};

constexpr Q_DECL_UNUSED std::array<ColorRange, 9> ColorRanges{
    ColorRange{std::numeric_limits<float>::min(), 0.0f, Neutral},
    ColorRange{0.0f, 5.0f, Green},
    ColorRange{5.0f, 20.0f, LightGreen},
    ColorRange{20.0f, 35.0f, Yellow},
    ColorRange{35.0f, 50.0f, Orange},
    ColorRange{50.0f, 100.0f, LightRed},
    ColorRange{100.0f, 250.0f, Red},
    ColorRange{250.0f, 500.0f, DarkRed},
    ColorRange{500.0f, std::numeric_limits<float>::max(), Magenta}
};

}

#endif // COLORS_H
