#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

class Utils {
public:
    static std::string resolve_path(const std::string& path);
    static std::string resolve_variable(const std::string& input_var);

    static void setWorkspaceFolder(const std::string& ws);
private:
    static std::string workspaceFolder;
};

#endif // UTILS_HPP