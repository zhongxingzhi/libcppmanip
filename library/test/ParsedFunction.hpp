#ifndef CPPMANIP_9DCB109EF1064760828DE63EE74C3314_HPP
#define CPPMANIP_9DCB109EF1064760828DE63EE74C3314_HPP
#include "Condition.hpp"
#include <clang/AST/StmtIterator.h>
#include <clang/AST/ASTContext.h>
#include <clang/Basic/SourceManager.h>

namespace cppmanip
{
namespace test
{

class ParsedFunction : boost::noncopyable
{
public:
    ParsedFunction(const std::string& source);
    ~ParsedFunction();
    clang::StmtRange stmts();
    clang::ASTContext& getASTContext() { return *astContext; }
private:
    Condition parsed, canFinish;
    clang::StmtRange range;
    clang::ASTContext *astContext;
    boost::thread thread;
};

}
}
#endif // CPPMANIP_9DCB109EF1064760828DE63EE74C3314_HPP
