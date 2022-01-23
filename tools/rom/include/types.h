#ifndef MExtTypes_H
#define MExtTypes_H

#include <string>
#include <vector>
#include <functional>

struct ExtractRule {
    std::string name;
    std::vector<std::string> rules;
};

struct Transformer {
    std::string name;
    std::function<void(std::string, std::string)> callback;
};

#endif