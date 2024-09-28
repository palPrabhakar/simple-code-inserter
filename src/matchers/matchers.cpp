#include "matchers.h"
#include <clang/ASTMatchers/ASTMatchers.h>

namespace matchers {
const DeclarationMatcher get_decl_matchers(std::string fname) {
  return functionDecl(isDefinition(), unless(isInStdNamespace()),
                      isExpansionInFileMatching(fname))
      .bind("fn");
}
} // namespace matchers
