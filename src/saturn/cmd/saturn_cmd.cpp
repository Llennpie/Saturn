#include "saturn_cmd.h"
#include "saturn_cmd_def.h"
#include "saturn/filesystem/saturn_registerfile.h"

#include <string>
#include <map>
#include <optional>
#include <vector>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdarg>

std::vector<std::pair<int, int>> loops = {};
int line = 0;
int* registers = (int*)malloc(sizeof(int) * SATURN_CMD_REG_COUNT);

bool is_cli = false;

bool paused = false;
std::vector<std::string> lines = {};
std::map<std::string, int> aliases = {};

//#define CMD_DO_DEBUG // log the command engine's behavior, comment this line out to disable

std::vector<std::string> args(std::string input, char delimiter) {
    std::string token = "";
    std::vector<std::string> tokens = {};
    for (int i = 0; i < input.length(); i++) {
        if (input[i] == delimiter) {
            tokens.push_back(token);
            token = "";
        }
        else token += input[i];
    }
    tokens.push_back(token);
    return tokens;
}

int copy_string(char* dst, char* src, int max) {
    int i = -1;
    do {
        i++;
        dst[i] = src[i];
    }
    while (src[i] != 0);
    return i;
}

void saturn_cmd_debug(int num, char* msg, ...) {
#ifdef CMD_DO_DEBUG
    va_list args;
    va_start(args, msg);
    std::cout << "[CMD_ENGINE] ";
    char* current = msg;
    for (int i = 0; i < num; i++) {
        std::cout << current;
        current = va_arg(args, char*);
    }
    va_end(args);
    std::cout << std::endl;
#endif
}

bool saturn_cmd_exec_path(CommandContext* context, std::vector<std::string> tokens, int cmd_iter, int token_iter) {
    bool literals = false;
    int command_count = sizeof(commands) / sizeof(Command);
    for (int i = cmd_iter; i < command_count; i++) {
        int command = (int)(uintptr_t)commands[i];
        if (command == CMDID_LITERALS) literals = true;
        if (command == CMDID_END) break;
        if (token_iter == tokens.size()) {
            if (command == CMDID_EXEC) {
                saturn_cmd_debug(1, "Running a function");
                i++;
                ((CommandFunction)commands[i])(*context);
                return true;
            }
            break;
        }
        if (command == CMDID_LITERAL) {
            i++;
            if (!literals) continue;
            char name[256];
            copy_string(name, (char*)commands[i], 256);
            name[255] = 0;
            saturn_cmd_debug(3, "Literal ", name, "...");
            if (strcmp(name, tokens[token_iter++].c_str()) == 0) {
                i++;
                saturn_cmd_debug(1, "Matched");
                return saturn_cmd_exec_path(context, tokens, i, token_iter);
            }
            saturn_cmd_debug(1, "Didn't match, skipping");
            token_iter--;
            i++;
            int scope = 1;
            int skipped_instr = 0;
            while (i < command_count) {
                command = (int)(uintptr_t)commands[i];
                if (command == CMDID_LITERALS) scope++;
                else if (command == CMDID_END) scope--;
                else {
                    i++;
                    if (command == CMDID_ARG_ARR) i++;
                    if (command == CMDID_LITERAL) scope++;
                }
                if (scope == 0) break;
                i++;
                skipped_instr++;
            }
            saturn_cmd_debug(3, "Skipped ", std::to_string(skipped_instr).c_str(), " instructions");
        }
        else if (command == CMDID_DEBUG) {
            i++;
            char name[256];
            copy_string(name, (char*)commands[i], 256);
            name[255] = 0;
            saturn_cmd_debug(1, name);
        }
        else if (command != CMDID_LITERALS) {
            i++;
            char name[256];
            copy_string(name, (char*)commands[i], 256);
            name[255] = 0;
            saturn_cmd_debug(2, "Argument type ", (command == CMDID_ARG_INT ? "INT" : command == CMDID_ARG_FLT ? "FLT" : command == CMDID_ARG_STR ? "STR" : command == CMDID_ARG_BIN ? "BIN" : "ARR"));
            if (command == CMDID_ARG_INT) {
                if (aliases.find(tokens[token_iter]) == aliases.end()) context->insert({ name, (void*)std::stoi(tokens[token_iter++]) });
                else context->insert({ name, (void*)aliases[tokens[token_iter++]] });
            }
            if (command == CMDID_ARG_FLT) {
                float flt = std::stof(tokens[token_iter++]);
                context->insert({ name, (void*)(*(int*)&flt) });
            }
            if (command == CMDID_ARG_STR) context->insert({ name, (void*)&tokens[token_iter++] });
            if (command == CMDID_ARG_BIN) context->insert({ name, (void*)(tokens[token_iter++] == "true") });
            if (command == CMDID_ARG_ARR) {
                i++;
                char* ptr = (char*)commands[i];
                char* token = (char*)tokens[token_iter++].c_str();
                int index = 0;
                while (ptr[0] != 0) {
                    char str[256];
                    ptr += copy_string(str, ptr, 256) + 1;
                    str[255] = 0;
                    if (strcmp(str, token) == 0) break;
                    index++;
                }
                context->insert({ name, (void*)index });
            }
        }
    }
    return false;
}

void saturn_cmd_eval(std::string command) {
#ifdef CMD_DO_DEBUG
    std::cout << std::endl;
#endif
    saturn_cmd_debug(3, "Running command engine... '", command.c_str(), "'");
    std::vector<std::string> tokens = args(command, ' ');
    CommandContext context = {};
    context.insert({ "cli", (void*)is_cli });
    if (saturn_cmd_exec_path(&context, tokens, 0, 0)) saturn_cmd_debug(1, "Finished successfully");
    else saturn_cmd_debug(1, "Execution failed");
    saturn_cmd_registers_save();
}

void saturn_cmd_eval_file(std::string path) {
    std::ifstream file = std::ifstream(path);
    if (!file.good()) return;
    is_cli = false;
    int size = std::filesystem::file_size(path);
    if (size == 0) return;
    char* data = (char*)malloc(size + 1);
    data[size] = 0;
    file.read(data, size);
    lines = args(std::string(data), '\n');
    free(data);
    line = 0;
    paused = true;
}

void saturn_cmd_clear_registers() {
    for (int i = 0; i < SATURN_CMD_REG_COUNT; i++) {
        registers[i] = 0;
    }
}

void saturn_cmd_pause() {
    if (paused) return;
    paused = true;
}

void saturn_cmd_resume() {
    if (!paused) return;
    paused = false;
    for (;line < lines.size(); line++) {
        if (line < 0) line = 0;
        saturn_cmd_eval(lines[line]);
        if (paused) {
            line++;
            break;
        }
    }
}