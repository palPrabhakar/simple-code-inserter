#pragma once

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <clang/AST/Decl.h>
#include "clang/Lex/Lexer.h"

namespace printers {

using namespace clang;
using MatchFinder = clang::ast_matchers::MatchFinder;

class FunctionPrinter : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const clang::CompoundStmt *fn_decl =
            Result.Nodes.getNodeAs<clang::CompoundStmt>("fn")) {
      // llvm::outs() << "Matched function definition outside std: "
                   // << fn_decl->getQualifiedNameAsString() << "\n";
      // fn_decl->dump();
      SourceManager &SrcMgr = *Result.SourceManager;
      LangOptions LangOpts = Result.Context->getLangOpts();
      
      // Get the range of the compound statement
      SourceRange Range = fn_decl->getSourceRange();
      
      // Extract the source text from the range
      llvm::StringRef CompoundStmtText = Lexer::getSourceText(CharSourceRange::getTokenRange(Range), SrcMgr, LangOpts);

      // Print the compound statement as source text
      llvm::outs() << "Compound Statement:\n" << CompoundStmtText << "\n\n";
    }
  }
};

} // namespace printers
