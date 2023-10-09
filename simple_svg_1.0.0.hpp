/*******************************************************************************
*  The "New BSD License" : http://www.opensource.org/licenses/bsd-license.php  *
********************************************************************************

Copyright (c) 2010, Mark Turney
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/

#ifndef SIMPLE_SVG_HPP
#define SIMPLE_SVG_HPP

#include <utility>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <optional>

#include <iostream>

namespace svg
{
    // Utility XML/String Functions.
    template <typename T>
    std::string attribute(std::string const & attribute_name,
        T const & value, std::string const & unit = "")
    {
        std::stringstream ss;
        ss << attribute_name << "=\"" << value << unit << "\" ";
        return ss.str();
    }
    inline std::string elemStart(std::string const & element_name)
    {
        return "    <" + element_name + " ";
    }
    inline std::string elemEnd(std::string const & element_name)
    {
        return "</" + element_name + ">\n";
    }
    inline std::string emptyElemEnd()
    {
        return "/>\n";
    }

    struct Dimensions
    {
        Dimensions(double width, double height) : width(width), height(height) { }
        Dimensions(double combined = 0) : width(combined), height(combined) { }
        double width;
        double height;
    };

    struct Point
    {
        explicit Point(double x = 0, double y = 0) : x(x), y(y) { }
        double x;
        double y;
    };
    inline std::optional<Point> getMinPoint(std::vector<Point> const & points)
    {
        if (points.empty())
            return {};

        Point min = points[0];
        for (auto point : points) {
            if (point.x < min.x)
                min.x = point.x;
            if (point.y < min.y)
                min.y = point.y;
        }
        return {min};
    }
    inline std::optional<Point> getMaxPoint(std::vector<Point> const & points)
    {
        if (points.empty())
            return {};

        Point max = points[0];
        for (auto point : points) {
            if (point.x > max.x)
                max.x = point.x;
            if (point.y > max.y)
                max.y = point.y;
        }
        return {max};
    }

    // Defines the dimensions, scale, origin, and origin offset of the document.
    struct Layout
    {
        enum Origin { TopLeft, BottomLeft, TopRight, BottomRight };

        explicit Layout(Dimensions const & dimensions = Dimensions(400, 300), Origin origin = BottomLeft,
            double scale = 1, Point const & origin_offset = Point(0, 0))
            : dimensions(dimensions), scale(scale), origin(origin), origin_offset(origin_offset) { }
        Dimensions dimensions;
        double scale;
        Origin origin;
        Point origin_offset;
    };

    // Convert coordinates in user space to SVG native space.
    inline double translateX(double x, Layout const & layout)
    {
        if (layout.origin == Layout::BottomRight || layout.origin == Layout::TopRight)
            return layout.dimensions.width - ((x + layout.origin_offset.x) * layout.scale);
        else
            return (layout.origin_offset.x + x) * layout.scale;
    }

    inline double translateY(double y, Layout const & layout)
    {
        if (layout.origin == Layout::BottomLeft || layout.origin == Layout::BottomRight)
            return layout.dimensions.height - ((y + layout.origin_offset.y) * layout.scale);
        else
            return (layout.origin_offset.y + y) * layout.scale;
    }
    inline double translateScale(double dimension, Layout const & layout)
    {
        return dimension * layout.scale;
    }

    class Serializeable
    {
    public:
        Serializeable() = default;
        virtual ~Serializeable() = default;
        virtual std::string toString(Layout const & layout) const = 0;
    };

    class Color : public Serializeable
    {
    public:
        enum Defaults { Transparent = -1, Aqua, Black, Blue, Brown, Cyan, Fuchsia,
            Green, Lime, Magenta, Orange, Purple, Red, Silver, White, Yellow };

        Color(int r, int g, int b) : transparent(false), red(r), green(g), blue(b) { }
        Color(Defaults color)
            : transparent(false), red(0), green(0), blue(0)
        {
            switch (color)
            {
                case Aqua: assign(0, 255, 255); break;
                case Black: assign(0, 0, 0); break;
                case Blue: assign(0, 0, 255); break;
                case Brown: assign(165, 42, 42); break;
                case Cyan: assign(0, 255, 255); break;
                case Fuchsia: assign(255, 0, 255); break;
                case Green: assign(0, 128, 0); break;
                case Lime: assign(0, 255, 0); break;
                case Magenta: assign(255, 0, 255); break;
                case Orange: assign(255, 165, 0); break;
                case Purple: assign(128, 0, 128); break;
                case Red: assign(255, 0, 0); break;
                case Silver: assign(192, 192, 192); break;
                case White: assign(255, 255, 255); break;
                case Yellow: assign(255, 255, 0); break;
                default: transparent = true; break;
            }
        }
        ~Color() override = default;
        std::string toString(Layout const &) const override
        {
            std::stringstream ss;
            if (transparent)
                ss << "none";
            else
                ss << "rgb(" << red << "," << green << "," << blue << ")";
            return ss.str();
        }
    private:
            bool transparent;
            int red;
            int green;
            int blue;

            void assign(int r, int g, int b)
            {
                red = r;
                green = g;
                blue = b;
            }
    };

    class Fill : public Serializeable
    {
    public:
        Fill(Color::Defaults color) : color(color) { }
		Fill(Color color = Color::Transparent)
            : color(color) { }
        std::string toString(Layout const & layout) const override
        {
            std::stringstream ss;
            ss << attribute("fill", color.toString(layout));
            return ss.str();
        }
    private:
        Color color;
    };

    class Stroke : public Serializeable
    {
    public:
        explicit Stroke(double width = -1, Color color = Color::Transparent, bool nonScalingStroke = false)
            : width(width), color(color), nonScaling(nonScalingStroke) { }
        std::string toString(Layout const & layout) const override
        {
            // If stroke width is invalid.
            if (width < 0)
                return {};

            std::stringstream ss;
            ss << attribute("stroke-width", translateScale(width, layout)) << attribute("stroke", color.toString(layout));
            if (nonScaling)
               ss << attribute("vector-effect", "non-scaling-stroke");
            return ss.str();
        }
		double getWidth() const { return this->width; }
    private:
        double width;
        Color color;
        bool nonScaling;
    };

    class Font : public Serializeable
    {
    public:
        explicit Font(double size = 12, std::string family = "Verdana") : size(size), family(std::move(family)) { }
        std::string toString(Layout const & layout) const override
        {
            std::stringstream ss;
            ss << attribute("font-size", translateScale(size, layout)) << attribute("font-family", family);
            return ss.str();
        }
    private:
        double size;
        std::string family;
    };

    class Shape : public Serializeable
    {
    public:
        explicit Shape(Fill fill = Fill(), Stroke stroke = Stroke())
            : fill(std::move(fill)), stroke(std::move(stroke)) { }
        ~Shape() override = default;
        std::string toString(Layout const & layout) const override = 0;
        virtual void offset(Point const & offset) = 0;
    protected:
        Fill fill;
        Stroke stroke;
    };
    template <typename T>
    std::string vectorToString(std::vector<T> collection, Layout const & layout)
    {
        std::string combination_str;
        for (unsigned i = 0; i < collection.size(); ++i)
            combination_str += collection[i].toString(layout);

        return combination_str;
    }

    class Circle : public Shape
    {
    public:
        Circle(Point const & center, double diameter, Fill const & fill,
            Stroke const & stroke = Stroke())
            : Shape(fill, stroke), center(center), radius(diameter / 2) { }
        std::string toString(Layout const & layout) const override
        {
            std::stringstream ss;
            ss << elemStart("circle") << attribute("cx", translateX(center.x, layout))
                << attribute("cy", translateY(center.y, layout))
                << attribute("r", translateScale(radius, layout)) << fill.toString(layout)
                << stroke.toString(layout) << emptyElemEnd();
            return ss.str();
        }
        void offset(Point const & offset) override
        {
            center.x += offset.x;
            center.y += offset.y;
        }
    private:
        Point center;
        double radius;
    };

    class Elipse : public Shape
    {
    public:
        Elipse(Point const & center, double width, double height,
            Fill const & fill = Fill(), Stroke const & stroke = Stroke())
            : Shape(fill, stroke), center(center), radius_width(width / 2),
            radius_height(height / 2) { }
        std::string toString(Layout const & layout) const override
        {
            std::stringstream ss;
            ss << elemStart("ellipse") << attribute("cx", translateX(center.x, layout))
                << attribute("cy", translateY(center.y, layout))
                << attribute("rx", translateScale(radius_width, layout))
                << attribute("ry", translateScale(radius_height, layout))
                << fill.toString(layout) << stroke.toString(layout) << emptyElemEnd();
            return ss.str();
        }
        void offset(Point const & offset) override
        {
            center.x += offset.x;
            center.y += offset.y;
        }
    private:
        Point center;
        double radius_width;
        double radius_height;
    };

    class Rectangle : public Shape
    {
    public:
        Rectangle(Point const & edge, double width, double height,
            Fill const & fill = Fill(), Stroke const & stroke = Stroke())
            : Shape(fill, stroke), edge(edge), width(width),
            height(height) { }
        std::string toString(Layout const & layout) const override
        {
            std::stringstream ss;
            ss << elemStart("rect") << attribute("x", translateX(edge.x, layout))
                << attribute("y", translateY(edge.y, layout))
                << attribute("width", translateScale(width, layout))
                << attribute("height", translateScale(height, layout))
                << fill.toString(layout) << stroke.toString(layout) << emptyElemEnd();
            return ss.str();
        }
        void offset(Point const & offset) override
        {
            edge.x += offset.x;
            edge.y += offset.y;
        }
    private:
        Point edge;
        double width;
        double height;
    };

    class Line : public Shape
    {
    public:
        Line(Point const & start_point, Point const & end_point,
            Stroke const & stroke = Stroke())
            : Shape(Fill(), stroke), start_point(start_point),
            end_point(end_point) { }
        std::string toString(Layout const & layout) const override
        {
            std::stringstream ss;
            ss << elemStart("line") << attribute("x1", translateX(start_point.x, layout))
                << attribute("y1", translateY(start_point.y, layout))
                << attribute("x2", translateX(end_point.x, layout))
                << attribute("y2", translateY(end_point.y, layout))
                << stroke.toString(layout) << emptyElemEnd();
            return ss.str();
        }
        void offset(Point const & offset) override
        {
            start_point.x += offset.x;
            start_point.y += offset.y;

            end_point.x += offset.x;
            end_point.y += offset.y;
        }
    private:
        Point start_point;
        Point end_point;
    };

    class Polygon : public Shape
    {
    public:
        explicit Polygon(Fill const & fill = Fill(), Stroke const & stroke = Stroke())
            : Shape(fill, stroke) { }
		Polygon(Stroke const & stroke = Stroke()) : Shape(Color::Transparent, stroke) { }
        Polygon & operator<<(Point const & point)
        {
            points.push_back(point);
            return *this;
        }
        std::string toString(Layout const & layout) const override
        {
            std::stringstream ss;
            ss << elemStart("polygon");

            ss << "points=\"";
            for (auto point : points)
                ss << translateX(point.x, layout) << "," << translateY(point.y, layout) << " ";
            ss << "\" ";

            ss << fill.toString(layout) << stroke.toString(layout) << emptyElemEnd();
            return ss.str();
        }
        void offset(Point const & offset) override
        {
            for (auto & point : points) {
                point.x += offset.x;
                point.y += offset.y;
            }
        }
    private:
        std::vector<Point> points;
    };

    class Path : public Shape
    {
    public:
       explicit Path(Fill const & fill = Fill(), Stroke const & stroke = Stroke())
          : Shape(fill, stroke) 
       {  startNewSubPath(); }
       explicit Path(Stroke const & stroke = Stroke()) : Shape(Color::Transparent, stroke)
       {  startNewSubPath(); }
       Path & operator<<(Point const & point)
       {
          paths.back().push_back(point);
          return *this;
       }

       void startNewSubPath()
       {
          if (paths.empty() || !paths.back().empty())
            paths.emplace_back();
       }

       std::string toString(Layout const & layout) const override
       {
          std::stringstream ss;
          ss << elemStart("path");

          ss << "d=\"";
          for (auto const& subpath: paths)
          {
             if (subpath.empty())
                continue;

             ss << "M";
             for (auto const& point: subpath)
                ss << translateX(point.x, layout) << "," << translateY(point.y, layout) << " ";
             ss << "z ";
          }
          ss << "\" ";
          ss << "fill-rule=\"evenodd\" ";

          ss << fill.toString(layout) << stroke.toString(layout) << emptyElemEnd();
          return ss.str();
       }

       void offset(Point const & offset) override
       {
          for (auto& subpath : paths)
             for (auto& point : subpath)
             {
                point.x += offset.x;
                point.y += offset.y;
             }
       }
    private:
       std::vector<std::vector<Point>> paths;
    };

    class Polyline : public Shape
    {
    public:
        explicit Polyline(Fill const & fill, Stroke const & stroke = Stroke())
            : Shape(fill, stroke) { }
		explicit Polyline(Stroke const & stroke = Stroke()) : Shape(Color::Transparent, stroke) { }
		explicit Polyline(std::vector<Point> const & points,
            Fill const & fill = Fill(), Stroke const & stroke = Stroke())
            : Shape(fill, stroke), points(points) { }
        Polyline & operator<<(Point const & point)
        {
            points.push_back(point);
            return *this;
        }
        std::string toString(Layout const & layout) const override
        {
            std::stringstream ss;
            ss << elemStart("polyline");

            ss << "points=\"";
            for (auto point : points)
                ss << translateX(point.x, layout) << "," << translateY(point.y, layout) << " ";
            ss << "\" ";

            ss << fill.toString(layout) << stroke.toString(layout) << emptyElemEnd();
            return ss.str();
        }
        void offset(Point const & offset) override
        {
            for (auto & point : points) {
                point.x += offset.x;
                point.y += offset.y;
            }
        }
        std::vector<Point> points;
    };

    class Text : public Shape
    {
    public:
        Text(Point const & origin, std::string content, Fill const & fill = Fill(),
             Font font = Font(), Stroke const & stroke = Stroke())
            : Shape(fill, stroke), origin(origin), content(std::move(content)), font(std::move(font)) { }
        std::string toString(Layout const & layout) const override
        {
            std::stringstream ss;
            ss << elemStart("text") << attribute("x", translateX(origin.x, layout))
                << attribute("y", translateY(origin.y, layout))
                << fill.toString(layout) << stroke.toString(layout) << font.toString(layout)
                << ">" << content << elemEnd("text");
            return ss.str();
        }
        void offset(Point const & offset) override
        {
            origin.x += offset.x;
            origin.y += offset.y;
        }
    private:
        Point origin;
        std::string content;
        Font font;
    };

    // Sample charting class.
    class LineChart : public Shape
    {
    public:
        explicit LineChart(Dimensions margin = Dimensions(),
                  Stroke axis_stroke = Stroke(.5, Color::Purple))
            : axis_stroke(std::move(axis_stroke)), margin(margin) { }
        LineChart & operator<<(Polyline const & polyline)
        {
            if (polyline.points.empty())
                return *this;

            polylines.push_back(polyline);
            return *this;
        }
        std::string toString(Layout const & layout) const override
        {
            if (polylines.empty())
                return "";

            std::string ret;
            for (const auto & polyline : polylines)
                ret += polylineToString(polyline, layout);

            return ret + axisString(layout);
        }
        void offset(Point const & offset) override
        {
            for (auto & polyline : polylines)
                polyline.offset(offset);
        }
    private:
        Stroke axis_stroke;
        Dimensions margin;
        std::vector<Polyline> polylines;

        std::optional<Dimensions> getDimensions() const
        {
            if (polylines.empty())
                return {};

            std::optional<Point> min = getMinPoint(polylines[0].points);
            std::optional<Point> max = getMaxPoint(polylines[0].points);
            for (const auto & polyline : polylines) {
                if (getMinPoint(polyline.points)->x < min->x)
                    min->x = getMinPoint(polyline.points)->x;
                if (getMinPoint(polyline.points)->y < min->y)
                    min->y = getMinPoint(polyline.points)->y;
                if (getMaxPoint(polyline.points)->x > max->x)
                    max->x = getMaxPoint(polyline.points)->x;
                if (getMaxPoint(polyline.points)->y > max->y)
                    max->y = getMaxPoint(polyline.points)->y;
            }

            return {Dimensions(max->x - min->x, max->y - min->y)};
        }
        std::string axisString(Layout const & layout) const
        {
            std::optional<Dimensions> dimensions = getDimensions();
            if (!dimensions)
                return "";

            // Make the axis 10% wider and higher than the data points.
            double width = dimensions->width * 1.1;
            double height = dimensions->height * 1.1;

            // Draw the axis.
            Polyline axis(Color::Transparent, axis_stroke);
            axis << Point(margin.width, margin.height + height) << Point(margin.width, margin.height)
                << Point(margin.width + width, margin.height);

            return axis.toString(layout);
        }
        std::string polylineToString(Polyline const & polyline, Layout const & layout, bool withVertices = true) const
        {
            Polyline shifted_polyline = polyline;
            shifted_polyline.offset(Point(margin.width, margin.height));

            if (withVertices) {
                std::vector<Circle> vertices;
                for (auto & point : shifted_polyline.points)
                    vertices.emplace_back(point, getDimensions()->height / 100.0, Color::Black);
                return shifted_polyline.toString(layout) + vectorToString(vertices, layout);
            } else {
                return shifted_polyline.toString(layout);
            }

        }
    };

    class Document
    {
    public:
        Document() = default;
        explicit Document(std::string file_name, Layout layout = Layout())
            : file_name(std::move(file_name)), layout(layout) { }

        Document & operator<<(Shape const & shape)
        {
            body_nodes_str_list.push_back(shape.toString(layout));
            return *this;
        }
        std::string toString() const
        {
            std::stringstream ss;
            writeToStream(ss);
            return ss.str();
        }
        bool save() const
        {
            std::ofstream ofs(file_name.c_str());
            if (!ofs.good())
                return false;

            writeToStream(ofs);
            ofs.close();
            return true;
        }
    private:
        void writeToStream(std::ostream& str) const
        {
            str << "<?xml " << attribute("version", "1.0") << attribute("standalone", "no")
                << "?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
                << "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n<svg "
                << attribute("width", layout.dimensions.width, "px")
                << attribute("height", layout.dimensions.height, "px")
                << attribute("xmlns", "http://www.w3.org/2000/svg")
                << attribute("version", "1.1") << ">\n";
            for (const auto& body_node_str : body_nodes_str_list) {
                str << body_node_str;
            }
            str << elemEnd("svg");
        }

    private:
        std::string file_name;
        Layout layout;

        std::vector<std::string> body_nodes_str_list;
    };
}

#endif
