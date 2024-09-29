#pragma once

#include <string>
#include <vector>

#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>

#include "clang/Tooling/CompilationDatabase.h"
#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>

namespace utils
{
    std::vector<std::string> getSyntaxOnlyToolArgs(const std::vector<std::string> &ExtraArgs, llvm::StringRef FileName);

    bool customRunToolOnCodeWithArgs(std::unique_ptr<clang::FrontendAction> frontendAction,
                                     const std::vector<std::string> &args, const llvm::Twine &file);

    bool fileExists(const std::string &file);
    std::vector<std::string> getCompileArgs(const std::vector<clang::tooling::CompileCommand> &compileCommands);
    std::string getClangBuiltInIncludePath(const std::string &fullCallPath);
} // namespace utils
