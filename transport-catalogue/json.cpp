#include "json.h"
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace json {
    Document::Document(Node root)
            : root_(move(root)) {
    }

    const Node &Document::GetRoot() const {
        return root_;
    }

    bool Document::operator==(const Document &rhs) const {
        return this->GetRoot() == rhs.GetRoot();
    }

    bool Document::operator!=(const Document &rhs) const {
        return this->GetRoot() != rhs.GetRoot();
    }

    Node LoadNode(std::istream &input);

    Node LoadArray(std::istream &input) {
        std::vector<Node> result;

        char c;
        for (c = '['; input >> c && c != ']';) {
            if (c != ',') {
                input.putback(c);
            }
            result.push_back(LoadNode(input));
        }
        if (c != ']') {
            throw ParsingError("LoadArray error");
        }
        return Node(move(result));
    }

    Node LoadInt(std::istream &input) {
        int result = 0;
        while (isdigit(input.peek())) {
            result *= 10;
            result += input.get() - '0';
        }
        return Node(result);
    }

    Node LoadString(std::istream &input) {
        using namespace std::literals;

        auto it = std::istreambuf_iterator<char>(input);
        auto end = std::istreambuf_iterator<char>();
        std::string s;
        while (true) {
            if (it == end) {
                // Поток закончился до того, как встретили закрывающую кавычку?
                throw ParsingError("String parsing error");
            }
            const char ch = *it;
            if (ch == '"') {
                // Встретили закрывающую кавычку
                ++it;
                break;
            } else if (ch == '\\') {
                // Встретили начало escape-последовательности
                ++it;
                if (it == end) {
                    // Поток завершился сразу после символа обратной косой черты
                    throw ParsingError("String parsing error");
                }
                const char escaped_char = *(it);
                // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                }
            } else if (ch == '\n' || ch == '\r') {
                // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                throw ParsingError("Unexpected end of line"s);
            } else {
                // Просто считываем очередной символ и помещаем его в результирующую строку
                s.push_back(ch);
            }
            ++it;
        }

        return Node(move(s));
    }

    Node LoadDict(std::istream &input) {
        std::map<std::string, Node> result;

        char c;
        for (c = '{'; input >> c && c != '}';) {
            if (c == ',') {
                input >> c;
            }

            std::string key = LoadString(input).AsString();
            input >> c;
            result.insert({move(key), LoadNode(input)});
        }
        if (c != '}') {
            throw ParsingError("LoadDict error");
        }

        return Node(move(result));
    }

    Node LoadBool(std::istream &input) {
        using namespace std::literals;
        std::string parsed_bool;
        if (input.peek() == 't') {
            for (uint8_t i = 0; i < 4; ++i) {
                parsed_bool += static_cast<char>(input.get());
            }
            if (!input) {
                throw ParsingError("Failed to read bool from stream"s);
            }
            if (parsed_bool == "true"s) {
                return Node(true);
            } else {
                throw ParsingError("Failed to read bool from stream"s);
            }

        } else {
            for (uint8_t i = 0; i < 5; ++i) {
                parsed_bool += static_cast<char>(input.get());
            }
            if (!input) {
                throw ParsingError("Failed to read bool from stream"s);
            }
            if (parsed_bool == "false"s) {
                return Node(false);
            } else {
                throw ParsingError("Failed to read bool from stream"s);
            }
        }
    }

    Node LoadNull(std::istream &input) {
        using namespace std::literals;
        std::string parsed_null;
        if (input.peek() == 'n') {
            for (uint8_t i = 0; i < 4; ++i) {
                parsed_null += static_cast<char>(input.get());
            }
            if (!input) {
                throw ParsingError("Failed to read null from stream"s);
            }
            if (parsed_null == "null"s) {
                return Node(nullptr);
            } else {
                throw ParsingError("Failed to read null from stream"s);
            }
        }
        throw ParsingError("Null expected"s);
    }

    using Number = std::variant<int, double>;

    Node LoadNumerical(std::istream &input) {
        using namespace std::literals;

        std::string parsed_num;

        // Считывает в parsed_num очередной символ из input
        auto read_char = [&parsed_num, &input] {
            parsed_num += static_cast<char>(input.get());
            if (!input) {
                throw ParsingError("Failed to read number from stream"s);
            }
        };

        // Считывает одну или более цифр в parsed_num из input
        auto read_digits = [&input, read_char] {
            if (!std::isdigit(input.peek())) {
                throw ParsingError("A digit is expected"s);
            }
            while (std::isdigit(input.peek())) {
                read_char();
            }
        };

        if (input.peek() == '-') {
            read_char();
        }
        // Парсим целую часть числа
        if (input.peek() == '0') {
            read_char();
            // После 0 в JSON не могут идти другие цифры
        } else {
            read_digits();
        }

        bool is_int = true;
        // Парсим дробную часть числа
        if (input.peek() == '.') {
            read_char();
            read_digits();
            is_int = false;
        }

        // Парсим экспоненциальную часть числа
        if (int ch = input.peek(); ch == 'e' || ch == 'E') {
            read_char();
            if (ch = input.peek(); ch == '+' || ch == '-') {
                read_char();
            }
            read_digits();
            is_int = false;
        }

        try {
            if (is_int) {
                // Сначала пробуем преобразовать строку в int
                try {
                    return Node(std::stoi(parsed_num));
                } catch (...) {
                    // В случае неудачи, например, при переполнении,
                    // код ниже попробует преобразовать строку в double
                }
            }
            return Node(std::stod(parsed_num));
        } catch (...) {
            throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
        }
    }

    Node LoadNode(std::istream &input) {
        char c;
        input >> c;

        try {
            if (c == '[') {
                return LoadArray(input);
            } else if (c == ']') {
                throw ParsingError("Failed to load");
            } else if (c == '{') {
                return LoadDict(input);
            } else if (c == '}') {
                throw ParsingError("Failed to load");
            } else if (c == '"') {
                return LoadString(input);
            } else if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) {
                input.putback(c);
                return LoadNumerical(input);
            } else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            } else if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
        } catch (...) {
            throw ParsingError("Failed to load");
        }
        return LoadNode(input);
    }

    Document Load(std::istream &input) {
        return Document{LoadNode(input)};
    }

    // Перегрузка функции PrintValue для вывода значений null
    void PrintValue(std::nullptr_t, std::ostream &out) {
        using namespace std::literals;
        out << "null"sv;
    }

    // Перегрузка функции PrintValue для вывода значений string
    void PrintValue(std::string text, std::ostream &out) {
        using namespace std::literals;

        std::map<char, std::string> special_chars{
                {'\"', R"(\")"s},
                {'\r', R"(\r)"s},
                {'\n', R"(\n)"s},
                {'\t', "\t"s},
                {'\\', R"(\\)"s}
        };

        out << "\""s;
        for (char x: text) {
            if (special_chars.count(x)) {
                out << special_chars.at(x);
            } else {
                out << x;
            }
        }
        out << "\""s;
    }

    // Перегрузка функции PrintValue для вывода значений array
    void PrintValue(const Array &arr, std::ostream &out) {
        using namespace std::literals;
        out << "["sv;
        bool first = true;
        for (const auto &e: arr) {
            if (!first) {
                out << ","s;
            }
            first = false;
            std::visit(
                    [&out](const auto &value) { PrintValue(value, out); },
                    e.GetValue());

        }
        out << "]"sv;
    }

    // Перегрузка функции PrintValue для вывода значений dict
    void PrintValue(const Dict &dict, std::ostream &out) {
        using namespace std::literals;
        out << "{"sv;
        bool first = true;
        for (const auto &e: dict) {
            if (!first) {
                out << ","sv;
            }
            first = false;
            out << "\""sv;
            out << e.first;
            out << "\": "sv;
            std::visit(
                    [&out](const auto &value) { PrintValue(value, out); },
                    e.second.GetValue());
        }
        out << "}"sv;
    }

    void PrintNode(Node const &node, std::ostream &out) {
        std::visit(
                [&out](const auto &value) { PrintValue(value, out); },
                node.GetValue());
    }

    void Print(const Document &doc, std::ostream &out) {
        PrintContext ctx(out, 2, 2);
        //ctx.PrintIndent();

        // Делегируем вывод тега своим подклассам
        PrintNode(doc.GetRoot(), ctx.out);

        //ctx.out << std::endl;
    }

    bool operator==(const Node &lhs, const Array &rhs) {
        return lhs.AsArray() == rhs;
    }
} //namespace json