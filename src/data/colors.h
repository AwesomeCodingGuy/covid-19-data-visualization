#ifndef COLORS_H
#define COLORS_H

#include <QColor>

#include <array>
#include <limits>

namespace colors {

// Incidence
constexpr Q_DECL_UNUSED QColor Neutral      = QColor{180, 180, 180};
constexpr Q_DECL_UNUSED QColor Green        = QColor{112, 199, 120};
constexpr Q_DECL_UNUSED QColor LightGreen   = QColor{200, 200, 120};
constexpr Q_DECL_UNUSED QColor Yellow       = QColor{255, 200, 120};
constexpr Q_DECL_UNUSED QColor Orange       = QColor{255, 171,   0};
constexpr Q_DECL_UNUSED QColor LightRed     = QColor{238,  90,  24};
constexpr Q_DECL_UNUSED QColor Red          = QColor{183,  55,  68};
constexpr Q_DECL_UNUSED QColor DarkRed      = QColor{112,  39,  23};
constexpr Q_DECL_UNUSED QColor Magenta      = QColor{210,  0,  255};

// More colors
constexpr Q_DECL_UNUSED QColor Black        = QColor{0,     0,   0};
constexpr Q_DECL_UNUSED QColor DarkGray     = QColor{32,   32,  32};
constexpr Q_DECL_UNUSED QColor MapText      = QColor{255, 255, 255};
constexpr Q_DECL_UNUSED QColor MapBg        = QColor{101, 102, 203};

// Chart colors
constexpr Q_DECL_UNUSED std::array<QColor, 17> ChartColors{
    QColor{0,     0,   0},
    QColor{255, 127,   0},
    QColor{0,     0, 131},
    QColor{0,   145,   0},
    QColor{210,   0,   0},
    QColor{243, 200,   0},
    QColor{86,   84,  81},
    QColor{255,   0, 247},
    QColor{112, 228,   0},
    QColor{0,   106, 255},
    QColor{59,  246, 255},
    QColor{89,    7, 255},
    QColor{48,   92,   0},
    QColor{254,   0,  69},
    QColor{185, 185, 185},
    QColor{161, 128,   0},
    QColor{99,    0,  51}
};

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
