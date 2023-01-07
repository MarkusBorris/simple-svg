// 2015-16

//! \file

#ifndef LINECHART_HPP_INCL_kjdfghnicuh5n48th5842tx5
#define LINECHART_HPP_INCL_kjdfghnicuh5n48th5842tx5

#include "chart_axes.hpp"
#include "text.hpp"
#include "../simple_svg_1.0.0.hpp"
#include <stdexcept>
#include <string>
#include <vector>

namespace svg::ext
{
struct LineChartSetup
{
    Dimensions margin = Dimensions();
    double scale = 1.0;
};

class LineChart : public Shape
{
public:
    explicit LineChart(const LineChartSetup& setup = LineChartSetup())
        : setup(setup)
    {
    }

    LineChart& operator<<(const Polyline& polyline)
    {
        if (polyline.points.empty())
            return *this;

        polylines.push_back(polyline);
        return *this;
    }

    void setCoords(const ChartCoordSys& coords)
    {
        this->coordsys = coords;
        this->coordsys.offset(Point(setup.margin.width, setup.margin.height));
    }

    [[nodiscard]] std::string toString(const Layout& layout) const override
    {
        std::string ret;
        for (const auto& polyline : polylines)
            ret += polylineToString(polyline, layout);

        ret += coordsys.toString(layout);

        return ret;
    }

    void offset(const Point& offset) override
    {
        for (auto& polyline : polylines)
            polyline.offset(offset);
        coordsys.offset(offset);
    }

private:
    LineChartSetup setup;
    std::vector<Polyline> polylines;
    ChartCoordSys coordsys;

    [[nodiscard]] std::string polylineToString(const Polyline& polyline, const Layout& layout) const
    {
        Polyline shifted_polyline = polyline;
        shifted_polyline.offset(Point(setup.margin.width, setup.margin.height));

        return shifted_polyline.toString(layout);
    }
};
} // namespace svg::ext

#endif
