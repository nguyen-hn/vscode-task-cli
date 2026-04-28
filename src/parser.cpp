#include "parser.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <spdlog/spdlog.h>
#include <iostream>
#include "utils.hpp"

using json = nlohmann::json;

std::vector<Task> TaskParser::load_vscode_json(const std::string& file_path)
{
    m_file_path = file_path;
    // spdlog::debug("Loading tasks from: {}", m_file_path);
    std::vector<Task> result;
    std::ifstream file(m_file_path);
    if (!file.is_open()) {
        spdlog::error("Failed to open file: {}", m_file_path);
        return result;
    }
        
    try {
        json json_data = json::parse(file, nullptr, true, true);
        spdlog::debug("Successfully parsed JSON from: {}", m_file_path);

        if (json_data.contains("tasks") && json_data["tasks"].is_array()) {
            for (const auto& task : json_data["tasks"]) {
                Task task_;
                if (task.contains("label") && task["label"].is_string()) {
                    spdlog::debug("Found task: {}", task["label"].get<std::string>());
                    task_.label = task["label"].get<std::string>();
                } else {
                    spdlog::warn("Task without a valid label found in file: {}", m_file_path);
                }
                if (task.contains("type") && task["type"].is_string()) {
                    spdlog::debug("Task details: type {}", task["type"].get<std::string>());
                    task_.type = task["type"].get<std::string>();
                } else {
                    spdlog::warn("Task without a type");
                }
                if (task.contains("args") && task["args"].is_array()) {
                    spdlog::debug("Task details: args array with {} items", task["args"].size());
                    for (const auto& arg : task["args"]) {
                        if (arg.is_string()) {
                            spdlog::debug("Task arg: {}", arg.get<std::string>());
                        } else {
                            spdlog::warn("Non-string arg found in task: {}", task["label"].get<std::string>());
                        }
                    }
                    task_.args = task["args"].get<std::vector<std::string>>();
                }
                if (task.contains("command") && task["command"].is_string()) {
                    spdlog::debug("Task command: {}", task["command"].get<std::string>());
                    task_.command = task["command"].get<std::string>();
                }
                if (task.contains("options")) {
                    auto& opt = task["options"];
                    if (opt.contains("cwd")) {
                        spdlog::debug("option cwd {}", opt["cwd"].get<std::string>());
                        task_.options.cwd = Utils::resolve_variable(opt["cwd"].get<std::string>());
                    }
                    if (opt.contains("args") && opt["args"].is_array()) {                        
                        for(const auto& arg: opt["args"]) {
                            if (arg.is_string()) {
                                spdlog::debug("option arg {}", arg.get<std::string>());                                
                            } else {
                                spdlog::warn("Non-string arg found. Skipping.");
                            }
                        }
                        task_.options.args = opt["args"].get<std::vector<std::string>>();
                    }
                    if (opt.contains("env")) {                        
                        for(auto& [k,v]: opt["env"].items()) {
                            spdlog::debug("option env {}={}", k, v.get<std::string>());
                            task_.options.env[k] = v.get<std::string>();
                        }
                    }
                }
                if (task.contains("dependsOn")) {
                    if (task["dependsOn"].is_array()) {                                            
                        spdlog::debug("Task has dependencies array with {} items", task["dependsOn"].size());
                        for (const auto& dep : task["dependsOn"]) {
                            if (dep.is_string()) {
                                spdlog::debug("Task depends on: {}", dep.get<std::string>());
                                task_.dependsOnLabels.push_back(dep.get<std::string>());
                            } else {
                                spdlog::warn("Invalid dependency format in task: {}", task["label"].get<std::string>());
                            }
                        }
                    } else if (task["dependsOn"].is_string()) {
                        spdlog::debug("Task depends on: {}", task["dependsOn"].get<std::string>());
                        task_.dependsOnLabels.push_back(task["dependsOn"].get<std::string>());
                    } else {
                        spdlog::warn("Invalid dependsOn format in task: {}", task["label"].get<std::string>());
                    }
                }
                result.push_back(std::move(task_));
            }
        } else {
            spdlog::warn("No tasks array found in file: {}", m_file_path);
        }
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("JSON parsing error in file {}: {}", m_file_path, e.what());        
    }
    return result;
}
