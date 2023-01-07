// 2016

//! \file

#ifndef HISTCHART_HPP_dlkfhcg47tshx38tgh34gbx3n87gf
#define HISTCHART_HPP_dlkfhcg47tshx38tgh34gbx3n87gf

#include "chart_axes.hpp"
#include "text.hpp"
#include "../simple_svg_1.0.0.hpp"
#include <string>
#include <vector>

namespace svg::ext
{
struct HistChartSetup
{
    Dimensions margin = Dimensions();
    double scale = 1.0;
};

class HistChart : public Shape
{
public:
    explicit HistChart(const HistChartSetup& setup = HistChartSetup())
        : setup(setup)
    {
    }

    HistChart& operator<<(const Rectangle& rectangle)
    {
        rectangles.push_back(rectangle);
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
        for (const auto& rectangle : rectangles)
            ret += rectangle.toString(layout);

        ret += this->coordsys.toString(layout);

        return ret;
    }

    void offset(const Point& offset) override
    {
        for (auto& rectangle : rectangles)
            rectangle.offset(offset);
        coordsys.offset(offset);
    }

private:
    HistChartSetup setup;
    std::vector<Rectangle> rectangles;
    ChartCoordSys coordsys;
};
} // namespace svg::ext

#endif
