#ifndef RUNNER_HPP_
#define RUNNER_HPP_

#include "tasks.hpp"
#include <string.h>
#include <unordered_set>

class Runner {
public:
    Runner(const TaskManager& manager);
    bool runTask(const Task* task);
    bool runCommand(const Task& task);

private:
    const TaskManager& task_manager_;
    std::unordered_set<std::string> executed;
};

#endif // RUNNER_HPP_