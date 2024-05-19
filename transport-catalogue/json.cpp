#include "json.h"

using namespace std;

namespace json {

    namespace {
        using Number = std::variant<int, double>;

        std::string LoadLiteral(std::istream& input) {
            std::string s;
            while (std::isalpha(input.peek())) {
                s.push_back(static_cast<char>(input.get()));
            }
            return s;
        }

        Node LoadNode(istream& input);
        Node LoadArray(istream& input) {
            Array result;
            if (input.peek() == -1) throw ParsingError("Array parsing error");

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }

            return Node(std::move(result));
        }
        Node LoadNumber(std::istream& input) {
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
            }
            else {
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
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }
        Node LoadString(istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    ++it;
                    if (it == end) {
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем \\, \n, \t, \r, \"
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
                }
                else if (ch == '\n' || ch == '\r') {
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }
         Node LoadDict(istream& input) {
            Dict result;
            if (input.peek() == -1) throw ParsingError("Array parsing error");

            for (char c; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }

            return Node(move(result));
        }
        Node LoadBool(istream& input) {
            const auto s = LoadLiteral(input);
            if (s == "true"sv) {
                return Node{ true };
            }
            else if (s == "false"sv) {
                return Node{ false };
            }
            else {
                throw ParsingError("Failed to parse '"s + s + "' as bool"s);
            }

        }
        Node LoadNull(istream& input) {
            if (auto literal = LoadLiteral(input); literal == "null"sv) {
                return Node{ nullptr };
            }
            else {
                throw ParsingError("Failed to parse '"s + literal + "' as null"s);
            }
        }

        Node LoadNode(istream& input) {

            char c;
            input >> c;

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else {
                input.putback(c);
                return LoadNumber(input);
            }
        }


    }  // namespace

    bool Node::IsInt() const
    {
        return std::holds_alternative<int>(value_);
    }

    bool Node::IsDouble() const
    {
        return std::holds_alternative<int>(value_) || std::holds_alternative<double>(value_);
    }

    bool Node::IsPureDouble() const
    {
        return std::holds_alternative<double>(value_);
    }

    bool Node::IsBool() const
    {
        return std::holds_alternative<bool>(value_);
    }

    bool Node::IsString() const
    {
        return std::holds_alternative<std::string>(value_);
    }

    bool Node::IsNull() const
    {
        return std::holds_alternative<std::nullptr_t>(value_);
    }

    bool Node::IsArray() const
    {
        return std::holds_alternative<Array>(value_);
    }

    bool Node::IsMap() const
    {
        return std::holds_alternative<Dict>(value_);
    }

    int Node::AsInt() const {
        if (!IsInt()) throw std::logic_error("wrong type");
        return std::get<int>(value_);
    }

    bool Node::AsBool() const
    {
        if (!IsBool()) throw std::logic_error("wrong type");
        return std::get<bool>(value_);
    }

    double Node::AsDouble() const
    {
        if (!IsDouble()) throw std::logic_error("wrong type");
        if (IsInt()) return static_cast<double>(std::get<int>(value_));

        return std::get<double>(value_);
    }

    const string& Node::AsString() const {
        if (!IsString()) throw std::logic_error("wrong type");
        return std::get<std::string>(value_);
    }

    const Array& Node::AsArray() const {
        if (!IsArray()) throw std::logic_error("wrong type");
        return std::get<Array>(value_);
    }

    Array& Node::AsArray()
    {
        if (!IsArray()) throw std::logic_error("wrong type");
        return std::get<Array>(value_);
    }

    const Dict& Node::AsMap() const {
        if (!IsMap()) throw std::logic_error("wrong type");
        return std::get<Dict>(value_);
    }

    Dict& Node::AsMap()
    {
        if (!IsMap()) throw std::logic_error("wrong type");
        return std::get<Dict>(value_);
    }

    bool Node::operator==(const Node& rhs) const
    {
        return value_ == rhs.value_;
    }

    bool Node::operator!=(const Node& rhs) const
    {
        return !(value_ == rhs.value_);
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void PrintValue::operator()(std::nullptr_t)
    {
        ctx.out << "null"sv;
    }

    void PrintValue::operator()(const Array& array)
    {
        bool is_first = true;
        auto inner_ctx = ctx.Indented();
        ctx.out << "[\n"sv;

        for (const auto& value : array) {
            if (is_first) {
                is_first = false;
            }
            else {
                ctx.out << ",\n"sv;
            }
            inner_ctx.PrintIndent();

            std::visit(PrintValue{ inner_ctx }, value.GetValue());
        }
        ctx.out << "\n"s;
        ctx.PrintIndent();
        ctx.out << "]"sv;
    }

    void PrintValue::operator()(const Dict dict)
    {
        bool is_first = true;
        ctx.out << "{\n"sv;
        auto inner_ctx = ctx.Indented();

        for (auto& [key, value] : dict) {
            if (is_first) {
                is_first = false;
            }
            else {
                ctx.out << ",\n"sv;
            }
            inner_ctx.PrintIndent();

            ctx.out << "\"" << key << "\": ";
            std::visit(PrintValue{ inner_ctx }, value.GetValue());
        }
        ctx.out << "\n"s;
        ctx.PrintIndent();
        ctx.out << "}"sv;
    }

    void PrintValue::operator()(const bool value)
    {
        ctx.out << std::boolalpha << value;
    }

    void PrintValue::operator()(const int value) { ctx.out << value; }

    void PrintValue::operator()(const double value) { ctx.out << value; }

    void PrintValue::operator()(const std::string value)
    {
        ctx.out << "\""sv;
        for (const char& c : value) {
            if (c == '\n') {
                ctx.out << "\\n"sv;
                continue;
            }
            if (c == '\r') {
                ctx.out << "\\r"sv;
                continue;
            }
            if (c == '\"') ctx.out << "\\"sv;
            if (c == '\t') {
                ctx.out << "\\t"sv;
                continue;
            }
            if (c == '\\') ctx.out << "\\"sv;
            ctx.out << c;
        }
        ctx.out << "\""sv;
    }

    void Print(const Document& doc, std::ostream& out) {
        PrintContext ctx{ out };

        std::visit(PrintValue{ ctx }, doc.GetRoot().GetValue());
    }

}  // namespace json