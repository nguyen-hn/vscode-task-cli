#include "runner.hpp"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <cstdlib>
#include "utils.hpp"

Runner::Runner(const TaskManager& manager) : task_manager_(manager) {}

bool Runner::runTask(const Task* task) {
    if (!task) {
        spdlog::error("Null task provided to runTask");
        return false;
    }
    if (executed.count(task->label)) {
        spdlog::debug("Task '{}' already executed. Skipping.", task->label);
        return true;
    }

    spdlog::debug("Running task: {}", task->label);
    if (!task->options.env.empty()) {
        for(auto& [k,v]: task->options.env) {
            spdlog::debug("Setting environment variable: {}={}", k, v);
#ifdef _WIN32            
            _putenv_s(k.c_str(), v.c_str(), 1);
#else
            setenv(k.c_str(), v.c_str(), 1);
#endif
        }
    }
    for (const auto& dep : task->dependsOn) {
        if (!runTask(dep)) {
            spdlog::error("Failed to run dependency '{}' for task '{}'", dep->label, task->label);
            return false;
        }
    }
    bool ok = runCommand(*task);
    executed.insert(task->label);
    return ok;
}

bool Runner::runCommand(const Task& task) {
    namespace fs = std::filesystem;
    fs::path cwd = task.options.cwd.empty() ? fs::current_path() : fs::path(Utils::resolve_path(task.options.cwd));
    spdlog::debug("Changing working directory to: {}", cwd.string());
    // fs::current_path(cwd);

    std::string command = "cd " + cwd.string() + " && " + task.command;
    for (const auto& arg : task.options.args) {
        command += " " + arg;
    }
    spdlog::debug("Executing command: {}", command);
    std::string resolved_command = Utils::resolve_variable(command);
    spdlog::debug("Resolved command: {}", resolved_command);
    int result = std::system(command.c_str());
    if (result != 0) {
        spdlog::error("Command '{}' failed with exit code {}", command, result);
        return false;
    }
    return true;
}