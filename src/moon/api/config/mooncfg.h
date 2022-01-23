#ifndef MoonCFGApi
#define MoonCFGApi

#include <vector>
#include <string>
#include <sstream>
#include "libs/nlohmann/json.hpp"

class MoonCFG {
protected:
    std::string path;
    bool relativePath;
public:
    MoonCFG(std::string path, bool relativePath);

    nlohmann::json vjson;
    nlohmann::json nested(std::string key);
    std::string formatNestedKey(std::string key);
    std::string getString(std::string key);
    float getFloat(std::string key);
    bool getBool(std::string key);
    int getInt(std::string key);
    bool contains(std::string key);
    template< typename T > std::vector<T> getArray(std::string key) {
        nlohmann::json tmp = this->nested(key);
        if(tmp.is_array())
            return tmp.get<std::vector<T>>();
        return std::vector<std::string>();
    };

    void setString(std::string key, std::string value);
    void setFloat(std::string key, float value);
    void setBool(std::string key, bool value);
    void setInt(std::string key, int value);
    template< typename T > void setArray(std::string key, std::vector<T> array) {
        for(int i = 0; i < array.size(); i++)
            this->vjson[formatNestedKey(key)+"/"+std::to_string(i)] = array[i];
    };

    void reload();
    void save();
    bool isNewInstance = false;
};

#endif