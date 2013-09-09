#ifndef NAIVESTATEMENTLOCATOR_HPP
#define NAIVESTATEMENTLOCATOR_HPP
#include "StatementLocator.hpp"
#include "SourceExtractor.hpp"

class NaiveStatementLocator : public StatementLocator
{
public:
    NaiveStatementLocator(SourceExtractor& sourceExtractor, OffsetRange selection);

    clang::ConstStmtRange findStatementsInFunction(const clang::FunctionDecl& decl);

private:

    SourceExtractor& sourceExtractor;
    OffsetRange selection;

    bool functionDoesNotContainSelection(const clang::FunctionDecl& f);
    clang::ConstStmtRange findStatementsTouchingSelection(const clang::FunctionDecl& func);
    bool selectionOverlapsWithStmt(const clang::Stmt& stmt);
};

#endif // NAIVESTATEMENTLOCATOR_HPP