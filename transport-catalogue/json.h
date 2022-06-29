#pragma once

#include <istream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {
    class Node;

    using Array = std::vector<Node>;
    using Dict = std::map<std::string, Node>;

    class Node : public std::variant<std::nullptr_t, std::vector<Node>, std::map<std::string, Node>,
            bool, int, double, std::string> {
    public:
        using variant::variant;


        bool IsInt() const {
            return std::holds_alternative<int>(*this);
        }

        bool IsDouble() const {
            return std::holds_alternative<double>(*this) ||
                   std::holds_alternative<int>(*this);
        }

        bool IsPureDouble() const {
            return std::holds_alternative<double>(*this);
        }

        bool IsBool() const {
            return std::holds_alternative<bool>(*this);
        }

        bool IsString() const {
            return std::holds_alternative<std::string>(*this);
        }

        bool IsNull() const {
            return std::holds_alternative<std::nullptr_t>(*this);
        }

        bool IsArray() const {
            return std::holds_alternative<std::vector<Node>>(*this);
        }

        bool IsMap() const {
            return std::holds_alternative<std::map<std::string, Node>>(*this);
        }

        auto &AsInt() const {
            if (IsInt()) {
                return std::get<int>(*this);
            }
            throw std::logic_error("AsInt()");
        }

        double AsDouble() const {
            if (IsPureDouble()) {
                return std::get<double>(*this);
            } else if (IsDouble()) {
                return static_cast<double>(std::get<int>(*this));
            }
            throw std::logic_error("AsDouble()");
        }

        auto &AsBool() const {
            if (IsBool()) {
                return std::get<bool>(*this);
            }
            throw std::logic_error("AsBool()");
        }

        auto &AsString() const {
            if (IsString()) {
                return std::get<std::string>(*this);
            }
            throw std::logic_error("AsString()");
        }

        auto &AsArray() const {
            if (IsArray()) {
                return std::get<std::vector<Node>>(*this);
            }
            throw std::logic_error("AsArray()");
        }

        auto &AsMap() const {
            if (IsMap()) {
                return std::get<std::map<std::string, Node>>(*this);
            }
            throw std::logic_error("AsMap()");
        }

        const std::variant<std::nullptr_t, std::vector<Node>, std::map<std::string, Node>,
                bool, int, double, std::string> &GetValue() const { return *this; }
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node &GetRoot() const;

        bool operator==(const Document &rhs) const;

        bool operator!=(const Document &rhs) const;

    private:
        Node root_;
    };

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    // Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
    struct PrintContext {
        std::ostream &out;
        int indent_step = 4;
        int indent = 0;

        PrintContext(std::ostream &out)
                : out(out) {
        }

        PrintContext(std::ostream &out, int indent_step, int indent = 0)
                : out(out), indent_step(indent_step), indent(indent) {
        }

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const {
            return {out, indent_step, indent_step + indent};
        }
    };

    template<typename Value>
    void PrintValue(const Value &value, std::ostream &out) {
        out << std::boolalpha << value;
    }

    // Перегрузка функции PrintValue для вывода значений null
    void PrintValue(std::nullptr_t, std::ostream &out);

    // Перегрузка функции PrintValue для вывода значений string
    void PrintValue(std::string, std::ostream &out);

    // Перегрузка функции PrintValue для вывода значений array
    void PrintValue(const Array &, std::ostream &out);

    // Перегрузка функции PrintValue для вывода значений dict
    void PrintValue(const Dict &, std::ostream &out);

    void PrintNode(Node const &node, std::ostream &out);

    void Print(const Document &doc, std::ostream &out);

    Document Load(std::istream &input);

    bool operator==(const Node &lhs, const Array &rhs);


} //namespace json