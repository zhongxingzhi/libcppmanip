#ifndef CPPMANIP_F6D4A1972E414BE5B79D1D592783B2D2_HPP
#define CPPMANIP_F6D4A1972E414BE5B79D1D592783B2D2_HPP
#include "StatementExtractor.hpp"
#include "ast/Function.hpp"
#include <clang/AST/ASTContext.h>

namespace cppmanip
{

class TranslationUnitFunctionExtractor
{
public:
    typedef std::function<ast::FunctionPtr(clang::ASTContext&)> LocateFunction;
    typedef std::function<ast::StatementRange(ast::FunctionPtr)> LocateStatements;
    typedef std::function<StatementExtractorPtr()> CreateStatementExtractor;
    TranslationUnitFunctionExtractor(LocateFunction locateFunction, LocateStatements locateStatements, CreateStatementExtractor createStmtExtractor)
        : locateFunction(locateFunction), locateStatements(locateStatements), createStmtExtractor(createStmtExtractor) { }
    virtual void handleTranslationUnit(clang::ASTContext& ctx);
private:
    LocateFunction locateFunction;
    LocateStatements locateStatements;
    CreateStatementExtractor createStmtExtractor;
};

}
#endif // CPPMANIP_F6D4A1972E414BE5B79D1D592783B2D2_HPP
