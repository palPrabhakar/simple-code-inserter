#pragma once

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include <clang/AST/Decl.h>

namespace printers {

using MatchFinder = clang::ast_matchers::MatchFinder;

class FunctionPrinter : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const clang::FunctionDecl *fn_decl =
            Result.Nodes.getNodeAs<clang::FunctionDecl>("fn")) {
      llvm::outs() << "Matched function definition outside std: "
                   << fn_decl->getQualifiedNameAsString() << "\n";
      // fn_decl->dump();
    }
  }
};

} // namespace printers
