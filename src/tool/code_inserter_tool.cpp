#include "code_inserter_tool.h"
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Refactoring/AtomicChange.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/Transformer/RewriteRule.h>
#include <clang/Tooling/Transformer/Stencil.h>
#include <clang/Tooling/Transformer/Transformer.h>
#include <format>
#include <memory>
#include <stdexcept>

namespace clang {
namespace ast_matchers {
AST_POLYMORPHIC_MATCHER(isMacroExpansion,
                        AST_POLYMORPHIC_SUPPORTED_TYPES(Decl, Stmt, TypeLoc)) {
  auto &SourceManager = Finder->getASTContext().getSourceManager();
  return SourceManager.isMacroBodyExpansion(Node.getBeginLoc());
}
} // namespace ast_matchers
} // namespace clang

namespace sci {
using ::clang::ast_matchers::isMacroExpansion;
using ::clang::ast_matchers::MatchFinder;
using ::clang::tooling::ApplyChangesSpec;
using ::clang::tooling::newFrontendActionFactory;
using ::clang::tooling::Transformer;
using ::clang::transformer::RewriteRule;

std::vector<Stencil> parseCode(std::string code_str) {
  std::vector<Stencil> code;
  size_t start = 0;
  size_t end = code_str.find('{');
  while (end != std::string::npos) {
    code.push_back(cat(code_str.substr(start, end - start)));
    start = end + 1;
    end = code_str.find('}', start);
    if (end != std::string::npos) {
      auto val = code_str.substr(start, end - start);
      if (val != "name") {
        throw std::runtime_error(std::format("Invalid option: {}\n", val));
      }
      code.push_back(cat(name("fname")));
      start = end + 1;
    } else {
      throw std::runtime_error("Expected } after {\n");
    }
    end = code_str.find('{', start);
  }
  code.push_back(cat(code_str.substr(start)));
  return code;
}

ASTEdit CodeInserterTool::getAction(action_t type) {
  switch (type) {
  case action_t::print_at_top:
    return insertBefore(statements("fn"), catVector(m_top_code));
  case action_t::include_stmt:
    return addInclude(m_include, m_quoted ? Quoted : Angled);
  case action_t::print_before_rtn:
    return insertBefore(node("rtn"), catVector(m_end_code));
  case action_t::print_before_rtn_single_Ifelse:
    return changeTo(node("rtn"),
                    cat("{", catVector(m_end_code), node("rtn"), "}"));
  case action_t::print_end_void:
    return insertAfter(statements("fn"), catVector(m_end_code));
  default:
    throw std::runtime_error("Invalid action type\n");
  }
}

DynTypedMatcher CodeInserterTool::getMatcher(matcher_t type) {
  auto fn_decl = functionDecl(isDefinition(), isExpansionInMainFile(),
                              unless(isImplicit()), unless(isMacroExpansion()),
                              unless(isTemplateInstantiation()))
                     .bind("fname");

  switch (type) {
  case matcher_t::fn_stmt:
    return compoundStmt(hasParent(fn_decl)).bind("fn");
  case matcher_t::rtn_stmt:
    return returnStmt(hasAncestor(fn_decl)).bind("rtn");
  case matcher_t::fn_stmt_void:
    return compoundStmt(
               hasParent(functionDecl(isDefinition(), isExpansionInMainFile(),
                                      returns(asString("void")),
                                      unless(isImplicit()),
                                      unless(isMacroExpansion()),
                                      unless(isTemplateInstantiation()))
                             .bind("fname")))
        .bind("fn");
  case matcher_t::rtn_stmt_ifStmt:
    return returnStmt(
               hasAncestor(ifStmt(unless(hasDescendant(compoundStmt())))),
               hasAncestor(fn_decl))
        .bind("rtn");
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

  if (!m_top_code.empty()) {
    // register rule to insert print statements at the top of the function
    transformers.emplace_back(std::make_unique<Transformer>(
        makeRule(getMatcher(matcher_t::fn_stmt),
                 getAction(action_t::print_at_top)),
        getConsumer()));
    transformers.back()->registerMatchers(&finder);
  }

  if (!m_include.empty()) {
    // add include statement
    transformers.emplace_back(std::make_unique<Transformer>(
        makeRule(getMatcher(matcher_t::fn_stmt),
                 getAction(action_t::include_stmt)),
        getConsumer()));
    transformers.back()->registerMatchers(&finder);
  }

  if (!m_end_code.empty()) {
    // register rule to insert print statement before return statements
    transformers.emplace_back(std::make_unique<Transformer>(
        clang::transformer::applyFirst(
            {makeRule(getMatcher(matcher_t::rtn_stmt_ifStmt),
                      getAction(action_t::print_before_rtn_single_Ifelse)),
             makeRule(getMatcher(matcher_t::rtn_stmt),
                      getAction(action_t::print_before_rtn))}),
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
