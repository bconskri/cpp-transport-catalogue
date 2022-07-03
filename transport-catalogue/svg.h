#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <variant>

namespace svg {

    struct Point {
        Point() = default;

        Point(double x, double y)
                : x(x), y(y) {
        }

        double x = 0;
        double y = 0;
    };

    /* Добавляем цвет
    */
    struct Rgb {
        uint8_t red = 0, green = 0, blue = 0;

        Rgb() = default;

        explicit  Rgb(uint8_t red, uint8_t green , uint8_t blue)
                : red(red), green(green), blue(blue) {}

        void Render(std::ostream &out) const {
            out << "rgb(" << static_cast<int>(red) << ","
                << static_cast<int>(green) << "," << static_cast<int>(blue) << ")";
        }
    };


    struct Rgba {
        uint8_t red = 0, green = 0, blue = 0;
        double opacity = 1.0;

        Rgba() = default;

        explicit Rgba(uint8_t red, uint8_t green , uint8_t blue, double opacity)
                : red(red), green(green), blue(blue), opacity(opacity) {}

        void Render(std::ostream &out) const {
            out << "rgba(" << static_cast<int>(red) << ","
                << static_cast<int>(green) << "," << static_cast<int>(blue)
                << "," << opacity << ")";
        }
    };

    using Color = std::optional<std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>>;
    // Объявив в заголовочном файле константу со спецификатором inline,
    // мы сделаем так, что она будет одной на все единицы трансляции,
    // которые подключают этот заголовок.
    // В противном случае каждая единица трансляции будет использовать свою копию этой константы
    inline const Color NoneColor{"none"};

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
    struct RenderContext {
        RenderContext(std::ostream &out)
                : out(out) {
        }

        RenderContext(std::ostream &out, int indent_step, int indent = 0)
                : out(out), indent_step(indent_step), indent(indent) {
        }

        RenderContext Indented() const {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream &out;
        int indent_step = 0;
        int indent = 0;
    };

    inline std::ostream &operator<<(std::ostream &out, const StrokeLineCap &line) {
        switch (line) {
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

    inline std::ostream &operator<<(std::ostream &out, const StrokeLineJoin &line) {
        switch (line) {
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

    ///render color
    inline std::ostream &operator<<(std::ostream &out, const Color &color) {
        if (color) {
            const size_t index = color->index();

            switch (index) {
                case 0:
                    out << "none";
                    break;

                case 1:
                    out << std::get<std::string>(*color);
                    break;

                case 2:
                    std::get<Rgb>(*color).Render(out);
                    break;

                case 3:
                    std::get<Rgba>(*color).Render(out);
                    break;

                default:
                    throw std::bad_variant_access();
            }
        } else {
            out << "none";
        }
        return out;
    }

    template<typename Owner>
    class PathProps {
    public:
        ///задаёт значение свойства [fill] — цвет заливки. По умолчанию свойство не выводится.
        Owner &SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }

        ///задаёт значение свойства [stroke] - цвет контура. По умолчанию свойство не выводится.
        Owner &SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }

        ///задаёт значение свойства [stroke-width] - толщину линии.
        /// По умолчанию свойство не выводится.
        Owner &SetStrokeWidth(double width) {
            stroke_width_ = width;
            return AsOwner();
        }

        ///задаёт значение свойства [stroke-linecap] - тип формы конца линии.
        /// По умолчанию свойство не выводится.
        Owner &SetStrokeLineCap(StrokeLineCap line_cap) {
            line_cap_ = std::move(line_cap);
            return AsOwner();
        }

        ///задаёт значение свойства [stroke-linejoin] - тип формы соединения линий.
        /// По умолчанию свойство не выводится.
        Owner &SetStrokeLineJoin(StrokeLineJoin line_join) {
            line_join_ = std::move(line_join);
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream &out) const {
            using namespace std::literals;

            if (fill_color_) {
                out << " fill=\""sv << *fill_color_ << "\""sv;
            }
            if (stroke_color_) {
                out << " stroke=\""sv << *stroke_color_ << "\""sv;
            }
            if (stroke_width_) {
                out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
            }
            if (line_cap_) {
                out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
            }
            if (line_join_) {
                out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
            }

        }

    private:
        Owner &AsOwner() {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner &>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> stroke_width_;
        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;
    };

    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */
    class Object {
    public:
        virtual void Render(const RenderContext &context) const;

        virtual ~Object() =
        default;

    private:
        virtual void RenderObject(const RenderContext &context) const = 0;
    };

    /*ObjectContainer задаёт интерфейс для доступа к контейнеру SVG-объектов.
     * Через этот интерфейс Drawable-объекты могут визуализировать себя, добавляя в
     * контейнер SVG-примитивы.
     * svg::Document — пока единственный класс библиотеки, реализующий интерфейс ObjectContainer.
     */
    class ObjectContainer {
    public:
        /*
            Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
            Пример использования:
            Document doc;
            doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
           */
        template<typename Obj>
        void Add(Obj obj);

        virtual void AddPtr(std::unique_ptr<Object> &&obj) = 0;

    protected:
        virtual ~ObjectContainer() =
        default;
    };

    /*Интерфейс Drawable унифицирует работу с объектами, которые можно нарисовать,
     * подключив SVG-библиотеку. Для этого в нём есть метод Draw, принимающий ссылку
     * на интерфейс ObjectContainer.
     */
    class Drawable {
    public:
        virtual ~Drawable() =
        default;

        virtual void Draw(svg::ObjectContainer &container) const = 0;
    };

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
    class Circle
    final : public Object, public PathProps<Circle> {
    public:
        Circle &SetCenter(Point center);

        Circle &SetRadius(double radius);

    private:
        void RenderObject(const RenderContext &context) const
        override;

        Point center_;
        double radius_ = 1.0;
    };

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
    class Polyline
    final : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline &AddPoint(Point point);
        /*
         * Прочие методы и данные, необходимые для реализации элемента <polyline>
         */
    private:
        void RenderObject(const RenderContext &context) const
        override;

        std::vector<Point> points_;
    };

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
    class Text
    final : public Object, public PathProps<Text> {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text &SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text &SetOffset(Point point);

        // Задаёт размеры шрифта (атрибут font-size)
        Text &SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text &SetFontFamily(const std::string &font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text &SetFontWeight(const std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text &SetData(const std::string &data);

        // Прочие данные и методы, необходимые для реализации элемента <text>
    private:
        void RenderObject(const RenderContext &context) const
        override;

        Point coords_ = Point();
        Point offset_ = Point();
        uint32_t font_size_ = 1;
        std::optional<std::string> font_weight_ = std::nullopt;
        std::optional<std::string> font_family_ = std::nullopt;
        std::string text_;
    };

    class Document : public ObjectContainer {
    public:
        Document() =
        default;

        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object> &&obj)
        override;

        // Выводит в ostream svg-представление документа
        void Render(std::ostream &out) const;

        // Прочие методы и данные, необходимые для реализации класса Document
    private:
        std::vector<std::unique_ptr<Object>> objects_;
    };


    template<typename Obj>
    void ObjectContainer::Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }

}  // namespace svg