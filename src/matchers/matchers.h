#pragma once
#include "clang/ASTMatchers/ASTMatchers.h"

#include <string>

namespace matchers {
using namespace clang::ast_matchers;
const DeclarationMatcher get_decl_matchers(std::string fname);
} // namespace matchers
// DeclarationMatcher function_matcher
