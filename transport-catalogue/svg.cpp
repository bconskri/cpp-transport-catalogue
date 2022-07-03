#include "svg.h"
#include <map>

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext &context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

// ---------- Circle ------------------

    Circle &Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle &Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        // Выводим атрибуты, унаследованные от PathProps
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------
    Polyline &Polyline::AddPoint(Point point) {
        points_.emplace_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        out << "<polyline "sv;
        out << "points=\""sv;
        bool first = true;
        for (const auto point: points_) {
            if (first) {
                out << point.x << ","sv << point.y;
                first = false;
            } else {
                out << " "sv << point.x << ","sv << point.y;
            }

        }
        out << "\"";
        // Выводим атрибуты, унаследованные от PathProps
        RenderAttrs(context.out);
        out << " />"sv;
    }

    // ---------- Text ------------------
    Text &Text::SetPosition(Point pos) {
        coords_ = pos;
        return *this;
    }


    Text &Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }


    Text &Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }


    Text &Text::SetFontFamily(const std::string &font_family) {
        font_family_ = font_family;
        return *this;
    }


    Text &Text::SetData(const std::string &data) {
        text_ = data;
        return *this;
    }

    Text &Text::SetFontWeight(const std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    void Text::RenderObject(const RenderContext &context) const {
        auto &out = context.out;
        std::map<char, std::string> special_chars{
                {'\"', "&quot;"},
                {'\'', "&apos;"},
                {'<',  "&lt;"},
                {'>',  "&gt;"},
                {'&',  "&amp;"}
        };

        out << "<text "sv;
        out << "x=\""sv << coords_.x << "\" y=\""sv << coords_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;
        out << "font-size=\""sv << font_size_ << "\" "sv;

        if (font_family_) {
            out << "font-family=\""sv << *font_family_ << "\" "sv;
        }

        if (font_weight_) {
            out << "font-weight=\""sv << *font_weight_ << "\""sv;
        }
        // Выводим атрибуты, унаследованные от PathProps
        RenderAttrs(context.out);
        //
        out << ">";
        for (char x: text_) {
            if (special_chars.count(x)) {
                context.out << special_chars.at(x);
            } else {
                context.out << x;
            }
        }
        out << "</text>"sv;
    }

    void Document::Render(std::ostream &out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        RenderContext ctx(out, 2, 2);
        for (const auto &object: objects_) {
            object->Render(ctx);
        }

        out << "</svg>"sv << std::endl;
    }

    void Document::AddPtr(std::unique_ptr<Object> &&obj) {
        objects_.emplace_back(std::move(obj));
    }

}  // namespace svg