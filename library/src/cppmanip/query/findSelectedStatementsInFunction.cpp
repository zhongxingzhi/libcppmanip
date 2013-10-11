#include "findSelectedStatementsInFunction.hpp"
#include <clang/AST/ASTContext.h>
#include <boost/range/algorithm.hpp>

namespace cppmanip
{
namespace query
{
namespace
{

ast::StatementRange findSelectedStatements(ast::StatementRange stmts, std::function<bool(ast::StatementPtr)> isSelected)
{
    using namespace boost;
    return find_if<return_begin_found>(
        find_if<return_found_end>(stmts, isSelected), [&](const ast::StatementPtr& s) { return !isSelected(s); });
}

}

ast::StatementRange findSelectedStatementsInFunction(
    const ast::Function& decl, std::function<bool(ast::StatementPtr)> isSelected)
{
    auto found = findSelectedStatements(decl.getStatements(), isSelected);
    if (found.size() != 1 || found.front()->getChildren().empty())
        return found;
    return findSelectedStatements(found.front()->getChildren(), isSelected);
}

}
}
