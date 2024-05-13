#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

using namespace std::string_view_literals;

namespace json {

    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;


    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Node() = default;
        Node(nullptr_t)
            :value_(nullptr)
        {
        }
        Node(Array arr)
            :value_(std::move(arr))
        {
        }
        Node(Dict dict)
            :value_(std::move(dict))
        {
        }
        Node(bool value)
            :value_(value)
        {
        }
        Node(int value)
            :value_(value)
        {
        }
        Node(double value)
            :value_(value)
        {
        }
        Node(std::string value)
            :value_(std::move(value))
        {
        }

        bool IsInt() const;
        //Bозвращает true, если в Node хранится int либо double
        bool IsDouble() const;
        //Возвращает true, если в Node хранится double
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        int AsInt() const;
        bool AsBool() const;
        //Возвращает значение типа double, если внутри хранится double либо int. 
        //В последнем случае возвращается приведённое в double значение
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        const Value& GetValue() const { return value_; }

        bool operator==(const Node& rhs) const;
        bool operator!=(const Node& rhs) const;

    private:
        Value value_;
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document& rhs) const {
            return root_ == rhs.root_;
        }

        bool operator!=(const Document& rhs) const {
            return !(root_ == rhs.root_);
        }
    private:
        Node root_;
    };

    Document Load(std::istream& input);

    // Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
    struct PrintContext {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const {
            return { out, indent_step, indent_step + indent };
        }
    };

    struct PrintValue {
        PrintContext& ctx;

        void operator()(const std::nullptr_t);
        void operator()(const Array& array);
        void operator()(const Dict dict);
        void operator()(const bool value);
        void operator()(const int value);
        void operator()(const double value);
        void operator()(const std::string value);
    };
    void Print(const Document& doc, std::ostream& out);

}  // namespace json