#include "code_inserter_tool.h"
#include <algorithm>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Refactoring/AtomicChange.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/Transformer/RewriteRule.h>
#include <clang/Tooling/Transformer/Transformer.h>
#include <memory>
#include <stdexcept>

namespace sci {
using ::clang::ast_matchers::MatchFinder;
using ::clang::tooling::ApplyChangesSpec;
using ::clang::tooling::newFrontendActionFactory;
using ::clang::tooling::Transformer;
using ::clang::transformer::RewriteRule;

ASTEdit CodeInserterTool::getAction(action_t type) {
  switch (type) {
  case action_t::print_at_top:
    return insertBefore(statements("fn"),
                        cat("std::cout<<\"", name("fname"), "\"<<std::endl;"));
  case action_t::include_stmt:
    return addInclude("iostream", clang::transformer::IncludeFormat::Angled);
  case action_t::print_before_rtn:
    return insertBefore(node("rtn"), cat("std::cout<<\"end: ", name("fname"),
                                         "\"<<std::endl;"));
  case action_t::print_end_void:
    return insertAfter(statements("fn"), cat("std::cout<<\"end: ",
                                             name("fname"), "\"<<std::endl;"));
  default:
    throw std::runtime_error("Invalid action type\n");
  }
}

DynTypedMatcher CodeInserterTool::getMatcher(matcher_t type) {
  switch (type) {
  case matcher_t::fn_stmt:
    return compoundStmt(
               hasParent(functionDecl(isDefinition(), isExpansionInMainFile())
                             .bind("fname")))
        .bind("fn");
  case matcher_t::rtn_stmt:
    return returnStmt(
               hasAncestor(functionDecl(isExpansionInMainFile(), isDefinition())
                               .bind("fname")))
        .bind("rtn");
  case matcher_t::fn_stmt_void:
    return compoundStmt(
               hasParent(functionDecl(isDefinition(), isExpansionInMainFile(),
                                      returns(asString("void")))
                             .bind("fname")))
        .bind("fn");
  default:
    throw std::runtime_error("Invalid matcher type\n");
  }
}

bool CodeInserterTool::applySourceChanges() {
  std::unordered_map<std::string, AtomicChanges> source_changes;

  for (auto &change : m_changes) {
    source_changes[change.getFilePath()].push_back(std::move(change));
  }

  for (auto &[source, changes] : source_changes) {
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buffer_err =
        llvm::MemoryBuffer::getFile(source);
    if (!buffer_err) {
      llvm::errs() << "error: failed to open " << source << " for rewriting\n";
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
  std::vector<std::unique_ptr<Transformer>> transformers;
  MatchFinder finder;

  // register rule to insert print statements at the top of the function
  transformers.emplace_back(std::make_unique<Transformer>(
      makeRule(getMatcher(matcher_t::fn_stmt),
               {getAction(action_t::print_at_top),
                getAction(action_t::include_stmt)}),
      getConsumer()));
  transformers.back()->registerMatchers(&finder);

  if (m_end) {
    // register rule to insert print statement before return statements
    transformers.emplace_back(std::make_unique<Transformer>(
        makeRule(getMatcher(matcher_t::rtn_stmt),
                 getAction(action_t::print_before_rtn)),
        getConsumer()));
    transformers.back()->registerMatchers(&finder);

    // register rule to insert print statement at the end of void function
    transformers.emplace_back(std::make_unique<Transformer>(
        makeRule(getMatcher(matcher_t::fn_stmt_void),
                 getAction(action_t::print_end_void)),
        getConsumer()));
    transformers.back()->registerMatchers(&finder);
  }

  clang::tooling::ClangTool tool(options_parser.getCompilations(),
                                 options_parser.getSourcePathList());

  return tool.run(newFrontendActionFactory(&finder).get());
}

} // namespace sci
