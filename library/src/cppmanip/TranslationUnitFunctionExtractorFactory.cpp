#include "TranslationUnitFunctionExtractorFactory.hpp"
#include "legacy/DelayedFunctionExtractor.hpp"
#include "TranslationUnitFunctionExtractor.hpp"
#include "query/getFunctionFromAstInSelection.hpp"
#include "query/findSelectedStatementsInFunction.hpp"
#include "query/getStmtLocationRange.hpp"
#include "query/findLocalVariablesRequiredForStmts.hpp"
#include "query/findVariablesDeclaredByAndUsedAfterStmts.hpp"
#include "query/getFunctionStatements.hpp"
#include "format/printFunction.hpp"

namespace cppmanip
{

clangutil::HandleTranslationUnit TranslationUnitFunctionExtractorFactory::createFunctionExtractor(
    const std::string& extractedMethodName, ast::SourceOffsetRange selection, text::OffsetBasedTextModifier& sourceOperations)
{
    using std::bind;
    using namespace std::placeholders;
    struct WithDeps
    {
        TranslationUnitFunctionExtractor functionExtractor;
        WithDeps(const std::string& extractedMethodName, ast::SourceOffsetRange selection, text::OffsetBasedTextModifier& sourceOperations)
            : functionExtractor(
                bind(query::getFunctionFromAstInSelection, _1, selection, [](clang::FunctionDecl& f) { return query::getFunctionStatements(f, query::getStmtOffsetRange); }),
                [=](ast::FunctionPtr decl) {
                    return query::findSelectedStatementsInFunction(*decl, [=](ast::StatementPtr s) {
                        auto r = s->getRange();
                        math::PositionRange<ast::SourceOffset> r1{r.getFrom(), r.getTo()}, r2{selection.getFrom(), selection.getTo()};
                        return r1.overlapsWith(r2);
                    });
                },
                [&]() {
                    return std::make_shared<legacy::DelayedFunctionExtractor>(
                        sourceOperations, format::printFunctionCall, format::printFunctionDefinition,
                        query::findLocalVariablesRequiredForStmts,
                        query::findVariablesDeclaredByAndUsedAfterStmts, extractedMethodName);
                }) { }
    };
    auto withDeps = std::make_shared<WithDeps>(extractedMethodName, selection, sourceOperations);
    std::shared_ptr<TranslationUnitFunctionExtractor> handler{withDeps, &withDeps->functionExtractor};
    return [=](clang::ASTContext& ctx){ handler->handleTranslationUnit(ctx); };
}

}

