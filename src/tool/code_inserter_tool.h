#pragma once
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/Transformer/RewriteRule.h"
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchersInternal.h>
#include <clang/Basic/CharInfo.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Transformer/Stencil.h>
#include <initializer_list>
#include <llvm/ADT/StringRef.h>

namespace sci {
using ::clang::ast_matchers::asString;
using ::clang::ast_matchers::compoundStmt;
using ::clang::ast_matchers::functionDecl;
using ::clang::ast_matchers::hasAncestor;
using ::clang::ast_matchers::hasParent;
using ::clang::ast_matchers::isDefinition;
using ::clang::ast_matchers::isExpansionInMainFile;
using ::clang::ast_matchers::isInStdNamespace;
using ::clang::ast_matchers::returns;
using ::clang::ast_matchers::returnStmt;
using ::clang::ast_matchers::unless;
using ::clang::ast_matchers::internal::DynTypedMatcher;
using ::clang::tooling::AtomicChange;
using ::clang::tooling::AtomicChanges;
using ::clang::tooling::CommonOptionsParser;
using ::clang::transformer::ASTEdit;
using ::clang::transformer::cat;
using ::clang::transformer::makeRule;
using ::clang::transformer::name;
using ::clang::transformer::node;
using ::clang::transformer::statements;
using ::clang::ast_matchers::hasDescendant;
using ::clang::ast_matchers::ifStmt;

class CodeInserterTool {
public:
  CodeInserterTool(bool end = false) : m_end(end) {}
  bool run(CommonOptionsParser &);
  bool applySourceChanges();

private:
  AtomicChanges m_changes;
  bool m_end;

  enum class action_t {
    print_at_top,
    print_before_rtn,
    print_end_void,
    print_before_rtn_single_Ifelse,
    include_stmt
  };

  ASTEdit getAction(enum action_t);

  enum class matcher_t { fn_stmt, rtn_stmt, rtn_stmt_ifStmt, fn_stmt_void };

  DynTypedMatcher getMatcher(matcher_t);

  auto getConsumer() {
    return [&](llvm::Expected<llvm::MutableArrayRef<AtomicChange>> C) {
      if (C) {
        m_changes.insert(m_changes.end(), std::make_move_iterator(C->begin()),
                         std::make_move_iterator(C->end()));
      } else {
        llvm::errs() << "Error generating changes: "
                     << llvm::toString(C.takeError()) << "\n";
      }
    };
  }
};
} // namespace sci
