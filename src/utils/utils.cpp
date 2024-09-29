#include "utils.h"
#include <fstream>

namespace utils {

std::vector<std::string>
getSyntaxOnlyToolArgs(const std::vector<std::string> &extraArgs,
                      llvm::StringRef fileName) {
  std::vector<std::string> args;

  args.push_back("insert-print-stmts");
  args.push_back("-fsyntax-only");

  args.insert(args.end(), extraArgs.begin(), extraArgs.end());
  args.push_back(fileName.str());

  return args;
}

bool customRunToolOnCodeWithArgs(
    std::unique_ptr<clang::FrontendAction> frontendAction,
    const std::vector<std::string> &args, const llvm::Twine &fileName) {
  llvm::SmallString<16> fileNameStorage;
  llvm::StringRef fileNameRef =
      fileName.toNullTerminatedStringRef(fileNameStorage);

  llvm::IntrusiveRefCntPtr<clang::FileManager> files(
      new clang::FileManager(clang::FileSystemOptions()));
  clang::tooling::ToolInvocation invocation(
      getSyntaxOnlyToolArgs(args, fileNameRef), std::move(frontendAction),
      files.get());

  return invocation.run();
}

bool fileExists(const std::string &file) { return std::ifstream(file).good(); }

std::vector<std::string> getCompileArgs(
    const std::vector<clang::tooling::CompileCommand> &compileCommands) {
  std::vector<std::string> compileArgs;

  for (auto &cmd : compileCommands) {
    for (auto &arg : cmd.CommandLine)
      compileArgs.push_back(arg);
  }

  if (compileArgs.empty() == false) {
    compileArgs.erase(begin(compileArgs));
    compileArgs.pop_back();
  }

  return compileArgs;
}

std::string getClangBuiltInIncludePath(const std::string &fullCallPath) {
  auto currentPath = fullCallPath;
  currentPath.erase(currentPath.rfind("/"));

  std::string line;
  std::ifstream file(currentPath + "/builtInInclude.path");
  std::getline(file, line);

  return line;
}

} // namespace utils
