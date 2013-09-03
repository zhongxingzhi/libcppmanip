#include <gtest/gtest.h>
#include "../src/CommandLineParser.hpp"

struct CommandLineParserTest : testing::Test
{
    CommandLineParser parser;
};

TEST_F(CommandLineParserTest, should_parse_extract_method_request)
{
    std::vector<const char *> args{ "", "file", "extract_method", "method", "20", "40" };
    OperationRequest req = parser.parse(args.size(), &args[0]);
    ASSERT_EQ("file", req.sourceFilename);
    ASSERT_EQ("method", req.extractedMethodName);
    ASSERT_EQ(20, req.sourceSelection.from);
    ASSERT_EQ(40, req.sourceSelection.to);
}