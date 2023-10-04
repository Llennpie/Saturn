#ifndef SaturnCommand
#define SaturnCommand

#include <string>
#include <map>
#include <vector>

typedef void* Command;
typedef std::map<std::string, void*> CommandContext;
typedef void (*CommandFunction)(CommandContext context);

extern std::vector<std::pair<int, int>> loops;
extern int line;
extern int* registers;

extern bool is_cli;

extern std::map<std::string, int> aliases;

extern void saturn_cmd_eval(std::string command);
extern void saturn_cmd_eval_file(std::string path);
extern void saturn_cmd_clear_registers();
extern void saturn_cmd_pause();
extern void saturn_cmd_resume();

#define CMDID_LITERALS 0
#define CMDID_LITERAL  1
#define CMDID_ARG_INT  2
#define CMDID_ARG_FLT  3
#define CMDID_ARG_STR  4
#define CMDID_ARG_BIN  5
#define CMDID_ARG_ARR  6
#define CMDID_EXEC     7
#define CMDID_END      8
#define CMDID_DEBUG    9

#define CMD_LITERALS()         (void*)0
#define CMD_LITERAL(name)      (void*)1, (void*)name
#define CMD_ARG_INT(name)      (void*)2, (void*)name
#define CMD_ARG_FLT(name)      (void*)3, (void*)name
#define CMD_ARG_STR(name)      (void*)4, (void*)name
#define CMD_ARG_BIN(name)      (void*)5, (void*)name
#define CMD_ARG_ARR(name, arr) (void*)6, (void*)name, (void*)arr 
#define CMD_EXEC(func)         (void*)7, (void*)func
#define CMD_END()              (void*)8
#define CMD_DEBUG(msg)         (void*)9, (void*)msg

#define SATURN_CMD_REG_COUNT 128

#endif