// #include <bits/stdc++.h>
#include <iostream>
#include <variant>
#include <vector>
#include <map>
#include <optional>
#include <string>
#include <fstream>
#include <sstream>

// JSON库的命名空间
namespace json {
    using namespace std;
    // JSON节点结构体
    struct Node;
    using Null = std::monostate;
    // using bool = bool;
    // using int64_t = int64_t;
    // using Float = double;
    // using string = std::string;
    using Array = std::vector<Node>;
    using Object = std::map<std::string, Node>;
    using Value = std::variant<Null, bool, int64_t, double, string, Array, Object>;
    
    // JSON节点结构体
    struct Node {
        Value value;
        Node(Value _value) : value(_value) {}
        Node() : value(Null{}) {}
        
        // 通过索引获取元素
        auto& operator[](const std::string& key) {
            if (auto object = std::get_if<Object>(&value)) {
                return  (*object)[key];
            }
            throw std::runtime_error("not an object");
        }
        
        // 通过索引获取元素
        auto operator[](size_t index) {
            if (auto array = std::get_if<Array>(&value)) {
                return array->at(index);
            }
            throw std::runtime_error("not an array");
        }
        
        // 在数组中添加元素
        void push(const Node& rhs) {
            if (auto array = std::get_if<Array>(&value)) {
                array->push_back(rhs);
            }
        }
    };

    // JSON解析器结构体
    struct JsonParser {
        std::string_view json_str;  // JSON字符串
        size_t pos = 0;  // 解析位置

        // 解析空格
        void parse_whitespace() {
            while (pos < json_str.size() && std::isspace(json_str[pos])) {
                ++pos;
            }
        }

        // 解析null
        /* std::optional是C++17标准库中引入的一种类型，用于表示一个可能不存在的值。
        它是一个容器，可以存储一个T类型的值或者没有值（即空值）。
        使用std::optional可以避免返回空指针或者使用特殊的值来表示缺失的值。*/
        auto parse_null() -> std::optional<Value> {
            if (json_str.substr(pos, 4) == "null") {
                pos += 4;
                return Null{};
            }
            return{};
        }

        // 解析true
        auto parse_true() -> std::optional<Value> {
            if (json_str.substr(pos, 4) == "true") {
                pos += 4;
                return true;
            }
            return {};
        }

        // 解析false 
        auto parse_false() -> std::optional<Value> {
            if (json_str.substr(pos, 5) == "false") {
                pos += 5;
                return false;
            }
            return {};
        }

        // 解析数字
        auto parse_number() -> std::optional<Value> {
            size_t endpos = pos;                           // 定义变量endpos并初始化为pos的值
            while (endpos < json_str.size() && (          // 当endpos小于json_str的长度且
                std::isdigit(json_str[endpos]) ||         // json_str的endpos位置是数字字符
                json_str[endpos] == 'e' ||                // 或者json_str的endpos位置是'e'
                json_str[endpos] == '.')) {               // 或者json_str的endpos位置是'.')
                endpos++;                                // endpos自增1
            }
            std::string number = std::string{ json_str.substr(pos, endpos - pos) };  // 使用json_str的子串来创建number字符串
            pos = endpos;                                  // 更新pos的值为endpos
            static auto is_double = [](std::string& number) {    // 定义静态成员函数is_double，参数为字符串引用number
                return number.find('.') != number.npos ||     // 如果number中存在'.'字符的位置不等于字符串npos（即number中存在小数点）
                    number.find('e') != number.npos;          // 或者number中存在'e'字符的位置不等于字符串npos（即number中存在指数符号）
            };
            if (is_double(number)) {                            // 如果number是小数或指数形式
                try {
                    double ret = std::stod(number);            // 将number转换为double类型
                    return ret;                               // 返回转换后的结果
                }
                catch (...) {                                   // 捕获任何异常
                    return {};                                 // 返回空的optional对象
                }
            }
            else {                                              // 如果number是整数
                try {
                    int64_t ret = std::stoi(number);           // 将number转换为int64_t类型
                    return ret;                                // 返回转换后的结果
                }
                catch (...) {                                    // 捕获任何异常
                    return {};                                  // 返回空的optional对象
                }
            }
        }

        // 解析字符串
        auto parse_string() -> std::optional<Value> {
            pos++;// "
            size_t endpos = pos;
            while (pos < json_str.size() && json_str[endpos] != '"') {
                endpos++;
            }
            std::string str = std::string{ json_str.substr(pos, endpos - pos) };
            pos = endpos + 1;// "
            return str;
        }

        // 解析数组
        auto parse_array() -> std::optional<Value> {
            pos++;// [
            Array arr;
            while (pos < json_str.size() && json_str[pos] != ']') {
                auto value = parse_value();
                arr.push_back(value.value());
                parse_whitespace();
                if (pos < json_str.size() && json_str[pos] == ',') {
                    pos++;// ,
                }
                parse_whitespace();
            }
            pos++;// ]
            return arr;
        }

        // 解析对象
        auto parse_object() -> std::optional<Value> {
            pos ++; // 解析到下一个字符，对应于"{"
            Object obj; // 创建一个对象用于存储键值对
            while (pos < json_str.size() && json_str[pos] != '}') {
                auto key = parse_value(); // 解析键
                parse_whitespace();
                if (!std::holds_alternative<string>(key.value())) {
                    return {}; // 键不是字符串类型，返回空值
                }
                if (pos < json_str.size() && json_str[pos] == ':') {
                    pos++; // 解析到下一个字符，对应于":"
                }
                parse_whitespace();
                auto val = parse_value(); // 解析值
                obj[std::get<string>(key.value())] = val.value(); // 将键值对添加到对象中
                parse_whitespace();
                if (pos < json_str.size() && json_str[pos] == ',') {
                    pos++; // 解析到下一个字符，对应于","
                }
                parse_whitespace();
            }
            pos ++; // 解析到下一个字符，对应于"}"
            return obj; // 返回解析出的对象
        }

        // 解析值
        auto parse_value() -> std::optional<Value> {
            parse_whitespace();
            switch (json_str[pos]) {
            case 'n':
                return parse_null();
            case 't':
                return parse_true();
            case 'f':
                return parse_false();
            case '"':
                return parse_string(); 
            case '[':
                return parse_array();
            case '{':
                return parse_object();
            default:
                return parse_number();
            }
        }

        // 解析JSON
        auto parse() -> std::optional<Node> {
            parse_whitespace();
            auto value = parse_value();
            if (!value) {
                return {};
            }
            return Node{ *value };
        }
    };


    // JSON解析辅助函数
    auto parser(std::string_view json_str) -> std::optional<Node> {
        JsonParser p{ json_str };
        return p.parse();
    }


    // JSON生成类
    class JsonGenerator {
    public:
        // 生成指定节点的字符串表示
        static auto generate(const Node& node) -> std::string {
            return std::visit(
                [](auto&& arg) -> std::string {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, Null>) {
                        return "null";  // 如果节点的类型是Null，则返回字符串"null"
                    }
                    else if constexpr (std::is_same_v<T, bool>) {
                        return arg ? "true" : "false";  // 如果节点的类型是bool，则返回字符串true或false
                    }
                    else if constexpr (std::is_same_v<T, int64_t>) {
                        return std::to_string(arg);  // 如果节点的类型是int64_t，则返回字符串类型的节点值
                    }
                    else if constexpr (std::is_same_v<T, double>) {
                        return std::to_string(arg);  // 如果节点的类型是double，则返回字符串类型的节点值
                    }
                    else if constexpr (std::is_same_v<T, string>) {
                        return generate_string(arg);  // 如果节点的类型是string，则调用generate_string函数生成字符串
                    }
                    else if constexpr (std::is_same_v<T, Array>) {
                        return generate_array(arg);  // 如果节点的类型是Array，则调用generate_array函数生成字符串
                    }
                    else if constexpr (std::is_same_v<T, Object>) {
                        return generate_object(arg);  // 如果节点的类型是Object，则调用generate_object函数生成字符串
                    }
                },
                node.value);
        }

        // 生成字符串形式的JSON
        static auto generate_string(const string& str) -> std::string {
            std::string json_str = "\"";  // 创建一个字符串变量json_str，并赋值为双引号
            json_str += str;  // 将输入的字符串str追加到json_str变量的末尾
            json_str += '"';  // 在json_str末尾追加一个双引号
            return json_str;  // 返回生成的json字符串
        }

        // 生成数组形式的JSON
        // 生成包含指定数组的 JSON 数组字符串
        static auto generate_array(const Array& array) -> std::string {
            std::string json_str = "[";
            for (const auto& node : array) {
                json_str += generate(node);
                json_str += ',';
            }
            // 去除最后一个逗号
            if (!array.empty()) json_str.pop_back();
            json_str += ']';
            return json_str;
        }

        // 生成对象形式的JSON
        // 生成对象的 JSON 字符串表示
        static auto generate_object(const Object& object) -> std::string {
            std::string json_str = "{"; // 初始化 JSON 字符串为 "{"
            for (const auto& [key, node] : object) { // 遍历对象的键值对
                json_str += generate_string(key); // 生成键的字符串表示并拼接到 JSON 字符串中
                json_str += ':'; // 在 JSON 字符串中添加冒号
                json_str += generate(node); // 生成值的字符串表示并拼接到 JSON 字符串中
                json_str += ','; // 在 JSON 字符串中添加逗号
            }
            if (!object.empty()) json_str.pop_back(); // 移除最后一个逗号
            json_str += '}'; // 在 JSON 字符串中添加大括号的闭合字符
            return json_str; // 返回生成的 JSON 字符串
        }
    };

    // 生成JSON字符串
    inline auto generate(const Node& node) -> std::string { return JsonGenerator::generate(node); }


    // 输出重载
    auto  operator << (std::ostream& out, const Node& t) ->std::ostream& {
        out << JsonGenerator::generate(t);
        return out;
    }

    
}

// 使用命名空间
using namespace json;

int main() {
    std::ifstream fin("launch.json");
    std::stringstream ss; 
    ss << fin.rdbuf();
    std::string s{ ss.str() };
    auto x = parser(s).value();
    std::cout << x << "\n";

    // 修改节点内容
    x["configurations"].push({true});
    x["configurations"].push({Null {}});
    x["version"] = { 114514LL };
    std::cout << x << "\n";
}
