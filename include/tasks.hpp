#ifndef TASKS_HPP_
#define TASKS_HPP_

#include <string>
#include <vector>
#include <map>

struct CommandOption {
    std::string cwd;
    std::map<std::string, std::string> env;
    std::string shell;
    std::vector<std::string> args;
};

struct Task {
    std::string label;
    std::string type;
    std::string command;
    std::vector<std::string> args;
    CommandOption options;
    std::vector<std::string> dependsOnLabels;
    std::vector<Task*> dependsOn;
};

class TaskManager {
public:
    bool load_tasks_from_file(const std::string& file_path);
    const Task* get_task_by_label(const std::string& label) const;
    void resolveDependencies();
private:
    std::vector<Task> tasks_;
};

#endif // TASKS_HPP_
