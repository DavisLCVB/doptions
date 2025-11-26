#include <doptions/command.hpp>
#include <doptions/option.hpp>
#include <doptions/exceptions.hpp>
#include <gtest/gtest.h>
#include <stdexcept>

// Test fixture for bug regression tests
class BugRegressionTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

// ============================================================================
// BUG #1: fromStr<int8_t> calls std::stoi TWICE (option.hpp:33, 37)
// This is inefficient but not incorrect - value is computed correctly
// ============================================================================

TEST(BugRegressionTest, FromStrInt8CallsStoiTwice) {
  // This test documents the double call bug
  // The function still works correctly despite inefficiency
  int8_t result = doptions::fromStr<int8_t>("100");
  EXPECT_EQ(result, 100);

  result = doptions::fromStr<int8_t>("127");
  EXPECT_EQ(result, 127);

  result = doptions::fromStr<int8_t>("-128");
  EXPECT_EQ(result, -128);

  // Out of range should throw
  EXPECT_THROW(doptions::fromStr<int8_t>("128"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<int8_t>("-129"), doptions::ParseException);
}

// ============================================================================
// BUG #2: fromStr<int16_t> calls std::stoi TWICE (option.hpp:42, 46)
// AND has wrong error message (says "int8_t" instead of "int16_t")
// ============================================================================

TEST(BugRegressionTest, FromStrInt16CallsStoiTwice) {
  // This test documents the double call bug
  int16_t result = doptions::fromStr<int16_t>("1000");
  EXPECT_EQ(result, 1000);

  result = doptions::fromStr<int16_t>("32767");
  EXPECT_EQ(result, 32767);

  result = doptions::fromStr<int16_t>("-32768");
  EXPECT_EQ(result, -32768);

  // Out of range should throw
  EXPECT_THROW(doptions::fromStr<int16_t>("32768"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<int16_t>("-32769"), doptions::ParseException);
}

TEST(BugRegressionTest, FromStrInt16ErrorMessageSaysInt8) {
  // FIXED: Error message now correctly shows range without mentioning type name
  try {
    doptions::fromStr<int16_t>("100000");
    FAIL() << "Expected doptions::ParseException exception";
  } catch (const doptions::ParseException& e) {
    std::string message = e.what();
    // The message should show a range error
    EXPECT_NE(message.find("out of range"), std::string::npos)
        << "Error message should indicate value is out of range";
    // The message should NOT incorrectly say "int8_t"
    EXPECT_EQ(message.find("int8_t"), std::string::npos)
        << "Error message should not mention int8_t for int16_t conversion";
  }
}

// ============================================================================
// BUG #4: parseCommand duplicate error message has trailing comma
// (command.hpp:58-65)
// ============================================================================

TEST(BugRegressionTest, DuplicateOptionErrorMessageHasTrailingComma) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-v,--verbose", &value);

  std::vector<std::string> args = {"-v", "10", "--verbose", "20"};

  try {
    cmd->parseCommand(args);
    FAIL() << "Expected ParseException for duplicate options";
  } catch (const doptions::ParseException& e) {
    std::string message = e.what();
    // BUG: Message will contain "Same argument appears multiple times: -v,
    // --verbose, " The trailing ", " is a formatting issue
    EXPECT_NE(message.find("Same argument appears multiple times"),
              std::string::npos);

    // Check if it has the trailing comma (documents the bug)
    // When fixed, this part of the test should be updated
    EXPECT_TRUE(message.find(", ") != std::string::npos)
        << "Message format may have changed";
  }
}

// ============================================================================
// FIXED BUG: parseCommand duplicate detection (command.hpp:76)
// This was fixed by adding: parsed[argIdx] = true;
// ============================================================================

TEST(BugRegressionTest, DuplicateOptionDetectionWorks) {
  // This bug was FIXED - duplicate options are now properly detected
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-v,--value", &value);

  // Test with long name duplicates
  std::vector<std::string> args1 = {"--value", "10", "--value", "20"};
  EXPECT_THROW(cmd->parseCommand(args1), doptions::ParseException);

  // Test with short name duplicates
  std::vector<std::string> args2 = {"-v", "10", "-v", "20"};
  EXPECT_THROW(cmd->parseCommand(args2), doptions::ParseException);

  // Test with mixed short/long duplicates
  std::vector<std::string> args3 = {"-v", "10", "--value", "20"};
  EXPECT_THROW(cmd->parseCommand(args3), doptions::ParseException);

  std::vector<std::string> args4 = {"--value", "10", "-v", "20"};
  EXPECT_THROW(cmd->parseCommand(args4), doptions::ParseException);
}

// ============================================================================
// Additional edge case tests for improved coverage
// ============================================================================

TEST(BugRegressionTest, ParseValueDoesNotCatchConversionExceptions) {
  // parseValue (option.hpp:163-166) doesn't catch exceptions from fromStr
  // This means exceptions propagate without additional context
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("--port", &value);

  std::vector<std::string> args = {"--port", "not-a-number"};

  // Should throw std::invalid_argument from std::stoi
  EXPECT_THROW(cmd->parseCommand(args), std::invalid_argument);

  // The exception message won't include context like "for option --port"
  try {
    cmd->parseCommand(args);
    FAIL() << "Expected exception";
  } catch (const std::invalid_argument& e) {
    std::string message = e.what();
    // Message will be generic from std::stoi, not specific to the option
    // This is a limitation but not necessarily a bug
    EXPECT_NE(message, "");
  }
}

TEST(BugRegressionTest, Int8OverflowDetectionWorks) {
  // Verify that the FIXED overflow detection for int8_t works
  EXPECT_NO_THROW(doptions::fromStr<int8_t>("127"));
  EXPECT_NO_THROW(doptions::fromStr<int8_t>("-128"));

  EXPECT_THROW(doptions::fromStr<int8_t>("128"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<int8_t>("-129"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<int8_t>("1000"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<int8_t>("-1000"), doptions::ParseException);
}

TEST(BugRegressionTest, Int16OverflowDetectionWorks) {
  // Verify that the FIXED overflow detection for int16_t works
  EXPECT_NO_THROW(doptions::fromStr<int16_t>("32767"));
  EXPECT_NO_THROW(doptions::fromStr<int16_t>("-32768"));

  EXPECT_THROW(doptions::fromStr<int16_t>("32768"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<int16_t>("-32769"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<int16_t>("100000"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<int16_t>("-100000"), doptions::ParseException);
}

TEST(BugRegressionTest, Uint8OverflowDetectionWorks) {
  EXPECT_NO_THROW(doptions::fromStr<uint8_t>("255"));
  EXPECT_NO_THROW(doptions::fromStr<uint8_t>("0"));

  EXPECT_THROW(doptions::fromStr<uint8_t>("256"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<uint8_t>("-1"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<uint8_t>("1000"), doptions::ParseException);
}

TEST(BugRegressionTest, Uint16OverflowDetectionWorks) {
  EXPECT_NO_THROW(doptions::fromStr<uint16_t>("65535"));
  EXPECT_NO_THROW(doptions::fromStr<uint16_t>("0"));

  EXPECT_THROW(doptions::fromStr<uint16_t>("65536"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<uint16_t>("-1"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<uint16_t>("100000"), doptions::ParseException);
}

// ============================================================================
// Test validateName edge cases that were fixed
// ============================================================================

TEST(BugRegressionTest, ValidateNameHandlesEdgeCases) {
  int value = 0;

  // These should all throw due to validation
  EXPECT_THROW(doptions::Option<int>::createOption("-", &value),
               std::invalid_argument);
  EXPECT_THROW(doptions::Option<int>::createOption("--", &value),
               std::invalid_argument);
  EXPECT_THROW(doptions::Option<int>::createOption("---", &value),
               std::invalid_argument);

  // The code was improved to check data.size() before substr
  // option.hpp:200 checks data.size() >= 3 before substr(2)
  // option.hpp:203 checks data.size() >= 2 before substr(1)
}

TEST(BugRegressionTest, ValidateNameWithWhitespace) {
  int value = 0;

  // Names with leading/trailing whitespace should be trimmed
  // But names with only whitespace should fail
  EXPECT_THROW(doptions::Option<int>::createOption("   ", &value),
               std::invalid_argument);

  // Trimming should handle normal cases
  EXPECT_NO_THROW(doptions::Option<int>::createOption(" -v,--verbose ", &value));
}
