// 2016

//! \file

#ifndef CHART_AXES_HPP_sdsfijgch49tz753
#define CHART_AXES_HPP_sdsfijgch49tz753

#include "text.hpp"
#include "../simple_svg_1.0.0.hpp"
#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace svg::ext {
using PhysicalValue = double;

struct AxisCaptions {
    PhysicalValue from = 0.0;
    PhysicalValue step = 0.0;
    std::string quantity;
    std::string unit;
};

using ScaleTickCount = long;

struct XAxis {
    double posY = 0.0;
    double leftmostScaleTick = 0.0;
    double scaleTickStep = 0.0;
    ScaleTickCount count = 0;
    AxisCaptions physicalCaptions;
};

struct YAxis {
    double posX = 0.0;
    double lowermostScaleTick = 0.0;
    double scaleTickStep = 0.0;
    ScaleTickCount count = 0;
    AxisCaptions physicalCaptions;
};

class ChartCoordSys : public Shape {
public:
    ChartCoordSys() = default;

    ChartCoordSys(const Stroke& s, XAxis xa, YAxis ya)
        : Shape(Fill(), s)
        , xa(std::move(xa))
        , ya(std::move(ya))
        , axis_stroke(s) {
    }

    [[nodiscard]] std::string toString(const Layout& layout) const override {
        std::string ret;

        std::vector<Polyline> CoordSysLines;
        toPolylines(CoordSysLines);
        for (const auto& line : CoordSysLines)
            ret += polylineToString(line, layout);

        std::vector<Text_utf8> CoordSysTexts;
        toTexts(CoordSysTexts);
        for (const auto& txt : CoordSysTexts)
            ret += textToString(txt, layout);

        return ret;
    }

    void offset(const Point& offset) override {
        this->margin_offset = offset;
    }

private:
    XAxis xa;
    YAxis ya;
    Stroke axis_stroke{0.5, Color::Black};
    Point margin_offset;

    [[nodiscard]] std::string polylineToString(const Polyline& polyline, const Layout& layout) const {
        Polyline shifted_polyline = polyline;
        shifted_polyline.offset(this->margin_offset);

        return shifted_polyline.toString(layout);
    }

    [[nodiscard]] std::string textToString(const Text_utf8& text, const Layout& layout) const {
        Text_utf8 shifted_text = text;
        shifted_text.offset(this->margin_offset);

        return shifted_text.toString(layout);
    }

    void toPolylines(std::vector<Polyline>& retLines) const {
        retLines.resize(static_cast<size_t>(2 + xa.count + ya.count));
        const Color fillCol = Color::Transparent;

        Polyline X(fillCol, axis_stroke);
        X << Point(xa.leftmostScaleTick, xa.posY)
          << Point(xa.leftmostScaleTick + (static_cast<double>(xa.count + 1)) * xa.scaleTickStep, xa.posY);
        retLines[0] = X;

        Polyline Y(fillCol, axis_stroke);
        Y << Point(ya.posX, ya.lowermostScaleTick)
          << Point(ya.posX, ya.lowermostScaleTick + (static_cast<double>(ya.count + 1)) * ya.scaleTickStep);
        retLines[1] = Y;

        const auto axis_stroke_width_timesX = axis_stroke.getWidth() * 3.0;

        const auto xticktop = xa.posY + axis_stroke_width_timesX;
        const auto xtickbottom = xa.posY - axis_stroke_width_timesX;
        for (ScaleTickCount i = 1; i <= xa.count; ++i) {
            Polyline xTick(fillCol, axis_stroke);
            const auto x = xa.leftmostScaleTick + static_cast<double>(i) * xa.scaleTickStep;
            xTick << Point(x, xticktop) << Point(x, xtickbottom);
            retLines[static_cast<size_t>(1 + i)] = xTick;
        }

        const auto ytickleft = ya.posX - axis_stroke_width_timesX;
        const auto ytickright = ya.posX + axis_stroke_width_timesX;
        for (ScaleTickCount i = 1; i <= ya.count; ++i) {
            Polyline yTick(fillCol, axis_stroke);
            const auto y = ya.lowermostScaleTick + static_cast<double>(i) * ya.scaleTickStep;
            yTick << Point(ytickleft, y) << Point(ytickright, y);
            retLines[static_cast<size_t>(1 + xa.count + i)] = yTick;
        }
    }

    void toTexts(std::vector<Text_utf8>& retTexts) const {
        // x-axis has 1, y-axis 2 text labels, then the scale tick number labels
        retTexts.reserve(static_cast<size_t>(1 + 2 + xa.count + ya.count));
        const Color txtColor(Color::Black);
        const double txtFontSize = 10.0;
        const Font txtFont(txtFontSize, "Calibri");
        const double tickFontSize = 10.0;
        const Font tickFont(tickFontSize, "Calibri");
        const auto axis_stroke_width_timesX = axis_stroke.getWidth() * 3.0;

        Text_utf8::Format txtformat;
        txtformat.color = txtColor;
        txtformat.font = txtFont;
        txtformat.anchor = TextAnchor::center;

        std::string xTxt = xa.physicalCaptions.quantity;
        if (!xa.physicalCaptions.unit.empty()) {
            xTxt += " / ";
            xTxt += xa.physicalCaptions.unit;
        }
        const Point posX(
            xa.leftmostScaleTick + (static_cast<double>(xa.count) + 0.25) * xa.scaleTickStep,
            xa.posY - txtFontSize - axis_stroke_width_timesX);
        Text_utf8 X(posX, xTxt, txtformat);
        retTexts.push_back(X);

        txtformat.anchor = TextAnchor::right;
        std::string yTxtQu = ya.physicalCaptions.quantity;
        std::string yTxtUnit;
        if (!ya.physicalCaptions.unit.empty())
            yTxtUnit = ya.physicalCaptions.unit;
        const Point posYQu(
            ya.posX - axis_stroke_width_timesX * 2.0,
            ya.lowermostScaleTick + (static_cast<double>(ya.count) + 0.5) * ya.scaleTickStep);
        Text_utf8 YQu(posYQu, yTxtQu, txtformat);
        retTexts.push_back(YQu);
        const Point posYUnit(
            ya.posX - axis_stroke_width_timesX * 2.0,
            ya.lowermostScaleTick + static_cast<double>(ya.count) * ya.scaleTickStep);
        Text_utf8 YUnit(posYUnit, yTxtUnit, txtformat);
        retTexts.push_back(YUnit);

        txtformat.anchor = TextAnchor::center;
        txtformat.font = tickFont;
        const auto xtick_ycoord = xa.posY - tickFontSize - axis_stroke_width_timesX;
        for (ScaleTickCount i = 0; i < xa.count; ++i) {
            const auto x = xa.leftmostScaleTick + static_cast<double>(i) * xa.scaleTickStep;
            const Point posTick(x, xtick_ycoord);
            std::stringstream ss;
            ss << xa.physicalCaptions.from + static_cast<PhysicalValue>(i) * xa.physicalCaptions.step;
            Text_utf8 xTick(posTick, ss.str(), txtformat);
            retTexts.push_back(xTick);
        }

        txtformat.anchor = TextAnchor::right;
        std::stringstream yTickTxtSample;
        yTickTxtSample << ya.physicalCaptions.step;
        const auto ytick_xcoord = ya.posX - axis_stroke_width_timesX * 2.0;
        for (ScaleTickCount i = 0; i < ya.count; ++i) {
            const auto y = ya.lowermostScaleTick + static_cast<double>(i) * ya.scaleTickStep;
            const Point posTick(ytick_xcoord, y);
            std::stringstream ss;
            ss << ya.physicalCaptions.from + static_cast<PhysicalValue>(i) * ya.physicalCaptions.step;
            Text_utf8 yTick(posTick, ss.str(), txtformat);
            retTexts.push_back(yTick);
        }
    }
};
} // namespace svg::ext

#endif
