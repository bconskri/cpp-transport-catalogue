#pragma once

#include <istream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <iomanip>

namespace json {
    class Node;

    using Array = std::vector<Node>;
    using Dict = std::map<std::string, Node>;

    class Node : public std::variant<std::nullptr_t, std::vector<Node>, std::map<std::string, Node>,
            bool, int, double, std::string> {
    public:
        using variant::variant;


        bool IsInt() const;

        bool IsDouble() const;

        bool IsPureDouble() const;

        bool IsBool() const;

        bool IsString() const;

        bool IsNull() const;

        bool IsArray() const;

        bool IsMap() const;

        int &AsInt();

        double AsDouble();

        bool &AsBool();

        std::string &AsString();

        Array &AsArray();

        Dict &AsMap();

        const std::variant<std::nullptr_t, std::vector<Node>, std::map<std::string, Node>,
                bool, int, double, std::string> &GetValue() const;
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

        PrintContext(std::ostream &out);

        PrintContext(std::ostream &out, int indent_step, int indent = 0);

        void PrintIndent() const;

        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const;
    };

    template<typename Value>
    void PrintValue(const Value &value, std::ostream &out) {
        out << std::boolalpha << std::fixed << std::setprecision(6) << value;
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