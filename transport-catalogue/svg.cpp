#include "svg.h"

namespace svg {

    using namespace std::literals;

    std::ostream& operator<<(std::ostream& out, Color& color) {
        std::visit(ColorPrinter{ out }, color);
        return out;
    }

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }
    // ---------- PathProps ------------------

    std::ostream& operator<<(std::ostream& out, StrokeLineCap stroke_line_cap)
    {
        switch (stroke_line_cap) {
        case StrokeLineCap::BUTT:
            out << "butt";
            break;
        case StrokeLineCap::ROUND:
            out << "round";
            break;
        case StrokeLineCap::SQUARE:
            out << "square";
            break;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, StrokeLineJoin stroke_line_join)
    {
        switch (stroke_line_join) {
        case StrokeLineJoin::ARCS:
            out << "arcs";
            break;
        case StrokeLineJoin::BEVEL:
            out << "bevel";
            break;
        case StrokeLineJoin::MITER:
            out << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:
            out << "round";
            break;
        }
        return out;
    }


    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point)
    {
        points_.push_back({ point.x, point.y });
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        bool is_first = true;
        out << "<polyline points=\""sv;
        for (const auto& [x, y] : points_) {
            if (is_first) {
                out << x << ","sv << y;
                is_first = false;
            }
            else {
                out << " "sv << x << ","sv << y;
            }
        }
        out << "\" "sv;
        RenderAttrs(context.out);
        out << "/> "sv;
    }

    // ---------- Text ------------------

    Text& Text::SetPosition(Point pos)
    {
        position_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size)
    {
        size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family)
    {
        font_family_ = std::move(font_family);
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight)
    {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    Text& Text::SetData(std::string data)
    {
        data_ = std::move(data);
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<text "sv
            << "x=\""sv << position_.x << "\" "sv
            << "y=\""sv << position_.y << "\" "sv
            << "dx=\""sv << offset_.x << "\" "sv
            << "dy=\""sv << offset_.y << "\" "sv
            << "font-size=\""sv << size_ << "\" "sv;
        if (!font_family_.empty()) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        RenderAttrs(context.out);
        out << ">"sv << data_ << "</text>"sv;
    }

    // ---------- Document ------------------



    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.push_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const
    {
        RenderContext ctx(out, 2, 2);
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        for (const auto& obj : objects_) {
            obj->Render(ctx);
        }
        out << "</svg>"sv;
    }

}  // namespace svg

/*
<?xml version="1.0" encoding="UTF-8" ?>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1">
  <circle cx="50" cy="50" r="40" fill="grey" stroke="black"/>
  <circle cx="70" cy="70" r="30" fill="none" stroke="green"/>
  <circle cx="40" cy="40" r="30" fill="rgb(180,40,100)"/>
  <circle cx="100" cy="50" r="40" fill="rgba(255,255,0,0.5)"/>
</svg>
*/