#include "parser.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <spdlog/spdlog.h>
#include <iostream>

using json = nlohmann::json;

void TaskParser::load_vscode_json(const std::string& file_path)
{
    m_file_path = file_path;
    spdlog::debug("Loading tasks from: {}", m_file_path);
    std::ifstream file(m_file_path);
    if (!file.is_open()) {
        spdlog::error("Failed to open file: {}", m_file_path);
        return;
    }
        
    try {
        json json_data = json::parse(file, nullptr, true, true);
        spdlog::debug("Successfully parsed JSON from: {}", m_file_path);

        if (json_data.contains("tasks") && json_data["tasks"].is_array()) {
            for (const auto& task : json_data["tasks"]) {
                if (task.contains("label") && task["label"].is_string()) {
                    spdlog::debug("Found task: {}", task["label"].get<std::string>());
                } else {
                    spdlog::warn("Task without a valid label found in file: {}", m_file_path);
                }
                if (task.contains("type") && task["type"].is_string()) {
                    spdlog::debug("Task details: type {}", task["type"].get<std::string>());
                } else {
                    spdlog::warn("Task without a type");
                }
                if (task.contains("command") && task["command"].is_string()) {
                    spdlog::debug("Task command: {}", task["command"].get<std::string>());
                }
                if (task.contains("options")) {
                    auto& opt = task["options"];
                    if (opt.contains("cwd")) {
                        spdlog::debug("option cwd {}", opt["cwd"].get<std::string>());
                    }
                    if (opt.contains("args") && opt["args"].is_array()) {
                        std::cout << opt["args"] << std::endl;
                    }
                    /*opt["cwd"] = opt["cwd"].get<std::string>();*/
                    /*opt["shell"] = opt["shell"].get<std::string>();*/
                    /*if (opt.contains("env")) {*/
                    /*    for(auto& [k,v]: opt["env"].items()) {*/
                    /*        options["env"][k] = v.get<std::string>();*/
                    /*    }*/
                    /*}*/
                }
                if (task.contains("dependsOn")) {
                    std::cout << task["dependsOn"] << std::endl;
                }
            }
        } else {
            spdlog::warn("No tasks array found in file: {}", m_file_path);
        }
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("JSON parsing error in file {}: {}", m_file_path, e.what());
    }
}

void TaskParser::load_tasks(const std::string& file_path) {}
