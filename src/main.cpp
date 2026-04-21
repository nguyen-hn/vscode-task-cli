#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <argparse/argparse.hpp>
#include "parser.hpp"

int main(int argc, char* argv[]) {
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
    
    argparse::ArgumentParser program("vscode-task-runner", "A tool to run tasks defined in .vscode/tasks.json");
    program.add_argument("--task").help("The name of the task to run").required().nargs(1);
    // program.add_argument("--workspace").help("The path to the workspace").required().nargs(1);
    program.add_argument("--help").help("Show this help message and exit").default_value(false).implicit_value(true);

    program.add_description("A tool to run tasks defined in .vscode/tasks.json");
    program.add_epilog("Example usage:\n  vscode-task-runner --task \"build\"");

    try {
        program.parse_args(argc, argv);
        if (program.get<bool>("--help")) {
            std::cout << program << std::endl;
            return 0;
        }

        auto taskName = program.get<std::string>("--task");
        spdlog::debug("Running task: {}", taskName);

        TaskParser parser;
        parser.load_vscode_json("../tests/task.json");

    } catch (const std::exception& e) {
        std::cerr << program << std::endl;
        return 1;
    }
    return 0;
}
