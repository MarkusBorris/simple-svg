// 2016

//! \file

#ifndef SIMPLE_SVG_EXT_HPP_kjdsngiuxzhn4zu2ghx342zg3
#define SIMPLE_SVG_EXT_HPP_kjdsngiuxzhn4zu2ghx342zg3

#include "../simple_svg_1.0.0.hpp"

namespace svg
{
inline Dimensions operator-(Dimensions lhs, const Dimensions& rhs)
{
    return {lhs.width - rhs.width, lhs.height - rhs.height};
}
} // namespace svg

#endif
