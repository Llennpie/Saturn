#ifndef SaturnJsonParser
#define SaturnJsonParser

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <map>

// jsoncpp was misbehaving with math_util.h
// heres a custom json parser :)

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
        if (ptr == len) throw std::runtime_error("EOF");
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
    std::map<std::string, Value> obj;
    std::vector<Value> arr;
    std::string str;
    double num;
    ValueType type;
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
            if (is_invalid(character)) error("Invalid character", lineNum);
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
        if (tokens[ptr->get()].type == JSONTOKEN_OBJ_OPEN) {
            value.type = JSONVALUE_OBJECT;
            value.obj = {};
            while (true) {
                ptr->inc();
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
        }
        else if (tokens[ptr->get()].type == JSONTOKEN_ARR_OPEN) {
            value.type = JSONVALUE_ARRAY;
            value.arr = {};
            while (true) {
                ptr->inc();
                value.arr.push_back(parseValue(tokens, ptr));
                if (tokens[ptr->get()].type == JSONTOKEN_COMMA) continue;
                if (tokens[ptr->get()].type == JSONTOKEN_ARR_CLOSE) break;
                error("Expected comma or end of array", tokens[ptr->get()].line_num); 
            }
            ptr->inc();
        }
        else if (tokens[ptr->get()].type == JSONTOKEN_STRING_LITERAL) {
            value.type = JSONVALUE_STRING;
            value.str = tokens[ptr->get()].value;
            ptr->inc();
        }
        else if (tokens[ptr->get()].type == JSONTOKEN_NUMBER) {
            value.type = JSONVALUE_NUMBER;
            value.num = std::stod(tokens[ptr->get()].value);
            ptr->inc();
        }
        else if (tokens[ptr->get()].type == JSONTOKEN_TRUE) {
            value.type = JSONVALUE_BOOL;
            value.num = 1;
            ptr->inc();
        }
        else if (tokens[ptr->get()].type == JSONTOKEN_FALSE) {
            value.type = JSONVALUE_BOOL;
            value.num = 0;
            ptr->inc();
        }
        else if (tokens[ptr->get()].type == JSONTOKEN_NULL) {
            value.type = JSONVALUE_NULL;
            ptr->inc();
        }
        else error("Unexpected token", tokens[ptr->get()].line_num);
        return value;
    }
    void error(std::string msg, int line_num) {
        std::cout << "JSON parse error (at " << std::to_string(line_num) << "): " << msg << std::endl;
        throw std::runtime_error("Failed to parse JSON");
    }
public:
    Value operator [](std::string key) {
        if (type != JSONVALUE_OBJECT) throw std::runtime_error("not an object");
        if (!isMember(key)) throw std::runtime_error("key doesn't exist");
        return obj[key];
    }
    Value operator [](int index) {
        if (type != JSONVALUE_ARRAY) throw std::runtime_error("not an array");
        if (index < 0 || index >= arr.size()) throw std::runtime_error("index " + std::to_string(index) + " out of bounds, length: " + std::to_string(arr.size()));
        return arr[index];
    }
    void operator<<(std::ifstream& stream) {
        auto tokenVec = tokenize(stream);
        int numTokens = 0;
        Token* tokens = tokenVec.data();
        Increment increment = Increment(tokenVec.size());
        *this = parseValue(tokens, &increment);
    }
    int asInt() {
        if (type != JSONVALUE_NUMBER) throw std::runtime_error("not a number");
        return (int)num;
    }
    double asDouble() {
        if (type != JSONVALUE_NUMBER) throw std::runtime_error("not a number");
        return num;
    }
    float asFloat() {
        if (type != JSONVALUE_NUMBER) throw std::runtime_error("not a number");
        return (float)num;
    }
    std::string asString() {
        if (type != JSONVALUE_STRING) throw std::runtime_error("not a string");
        return str;
    }
    bool asBool() {
        if (type != JSONVALUE_BOOL) throw std::runtime_error("not a bool");
        return num != 0;
    }
    bool isMember(std::string name) {
        if (type != JSONVALUE_OBJECT) throw std::runtime_error("not an object");
        return obj.find(name) != obj.end();
    }
};

#undef is_alphanumeric
#undef is_letter
#undef is_number
#undef is_ignore
#undef is_symbol

#endif