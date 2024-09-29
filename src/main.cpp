#include "clang/Tooling/CommonOptionsParser.h"
#include "llvm/Support/CommandLine.h"
#include "tool/code_inserter_tool.h"

llvm::cl::OptionCategory ctCategory("code-inserter-tool options");

using ::clang::tooling::CommonOptionsParser;

int main(int argc, const char **argv) {
  auto expectedParser = CommonOptionsParser::create(argc, argv, ctCategory);
  if (!expectedParser) {
    llvm::errs() << expectedParser.takeError();
    return -1;
  }

  CommonOptionsParser &optionsParser = expectedParser.get();
  for (auto &source : optionsParser.getSourcePathList()) {
    sci::CodeInserterTool tool(source);
    if(!tool.init(argv[0], optionsParser)) 
      continue;

    if(!tool.run())
      continue;

    tool.applySourceChanges();
  }

  return 0;
}
