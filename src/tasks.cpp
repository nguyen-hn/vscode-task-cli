#include "tasks.hpp"
#include "parser.hpp"
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <functional>
#include <filesystem>

bool TaskManager::load_tasks_from_file(const std::string& workspace_path, const std::string& file_path)
{
    namespace fs = std::filesystem;
    fs::path wsPath(workspace_path);
    fs::path vscodeTask = wsPath / ".vscode" / file_path;
    fs::path rootTask = wsPath / file_path;
    TaskParser parser;

    std::vector<Task> loaded_tasks;
    if (fs::exists(vscodeTask)) {
        spdlog::debug("Found tasks file at: {}", vscodeTask.string());
        loaded_tasks = parser.load_vscode_json(vscodeTask.string());
    } else if (fs::exists(rootTask)) {
        spdlog::debug("Found tasks file at: {}", rootTask.string());
        loaded_tasks = parser.load_vscode_json(rootTask.string());
    } else {
        spdlog::error("Tasks file not found at either location: {} or {}", vscodeTask.string(), rootTask.string());
        return false;
    }
    
    if (loaded_tasks.empty()) {
        spdlog::error("No tasks loaded from file: {}", file_path);
        return false;
    }
    tasks_ = std::move(loaded_tasks);
    resolveDependencies();
    return true;
}

const Task* TaskManager::get_task_by_label(const std::string& label) const 
{
    for(auto& t: tasks_) {
        if (t.label == label) return &t;
    }
    return nullptr;
}

void TaskManager::resolveDependencies() {
    for(auto& task: tasks_) {
        for(auto& depend_label: task.dependsOnLabels) {
            const Task* dep = get_task_by_label(depend_label);
            if (dep) {
                task.dependsOn.push_back(const_cast<Task*>(dep));
            } else {
                spdlog::warn("Task '{}' depends on unknown task '{}'", task.label, depend_label);
            }
        }
    }
}

std::vector<const Task*> TaskManager::topologically_sorted_tasks(const Task* root) const {
    spdlog::debug("Starting topological sort for task: {}", root->label);
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> on_stack;
    std::vector<const Task*> order;
    bool cycleDetected = false;

    std::function<void(const Task*)> dfs = [&](const Task* task) {
        if (!task || cycleDetected) return;
        if (!visited.count(task->label)) {
            visited.insert(task->label);
            on_stack.insert(task->label);

            for(auto dep: task->dependsOn) {
                if (!visited.count(dep->label)) {
                    dfs(dep);
                } else if (on_stack.count(dep->label)) {
                    spdlog::error("Cycle detected: Task '{}' is already on the stack", dep->label);
                    cycleDetected = true;
                    return;
                }
                dfs(dep);
            }
            on_stack.erase(task->label);
            order.push_back(task);
        }
    };
    dfs(root);
    if (cycleDetected) {
        spdlog::error("Cycle detected in task dependencies. Cannot determine execution order.");
        return {};
    }
    // std::reverse(order.begin(), order.end());
    return order;
}