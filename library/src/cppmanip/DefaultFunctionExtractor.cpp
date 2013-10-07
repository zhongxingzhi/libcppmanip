#include "DefaultFunctionExtractor.hpp"
#include "query/findLocalVariablesRequiredForStmts.hpp"
#include "query/findSelectedStatementsInFunction.hpp"
#include "query/findVariablesDeclaredByAndUsedAfterStmts.hpp"
#include "format/printFunction.hpp"
#include "text/StrictOperationRecorder.hpp"
#include "text/OffsetConverter.hpp"
#include "text/SourceLocationConverter.hpp"
#include "text/convertReplacements.hpp"
#include "io/TextFileOps.hpp"
#include "clangutil/AstGateway.hpp"
#include "FileBasedStatementLocator.hpp"
#include <cppmanip/boundary/ExtractMethodError.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/adaptor/transformed.hpp>

namespace cppmanip
{

std::vector<std::string> getVariableNames(const ast::LocalVariables& variables)
{
    using boost::adaptors::transformed;
    std::vector<std::string> names;
    boost::push_back(names, variables | transformed(std::bind(&ast::LocalVariable::getName, std::placeholders::_1)));
    return names;
}

std::vector<std::string> getArgumentDeclarations(const ast::LocalVariables& variables)
{
    std::vector<std::string> args;
    for (auto d : variables)
        args.push_back(d->getNameWithType());
    return args;
}


std::string getSource(ast::StatementRange stmts)
{
    std::string source;
    for (auto stmt : stmts)
    {
        source += stmt->getSourceCode();
        if (stmt != stmts.back())
            source += stmt->getSourceCodeAfter();
    }
    return source;
}

struct ReplacementFunction
{
    std::string definition, call;
};

ReplacementFunction printReplacementFunctionFromStmts(const std::string& functionName, ast::StatementRange selected)
{
    auto required = query::findLocalVariablesRequiredForStmts(selected);
    return {
        format::printFunctionDefinition("void", functionName, getArgumentDeclarations(required), getSource(selected)),
        format::printFunctionCall(functionName, getVariableNames(required)) };
}

void defineFunction(const std::string& definition, ast::FunctionPtr originalFunction, text::OffsetBasedOperationRecorder& recorder)
{
    recorder.insertTextAt(definition, originalFunction->getDefinitionOffset());
}

ast::SourceOffsetRange getRange(ast::StatementRange stmts)
{
    return { stmts.front()->getRange().getFrom(), stmts.back()->getRange().getTo() };
}

void replaceStmtsWithCall(ast::StatementRange stmts, const std::string& call, text::OffsetBasedOperationRecorder& recorder)
{
    auto range = getRange(stmts);
    recorder.removeTextInRange(range.getFrom(), range.getTo());
    recorder.insertTextAt(call, range.getFrom());
}

boundary::SourceReplacements generateReplacements(ReplacementFunction replacementFunction, StatementLocator::FunctionAndStmts selected, const std::string& filename)
{
    text::OffsetBasedStrictOperationRecorder recorder;
    defineFunction(replacementFunction.definition, selected.function, recorder);
    replaceStmtsWithCall(selected.stmts, replacementFunction.call, recorder);
    text::OffsetConverter offsetCoverter(io::loadTextFromFile(filename));
    return text::convertReplacements(recorder.getReplacements(), [&](unsigned offset) { return offsetCoverter.getLocationFromOffset(offset); });
}

boundary::SourceReplacements DefaultFunctionExtractor::extractFunctionFromSelectionInFile(
    const std::string& functionName, boundary::SourceSelection selection, const std::string& filename)
{
    auto selected = stmtLocator->getSelectedFunctionAndStmts(selection);
    validator->validateStatements(functionName, selected.stmts, selected.function);
    auto replacementFunction = printReplacementFunctionFromStmts(functionName, selected.stmts);
    return generateReplacements(replacementFunction, selected, filename);
}

}
