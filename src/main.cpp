#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <argparse/argparse.hpp>
#include "parser.hpp"
#include "utils.hpp"
#include "runner.hpp"
#include "spdlog/cfg/env.h"
#include <unordered_set>

int main(int argc, char* argv[]) {
    auto console = spdlog::stdout_color_mt("console");    
    spdlog::set_level(spdlog::level::debug);
    spdlog::cfg::load_env_levels();
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
    
    argparse::ArgumentParser program("vscode-task-runner", "A tool to run tasks defined in .vscode/tasks.json");
    program.add_argument("--task").help("The name of the task to run").required().nargs(1);
    // program.add_argument("--workspace").help("The path to the workspace").required().nargs(1);
    program.add_argument("--help").help("Show this help message and exit").default_value(false).implicit_value(true);

    program.add_argument("--workspace")
        .help("The path to the workspace (default: current directory)")
        .default_value(std::string(".")).nargs(1);

    program.add_argument("--file")
        .help("The path to the tasks.json file (default: tasks.json)")
        .default_value(std::string("tasks.json")).nargs(1);


    program.add_description("A tool to run tasks defined in .vscode/tasks.json");
    program.add_epilog("Example usage:\n  vscode-task-runner --task \"build\"");

    try {
        program.parse_args(argc, argv);
        if (program.get<bool>("--help")) {
            std::cout << program << std::endl;
            return 0;
        }

        auto taskName = program.get<std::string>("--task");
        auto workspacePath = program.get<std::string>("--workspace");
        auto tasksFilePath = program.get<std::string>("--file");
        spdlog::debug("Running task: {}", taskName);
        spdlog::debug("Workspace path: {}", workspacePath);
        spdlog::debug("Tasks file path: {}", tasksFilePath);

        Utils::setWorkspaceFolder(Utils::resolve_path(workspacePath));

        // std::string fullpath = Utils::resolve_path(workspacePath + "/.vscode/" + tasksFilePath);
        // spdlog::debug("Resolved tasks file path: {}", fullpath);

        TaskManager manager;
        if (!manager.load_tasks_from_file(workspacePath, tasksFilePath)) {
            spdlog::error("Failed to load tasks from file: {} at workspace {} ", tasksFilePath, workspacePath);
            return 1;
        }

        const Task* task = manager.get_task_by_label(taskName);
        if (!task) {
            spdlog::error("Task '{}' not found in file: {}", taskName, tasksFilePath);
            return 1;
        }

        auto order = manager.topologically_sorted_tasks(task);
        if (order.empty()) {
            spdlog::error("Cannot run task: {}", taskName);
            return 1;
        }

        spdlog::debug("Ready to run task: {}", task->label);
        spdlog::debug("========================================");
        std::unordered_set<std::string> executedTasks;
        Runner runner(manager);
        // runner.runTask(task);
        for(auto t: order) {
            if (!executedTasks.count(t->label)) {
                spdlog::debug("Executing task: {}", t->label);
                runner.runTask(t);
                executedTasks.insert(t->label);
            } else {
                spdlog::debug("Skipping already executed task: {}", t->label);
            }
        }        

    } catch (const std::exception& e) {
        // std::cerr << e.what() << std::endl;
        spdlog::error("Error: {}", e.what());
        return 1;
    }
    return 0;
}
