#include "tasks.hpp"
#include <spdlog/spdlog.h>

bool TaskManager::load_tasks_from_file(const std::string& file_path)
{
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
