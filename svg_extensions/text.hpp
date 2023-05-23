// 2016

//! \file

#ifndef TEXT_HPP_kldfhjvkijme587tc76xrgfbhw8fgx4
#define TEXT_HPP_kldfhjvkijme587tc76xrgfbhw8fgx4

#include "../simple_svg_1.0.0.hpp"
#include "ul/ul.h"
#include <string>

namespace svg::ext {
enum class TextAnchor {
    left,
    center,
    right,
};

inline std::string anchor_to_string(TextAnchor a) {
    switch (a) {
        case TextAnchor::left:
            return "start";
        case TextAnchor::center:
            return "middle";
        case TextAnchor::right:
            return "end";
        default:
            throw std::runtime_error("invalid TextAnchor");
    }
}

using TextColor = Color;

//! Original svg::Text does not work for UTF8 strings, it doesn't automatically translate to HTMLish UTF8 first.
//! This Text class does. It also provides functionality for text alignment and anything you like... just add to
//! the implementation on demand.
// couldn't derive from svg::Text - would have needed access to private members
class Text_utf8 : public Shape {
public:
    struct Format {
        Format() {
        } // MinGW needs this strange line (!?)

        //! Text color. (Not background.)
        TextColor color{Color::Black};
        Font font{};
        //! Outlining/boldness width. If having the same color as the text, this is also used to generate boldness.
        //! Default is none (a negative value).
        Stroke stroke{-1.0, Color::Black};
        TextAnchor anchor{TextAnchor::left};
    };

    //! Note that the origin point denotes the left and *bottom* edge of the first character to be drawn.
    Text_utf8(const Point& origin, const std::string& content, const Format& f = Format{})
        : Shape(Fill(f.color), f.stroke)
        , origin(origin)
        , content(ul::str::utf8_to_html(content))
        , format(f) {
    }

    [[nodiscard]] std::string toString(const Layout& layout) const override {
        const std::string text_elem = "text";
        std::stringstream ss;
        ss << elemStart(text_elem) << attribute("x", translateX(origin.x, layout))
           << attribute("y", translateY(origin.y, layout));
        if (format.anchor != TextAnchor::left)
            ss << attribute("text-anchor", anchor_to_string(format.anchor));
        ss << fill.toString(layout) << stroke.toString(layout) << format.font.toString(layout) << ">" << content
           << elemEnd(text_elem);
        return ss.str();
    }

    void offset(const Point& offset) override {
        origin.x += offset.x;
        origin.y += offset.y;
    }

private:
    Point origin;
    std::string content;
    Format format;
};
} // namespace svg::ext

#endif
