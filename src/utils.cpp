#include "utils.hpp"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <cstdlib>

std::string Utils::workspaceFolder = std::filesystem::current_path().string();

std::string Utils::resolve_path(const std::string& path) {
    namespace fs = std::filesystem;
    try {
        fs::path p(path);
        if (p.is_absolute()) {
            spdlog::debug("Path '{}' is absolute.", p.string());
            return fs::canonical(p).string();
        } else {
            fs::path cwd = fs::current_path();
            fs::path resolve_path = cwd / p;
            spdlog::debug("Resolving relative path '{}' against current working directory '{}'.", p.string(), cwd.string());            
            return fs::canonical(resolve_path).string();
        }
    } catch (const std::filesystem::filesystem_error& e) {
        spdlog::error("Error resolving path '{}': {}", path, e.what());
        return path; // Return the original path if there's an error
    }
}

std::string Utils::resolve_variable(const std::string& input_var) {
    std::string result = input_var;
    auto pos = result.find("${workspaceFolder}");
    if (pos != std::string::npos) {     
        result.replace(pos, std::string("${workspaceFolder}").length(), workspaceFolder);
    }
    pos = result.find("${env:");
    while (pos != std::string::npos) {
        auto end = result.find("}", pos);
        if (end != std::string::npos) {
            std::string env_var = result.substr(pos + 6, end - pos - 6);
            spdlog::debug("Resolving environment variable: {}", env_var);
            const char* val = std::getenv(env_var.c_str());
            std::string replacement = val ? val : "";
            result.replace(pos, end - pos + 1, replacement);
        }
        pos = result.find("${env:}", pos + 1);
    }
    return result;
}

void Utils::setWorkspaceFolder(const std::string& ws) {
    workspaceFolder = ws;
    spdlog::debug("Workspace folder set to: {}", workspaceFolder);
}