#include "tool/code_inserter_tool.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

static llvm::cl::OptionCategory ctCategory("print-insert-tool options");

static llvm::cl::opt<bool> endOption(
    "end", llvm::cl::desc("Insert print statement at the end of the function."),
    llvm::cl::cat(ctCategory));

using ::clang::tooling::CommonOptionsParser;

int main(int argc, const char **argv) {
  auto expectedParser = CommonOptionsParser::create(argc, argv, ctCategory);
  if (!expectedParser) {
    llvm::errs() << expectedParser.takeError();
    return -1;
  }

  CommonOptionsParser &optionsParser = expectedParser.get();
  sci::CodeInserterTool tool;

  if (!tool.run(optionsParser))
    tool.applySourceChanges();

  return 0;
}
