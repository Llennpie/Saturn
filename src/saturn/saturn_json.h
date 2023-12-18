#ifndef SaturnJsonParser
#define SaturnJsonParser

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <map>
#include <iterator>
#include <sstream>
#include <iomanip>

#define pfx "Json Parser Error: "

namespace Json {
    enum TokenType {
        JSONTOKEN_OBJ_OPEN,
        JSONTOKEN_OBJ_CLOSE,
        JSONTOKEN_ARR_OPEN,
        JSONTOKEN_ARR_CLOSE,
        JSONTOKEN_COMMA,
        JSONTOKEN_COLON,
        JSONTOKEN_TRUE,
        JSONTOKEN_FALSE,
        JSONTOKEN_NULL,
        JSONTOKEN_NUMBER,
        JSONTOKEN_STRING_LITERAL,
        JSONTOKEN_NONE,
        JSONTOKEN_WORD,
    };
    struct Token;
    enum ValueType {
        JSONVALUE_NULL,
        JSONVALUE_OBJECT,
        JSONVALUE_ARRAY,
        JSONVALUE_NUMBER,
        JSONVALUE_STRING,
        JSONVALUE_BOOL,
    };
    class Value;
    class Increment;
}

struct Json::Token {
    std::string value;
    TokenType type;
    int line_num;
    Token(TokenType type, std::string value, int line_num) {
        this->type = type;
        this->value = value;
        this->line_num = line_num;
    }
};

class Json::Increment {
public:
    int ptr;
    int len;
    void inc() {
        ptr++;
    }
    int get() {
        if (ptr == len) throw std::runtime_error(pfx "EOF");
        return ptr;
    }
    Increment(int length) {
        ptr = 0;
        len = length;
    }
};

#define is_wordable(c) (is_letter(c) || is_number(c) || (c) == '.' || (c) == '-')
#define is_letter(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define is_hexadecimal(c) (((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f') || is_number(c))
#define is_number(c) ((c) >= '0' && (c) <= '9')
#define is_whitespace(c) ((c) <= 32 || (c) >= 127)
#define is_invalid(c) (is_whitespace(c) && (c) != 32 && (c) != 10)
#define is_symbol(c) (!is_whitespace(c) && !is_alphanumeric(c))

class Json::Value {
private:
    std::map<std::string, Value> obj = {};
    std::vector<Value> arr = {};
    std::string str = "";
    double num = 0;
    ValueType type = JSONVALUE_NULL;
    std::vector<Token> tokenize(std::ifstream& stream) {
        std::vector<Token> tokens = {};
        TokenType type = JSONTOKEN_NONE;
        std::string value = "";
        bool backslash = false;
        int lineNum = 1;
        int unicodeInput = 0;
        std::string unicode = "";
        bool readNext = true;
        char character;
        while (!stream.eof()) {
            if (readNext) stream.read(&character, 1);
            readNext = true;
            if (character == 0) break;
            if (character == 0x0D) continue;
            if (character == '\n') lineNum++;
            if (is_invalid(character)) continue;
            if (type == JSONTOKEN_NONE) {
                if (is_whitespace(character)) continue;
                else if (is_wordable(character)) {
                    value = std::string() + character;
                    type = JSONTOKEN_WORD;
                }
                else if (character == ':') tokens.push_back(Token(JSONTOKEN_COLON, ":", lineNum));
                else if (character == ',') tokens.push_back(Token(JSONTOKEN_COMMA, ",", lineNum));
                else if (character == '{') tokens.push_back(Token(JSONTOKEN_OBJ_OPEN, "{", lineNum));
                else if (character == '}') tokens.push_back(Token(JSONTOKEN_OBJ_CLOSE, "}", lineNum));
                else if (character == '[') tokens.push_back(Token(JSONTOKEN_ARR_OPEN, "[", lineNum));
                else if (character == ']') tokens.push_back(Token(JSONTOKEN_ARR_CLOSE, "]", lineNum));
                else if (character == '"') {
                    type = JSONTOKEN_STRING_LITERAL;
                    value = "";
                }
                else error("Unknown token: ", lineNum);
            }
            else if (type == JSONTOKEN_WORD) {
                if (!is_wordable(character)) {
                    if (value == "null") type = JSONTOKEN_NULL;
                    else if (value == "true") type = JSONTOKEN_TRUE;
                    else if (value == "false") type = JSONTOKEN_FALSE;
                    else {
                        type = JSONTOKEN_NUMBER;
                        try {
                            std::stod(value);
                        }
                        catch (...) {
                            error("Malformed number: " + value, lineNum);
                        }
                    }
                    tokens.push_back(Token(type, value, lineNum));
                    type = JSONTOKEN_NONE;
                    readNext = false;
                }
                else value += character;
            }
            else if (type == JSONTOKEN_STRING_LITERAL) {
                if (character == '\n') error("Unexpected EOL", lineNum);
                if (backslash) {
                    backslash = false;
                    if (character == 'n') value += "\n";
                    else if (character == 'b') value += "\b";
                    else if (character == 'f') value += "\f";
                    else if (character == 'r') value += "\r";
                    else if (character == 't') value += "\t";
                    else if (character == '"') value += "\"";
                    else if (character == '\\') value += "\\";
                    else if (character == 'u') {
                        unicodeInput = 4;
                        unicode = "";
                    }
                    else error("Invalid escape character: '" + std::to_string(character) + "'", lineNum);
                }
                else if (unicodeInput != 0) {
                    if (is_hexadecimal(character)) unicode += character;
                    else error("Invalid unicode notation", lineNum);
                    unicodeInput--;
                    if (unicodeInput == 0) {
                        char buf[5];
                        snprintf(buf, 4, "%X", std::stoi(unicode, 0, 16));
                        buf[4] = '\0';
                        value += buf;
                    }
                }
                else {
                    if (character == '\\') backslash = true;
                    else if (character == '"') {
                        tokens.push_back(Token(JSONTOKEN_STRING_LITERAL, value, lineNum));
                        type = JSONTOKEN_NONE;
                    }
                    else value += character;
                }
            }
        }
        return tokens;
    }
    Value parseValue(Token* tokens, Increment* ptr) {
        Value value;
        switch (tokens[ptr->get()].type) {
            case JSONTOKEN_OBJ_OPEN:
                value.type = JSONVALUE_OBJECT;
                value.obj = {};
                while (true) {
                    ptr->inc();
                    if (tokens[ptr->get()].type == JSONTOKEN_OBJ_CLOSE) break;
                    if (tokens[ptr->get()].type != JSONTOKEN_STRING_LITERAL) error("Expected json literal", tokens[ptr->get()].line_num);
                    std::string key = tokens[ptr->get()].value;
                    ptr->inc();
                    if (tokens[ptr->get()].type != JSONTOKEN_COLON) error("Expected a colon", tokens[ptr->get()].line_num); 
                    ptr->inc();
                    value.obj.insert({ key, parseValue(tokens, ptr) });
                    if (tokens[ptr->get()].type == JSONTOKEN_COMMA) continue;
                    if (tokens[ptr->get()].type == JSONTOKEN_OBJ_CLOSE) break;
                    error("Expected comma or end of object", tokens[ptr->get()].line_num); 
                }
                ptr->inc();
                break;
            case JSONTOKEN_ARR_OPEN:
                value.type = JSONVALUE_ARRAY;
                value.arr = {};
                while (true) {
                    ptr->inc();
                    if (tokens[ptr->get()].type == JSONTOKEN_ARR_CLOSE) break;
                    value.arr.push_back(parseValue(tokens, ptr));
                    if (tokens[ptr->get()].type == JSONTOKEN_COMMA) continue;
                    if (tokens[ptr->get()].type == JSONTOKEN_ARR_CLOSE) break;
                    error("Expected comma or end of array", tokens[ptr->get()].line_num); 
                }
                ptr->inc();
                break;
            case JSONTOKEN_STRING_LITERAL:
                value.type = JSONVALUE_STRING;
                value.str = tokens[ptr->get()].value;
                ptr->inc();
                break;
            case JSONTOKEN_NUMBER:
                value.type = JSONVALUE_NUMBER;
                value.num = std::stod(tokens[ptr->get()].value);
                ptr->inc();
                break;
            case JSONTOKEN_TRUE:
                value.type = JSONVALUE_BOOL;
                value.num = 1;
                ptr->inc();
                break;
            case JSONTOKEN_FALSE:
                value.type = JSONVALUE_BOOL;
                value.num = 0;
                ptr->inc();
                break;
            case JSONTOKEN_NULL:
                value.type = JSONVALUE_NULL;
                ptr->inc();
                break;
            default:
                error("Unexpected token", tokens[ptr->get()].line_num);
                break;
        }
        return value;
    }
    void error(std::string msg, int line_num) {
        std::cout << "JSON parse error (at line " << std::to_string(line_num) << "): " << msg << std::endl;
        throw std::runtime_error(pfx "Failed to parse JSON");
    }
    std::string escaped_str(std::string str) {
        std::string escaped = "";
        for (int i = 0; i < (int)str.length(); i++) {
            switch (str[i]) {
                case '\"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\b': escaped += "\\b"; break;
                case '\f': escaped += "\\f"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default:
                    if (str[i] >= 32 && str[i] <= 126) escaped += str[i];
                    else {
                        std::stringstream stream;
                        stream << std::hex << std::setw(4) << std::setfill('0') << (int)str[i];
                        escaped += stream.str();
                    }
                    break;
            }
        }
        return escaped;
    }
    std::string strnum(double x) {
        int integer = (int)x;
        if (x == integer) return std::to_string(integer);
        return std::to_string(x);
    }
public:
    Value operator [](std::string key) {
        if (type != JSONVALUE_OBJECT) throw std::runtime_error(pfx "not an object");
        if (!isMember(key)) throw std::runtime_error(pfx "key doesn't exist");
        return obj[key];
    }
    Value operator [](int index) {
        if (type != JSONVALUE_ARRAY) throw std::runtime_error(pfx "not an array");
        if (index < 0 || index >= (int)arr.size()) throw std::runtime_error(pfx "index " + std::to_string(index) + " out of bounds, length: " + std::to_string(arr.size()));
        return arr[index];
    }
    void operator<<(std::ifstream& stream) {
        auto tokenVec = tokenize(stream);
        Token* tokens = tokenVec.data();
        Increment increment = Increment(tokenVec.size());
        *this = parseValue(tokens, &increment);
    }
    int asInt() {
        return (int)asDouble();
    }
    float asFloat() {
        return (float)asDouble();
    }
    double asDouble() {
        if (type == JSONVALUE_STRING) return std::stod(str);
        if (type == JSONVALUE_ARRAY) return arr.size();
        if (type == JSONVALUE_OBJECT) return obj.size();
        if (type == JSONVALUE_NULL) return false;
        return num;
    }
    std::string asString() {
        if (type == JSONVALUE_STRING) return str;
        return stringify();
    }
    bool asBool() {
        if (type == JSONVALUE_STRING) return str == "true";
        if (type == JSONVALUE_ARRAY) return arr.size() != 0;
        if (type == JSONVALUE_OBJECT) return obj.size() != 0;
        if (type == JSONVALUE_NULL) return false;
        return num != 0;
    }
    bool isMember(std::string name) {
        if (type != JSONVALUE_OBJECT) throw std::runtime_error(pfx "not an object");
        return obj.find(name) != obj.end();
    }
    int size() {
        if (type == JSONVALUE_OBJECT) return obj.size();
        if (type == JSONVALUE_ARRAY) return arr.size();
        throw std::runtime_error(pfx "not a sizeable type");
    }
    void put(std::string name, Json::Value value) {
        if (type != JSONVALUE_OBJECT) throw std::runtime_error(pfx "not an object");
        obj.insert({ name, value });
    }
    void put(Json::Value value) {
        if (type != JSONVALUE_ARRAY) throw std::runtime_error(pfx "not an array");
        arr.push_back(value);
    }
    auto array() {
        if (type != JSONVALUE_ARRAY) throw std::runtime_error(pfx "not an array");
        return arr;
    }
    auto object() {
        if (type != JSONVALUE_OBJECT) throw std::runtime_error(pfx "not an object");
        return obj;
    }
    std::string stringify() {
        if (type == JSONVALUE_NUMBER) return strnum(num);
        if (type == JSONVALUE_STRING) return "\"" + escaped_str(str) + "\"";
        if (type == JSONVALUE_BOOL) return num != 0 ? "true" : "false";
        if (type == JSONVALUE_ARRAY) {
            std::string str = "";
            for (auto& element : array()) {
                str += "," + element.stringify();
            }
            if (str.length() != 0) str = str.substr(1, str.length() - 1);
            return "[" + str + "]";
        }
        if (type == JSONVALUE_OBJECT) {
            std::string str = "";
            for (auto& element : object()) {
                str += ",\"" + element.first + "\":" + element.second.stringify();
            }
            if (str.length() != 0) str = str.substr(1, str.length() - 1);
            return "{" + str + "}";
        }
        return "null";
    }
    Value& toNull() {
        type = JSONVALUE_NULL;
        return *this;
    }
    Value& toNumber(double x) {
        type = JSONVALUE_NUMBER;
        num = x;
        return *this;
    }
    Value& toBool(bool x) {
        type = JSONVALUE_BOOL;
        num = x;
        return *this;
    }
    Value& toString(std::string x) {
        type = JSONVALUE_STRING;
        str = x;
        return *this;
    }
    Value& toObject() {
        type = JSONVALUE_OBJECT;
        return *this;
    }
    Value& toArray() {
        type = JSONVALUE_ARRAY;
        return *this;
    }
    ValueType getType() {
        return type;
    }
};

#undef is_alphanumeric
#undef is_letter
#undef is_number
#undef is_ignore
#undef is_symbol

#undef pfx

#endif