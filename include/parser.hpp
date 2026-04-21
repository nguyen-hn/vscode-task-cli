#ifndef TASKS_PARSER_HPP
#define TASKS_PARSER_HPP
#include <nlohmann/json.hpp>
#include <fstream>
#include <spdlog/spdlog.h>

class TaskParser {
public:
    void load_vscode_json(const std::string& file_path);
    void load_tasks(const std::string& file_path);
private:
    std::string m_file_path;    
};

#endif // TASKS_PARSER_HPP
