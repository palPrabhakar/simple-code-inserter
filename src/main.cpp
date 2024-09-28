#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "matchers/matchers.h"
#include "printers/function_printer.h"
#include "utils/utils.h"

#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/Transformer/RewriteRule.h"
#include "clang/Tooling/Transformer/Stencil.h"
#include "clang/Tooling/Transformer/Transformer.h"

#include <clang/AST/DeclBase.h>

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace clang::transformer;

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

    // auto matcher = matchers::get_decl_matchers(sourceFile);
    auto matcher = functionDecl(isDefinition()).bind("fn");
    auto action = changeTo(name("fn"), cat(name("fn"), "_renamed"));
    // auto rewrite_rule = makeRule(matcher, action);
    AtomicChanges Changes;
    auto Consumer = [&](Expected<MutableArrayRef<AtomicChange>> C) {
      if (C) {
        llvm::outs() << "Matches\n";
        Changes.insert(Changes.end(), std::make_move_iterator(C->begin()),
                       std::make_move_iterator(C->end()));
      } else {
        llvm::errs() << "Error generating changes: "
                     << llvm::toString(C.takeError()) << "\n";
      }
    };

    // Transformer transformer(makeRule(functionDecl(hasName("bad")).bind("f"),
    //                                  changeTo(name("f"), cat("good"))),
    //                         std::move(Consumer));
    Transformer transformer(makeRule(matcher, action), std::move(Consumer));
    MatchFinder finder;
    transformer.registerMatchers(&finder);

    utils::customRunToolOnCodeWithArgs(
        newFrontendActionFactory(&finder)->create(), sourcetxt, compileArgs,
        sourceFile);

    auto changed_code =
        applyAtomicChanges(sourceFile, sourcetxt, Changes, ApplyChangesSpec());

    // llvm::outs() << *changed_code << "\n";
    std::error_code EC;
    llvm::raw_fd_ostream OS(sourceFile, EC, llvm::sys::fs::OF_TextWithCRLF);
    if (EC) {
      llvm::errs() << EC.message() << "\n";
      break;
    }
    OS << *changed_code;
  }

  return 0;
}
