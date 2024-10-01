#include "tool/code_inserter_tool.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"

static llvm::cl::OptionCategory ct_category("print-insert-tool options");

static llvm::cl::opt<bool> end_option(
    "end", llvm::cl::desc("Insert print statement at the end of the function."),
    llvm::cl::cat(ct_category));

using ::clang::tooling::CommonOptionsParser;

int main(int argc, const char **argv) {
  auto expected_parser = CommonOptionsParser::create(argc, argv, ct_category);
  if (!expected_parser) {
    llvm::errs() << expected_parser.takeError();
    return -1;
  }

  CommonOptionsParser &options_parser = expected_parser.get();
  sci::CodeInserterTool tool(end_option);

  if (!tool.run(options_parser))
    tool.applySourceChanges();

  return 0;
}
