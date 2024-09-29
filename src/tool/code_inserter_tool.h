#pragma once
#include "clang/Tooling/Refactoring/AtomicChange.h"
#include "clang/Tooling/Transformer/RewriteRule.h"
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/CharInfo.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Transformer/Stencil.h>
#include <llvm/ADT/StringRef.h>

namespace sci {
using ::clang::ast_matchers::compoundStmt;
using ::clang::ast_matchers::functionDecl;
using ::clang::ast_matchers::hasParent;
using ::clang::ast_matchers::isDefinition;
using ::clang::ast_matchers::isExpansionInFileMatching;
using ::clang::ast_matchers::isInStdNamespace;
using ::clang::ast_matchers::unless;
using ::clang::tooling::AtomicChange;
using ::clang::tooling::AtomicChanges;
using ::clang::tooling::CommonOptionsParser;
using ::clang::transformer::ASTEdit;
using ::clang::transformer::cat;
using ::clang::transformer::makeRule;
using ::clang::transformer::name;
using ::clang::transformer::statements;

class CodeInserterTool {
public:
  CodeInserterTool(std::string source) : m_source(source) {}
  bool run();
  bool applySourceChanges();
  bool init(const char *, CommonOptionsParser &);

private:
  std::string m_source;
  std::string m_code;
  AtomicChanges m_changes;
  std::vector<std::string> m_compile_args;

  auto getRule() {
    auto actions = {
        insertBefore(statements("fn"),
                     cat("std::cout<<\"", name("fname"), "\"<<std::endl;")),

        addInclude("iostream", clang::transformer::IncludeFormat::Angled)};
    auto matcher =
        compoundStmt(
            hasParent(functionDecl(isDefinition(), unless(isInStdNamespace()),
                                   isExpansionInFileMatching(m_source))
                          .bind("fname")))
            .bind("fn");
    return makeRule(std::move(matcher), std::move(actions));
  }

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
