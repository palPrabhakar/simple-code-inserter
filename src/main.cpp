#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "matchers/matchers.h"
#include "printers/function_printer.h"
#include "utils/utils.h"

#include <clang/AST/DeclBase.h>

using namespace clang::tooling;
using namespace llvm;
using namespace clang::ast_matchers;

int main(int argc, const char **argv) {
  llvm::cl::OptionCategory ctCategory("clang-tool options");
  auto expectedParser = CommonOptionsParser::create(argc, argv, ctCategory);
  if (!expectedParser) {
    llvm::errs() << expectedParser.takeError();
    return -1;
  }

  CommonOptionsParser &optionsParser = expectedParser.get();
  for (auto &sourceFile : optionsParser.getSourcePathList()) {
    if (utils::fileExists(sourceFile) == false) {
      llvm::errs() << "File: " << sourceFile << " does not exist!\n";
      return -1;
    }

    auto sourcetxt = utils::getSourceCode(sourceFile);
    auto compileCommands = optionsParser.getCompilations().getCompileCommands(
        getAbsolutePath(sourceFile));

    std::vector<std::string> compileArgs =
        utils::getCompileArgs(compileCommands);
    compileArgs.push_back("-I" + utils::getClangBuiltInIncludePath(argv[0]));

    printers::FunctionPrinter Printer;
    auto matcher = matchers::get_decl_matchers(sourceFile);
    MatchFinder Finder;
    Finder.addMatcher(matcher, &Printer);

    utils::customRunToolOnCodeWithArgs(newFrontendActionFactory(&Finder)->create(),
                                       sourcetxt, compileArgs, sourceFile);
  }

  return 0;
}
