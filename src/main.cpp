#include "tool/code_inserter_tool.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include <llvm/Support/raw_ostream.h>

static llvm::cl::OptionCategory ct_category("simple-code-inserter options");

static llvm::cl::opt<std::string> top_code("top", llvm::cl::desc("<top code>"),
                                           llvm::cl::ZeroOrMore);
llvm::cl::opt<std::string> end_code("end", llvm::cl::desc("<end code>"),
                                    llvm::cl::ZeroOrMore);
llvm::cl::opt<std::string> include("include", llvm::cl::desc("<include header>"),
                                    llvm::cl::ZeroOrMore);

using ::clang::tooling::CommonOptionsParser;

int main(int argc, const char **argv) {
  auto expected_parser = CommonOptionsParser::create(argc, argv, ct_category);
  if (!expected_parser) {
    llvm::errs() << expected_parser.takeError();
    return -1;
  }

  CommonOptionsParser &options_parser = expected_parser.get();
  sci::CodeInserterTool tool(top_code, end_code, include);

  if (!tool.run(options_parser))
    tool.applySourceChanges();

  return 0;
}
