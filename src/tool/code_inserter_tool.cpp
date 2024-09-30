#include "code_inserter_tool.h"
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Refactoring/AtomicChange.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/Transformer/Transformer.h>

namespace sci {
using ::clang::ast_matchers::MatchFinder;
using ::clang::tooling::ApplyChangesSpec;
using ::clang::tooling::newFrontendActionFactory;
using ::clang::tooling::Transformer;

bool CodeInserterTool::applySourceChanges() {
  std::unordered_map<std::string, AtomicChanges> source_changes;

  for(auto &change: m_changes) {
    source_changes[change.getFilePath()].push_back(std::move(change));
  }

  for (auto &[source, changes] : source_changes) {
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buffer_err =
        llvm::MemoryBuffer::getFile(source);
    if (!buffer_err) {
      llvm::errs() << "error: failed to open " << source
                   << " for rewriting\n";
      return true;
    }
    auto result = applyAtomicChanges(source, (*buffer_err)->getBuffer(),
                                     changes, ApplyChangesSpec());

    if (!result) {
      llvm::errs() << toString(result.takeError());
      return false;
    }

    std::error_code ec;
    llvm::raw_fd_ostream os(source, ec, llvm::sys::fs::OF_TextWithCRLF);
    if (ec) {
      llvm::errs() << ec.message() << "\n";
      return false;
    }

    os << *result;
  }

  return true;
}

bool CodeInserterTool::run(CommonOptionsParser &options_parser) {
  Transformer transformer(getRule(), getConsumer());
  MatchFinder finder;
  transformer.registerMatchers(&finder);

  clang::tooling::ClangTool tool(options_parser.getCompilations(),
                                 options_parser.getSourcePathList());

  return tool.run(newFrontendActionFactory(&finder).get());
}

} // namespace sci
