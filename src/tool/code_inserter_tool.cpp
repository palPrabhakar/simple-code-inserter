#include "code_inserter_tool.h"
#include "../utils/utils.h"
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Refactoring/AtomicChange.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/Transformer/Transformer.h>

namespace sci {
using ::clang::ast_matchers::MatchFinder;
using ::clang::tooling::ApplyChangesSpec;
using ::clang::tooling::getAbsolutePath;
using ::clang::tooling::newFrontendActionFactory;
using ::clang::tooling::Transformer;

bool CodeInserterTool::applySourceChanges() {

  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buffer_err =
      llvm::MemoryBuffer::getFile(m_source);
  if (!buffer_err) {
    llvm::errs() << "error: failed to open " << m_source << " for rewriting\n";
    return true;
  }
  auto result =
      applyAtomicChanges(m_source, (*buffer_err)->getBuffer(), m_changes, ApplyChangesSpec());

  if (!result) {
    llvm::errs() << toString(result.takeError());
    return false;
  }

  std::error_code ec;
  llvm::raw_fd_ostream os(m_source, ec, llvm::sys::fs::OF_TextWithCRLF);
  if (ec) {
    llvm::errs() << ec.message() << "\n";
    return false;
  }

  os << *result;

  return true;
}

bool CodeInserterTool::init(const char *program_name,
                            CommonOptionsParser &optionsParser) {
  if (utils::fileExists(m_source) == false) {
    llvm::errs() << "File: " << m_source << " does not exist!\n";
    return false;
  }

  auto compileCommands = optionsParser.getCompilations().getCompileCommands(
      getAbsolutePath(m_source));

  m_compile_args = utils::getCompileArgs(compileCommands);
  m_compile_args.push_back("-I" +
                           utils::getClangBuiltInIncludePath(program_name));
  return true;
}

bool CodeInserterTool::run() {
  Transformer transformer(getRule(), getConsumer());
  MatchFinder finder;
  transformer.registerMatchers(&finder);

  utils::customRunToolOnCodeWithArgs(
      newFrontendActionFactory(&finder)->create(), m_compile_args, m_source);

  return true;
}

} // namespace sci
