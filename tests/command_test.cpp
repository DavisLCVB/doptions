#include <gtest/gtest.h>
#include <doptions/command.hpp>
#include <doptions/exceptions.hpp>
#include <stdexcept>
#include <string>
#include <vector>

// Test fixture for Command tests
class CommandTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

// ============================================================================
// Basic Command Creation Tests
// ============================================================================

TEST_F(CommandTest, CreateCommandWithValidName) {
  EXPECT_NO_THROW({
    auto cmd = doptions::Command::createCommand("test");
    auto cmd2 = doptions::Command::createCommand("mycommand");
    auto cmd3 = doptions::Command::createCommand("my-command");
  });
}

TEST_F(CommandTest, CreateCommandWithInvalidName) {
  // Names starting with number
  EXPECT_THROW(doptions::Command::createCommand("1test"),
               std::invalid_argument);

  // Empty name
  EXPECT_THROW(doptions::Command::createCommand(""), std::invalid_argument);

  // Name too short (less than 4 chars for long names)
  EXPECT_THROW(doptions::Command::createCommand("ab"), std::invalid_argument);
}

// ============================================================================
// addOption Tests
// ============================================================================

TEST_F(CommandTest, AddSingleOption) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;

  EXPECT_NO_THROW({ cmd->addOption("-v,--value", &value); });
}

TEST_F(CommandTest, AddMultipleOptions) {
  auto cmd = doptions::Command::createCommand("test");
  int port = 0;
  std::string host;
  bool verbose = false;

  EXPECT_NO_THROW({
    cmd->addOption("-p,--port", &port);
    cmd->addOption("-h,--host", &host);
    cmd->addOption("-v,--verbose", &verbose);
  });
}

TEST_F(CommandTest, AddOptionsDifferentTypes) {
  auto cmd = doptions::Command::createCommand("test");
  int8_t i8 = 0;
  int16_t i16 = 0;
  int32_t i32 = 0;
  int64_t i64 = 0;
  uint8_t u8 = 0;
  uint16_t u16 = 0;
  uint32_t u32 = 0;
  uint64_t u64 = 0;
  float f = 0.0f;
  double d = 0.0;
  bool b = false;
  std::string s;

  EXPECT_NO_THROW({
    cmd->addOption("--int8", &i8);
    cmd->addOption("--int16", &i16);
    cmd->addOption("--int32", &i32);
    cmd->addOption("--int64", &i64);
    cmd->addOption("--uint8", &u8);
    cmd->addOption("--uint16", &u16);
    cmd->addOption("--uint32", &u32);
    cmd->addOption("--uint64", &u64);
    cmd->addOption("--float", &f);
    cmd->addOption("--double", &d);
    cmd->addOption("--bool", &b);
    cmd->addOption("--string", &s);
  });
}

// ============================================================================
// parseCommand Tests - Simple Cases
// ============================================================================

TEST_F(CommandTest, ParseSingleIntOption) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-n,--number", &value);

  std::vector<std::string> args = {"--number", "42"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(value, 42);
}

TEST_F(CommandTest, ParseSingleIntOptionShortName) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-n,--number", &value);

  std::vector<std::string> args = {"-n", "100"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(value, 100);
}

TEST_F(CommandTest, ParseSingleStringOption) {
  auto cmd = doptions::Command::createCommand("test");
  std::string value;
  cmd->addOption("-s,--string", &value);

  std::vector<std::string> args = {"--string", "hello"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(value, "hello");
}

TEST_F(CommandTest, ParseSingleBoolOption) {
  auto cmd = doptions::Command::createCommand("test");
  bool value = false;
  cmd->addOption("-v,--verbose", &value);

  std::vector<std::string> args = {"--verbose"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_TRUE(value);
}

TEST_F(CommandTest, ParseSingleBoolOptionShortName) {
  auto cmd = doptions::Command::createCommand("test");
  bool value = false;
  cmd->addOption("-v,--verbose", &value);

  std::vector<std::string> args = {"-v"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_TRUE(value);
}

TEST_F(CommandTest, ParseMultipleOptions) {
  auto cmd = doptions::Command::createCommand("test");
  int port = 0;
  std::string host;
  bool verbose = false;

  cmd->addOption("-p,--port", &port);
  cmd->addOption("-h,--host", &host);
  cmd->addOption("-v,--verbose", &verbose);

  std::vector<std::string> args = {"--port", "8080", "--host", "localhost",
                                   "--verbose"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(port, 8080);
  EXPECT_EQ(host, "localhost");
  EXPECT_TRUE(verbose);
}

TEST_F(CommandTest, ParseMultipleOptionsMixedShortLong) {
  auto cmd = doptions::Command::createCommand("test");
  int port = 0;
  std::string host;
  bool verbose = false;

  cmd->addOption("-p,--port", &port);
  cmd->addOption("-h,--host", &host);
  cmd->addOption("-v,--verbose", &verbose);

  std::vector<std::string> args = {"-p", "8080", "--host", "localhost", "-v"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(port, 8080);
  EXPECT_EQ(host, "localhost");
  EXPECT_TRUE(verbose);
}

// ============================================================================
// parseCommand Tests - All Integer Types
// ============================================================================

TEST_F(CommandTest, ParseAllIntegerTypes) {
  auto cmd = doptions::Command::createCommand("test");
  int8_t i8 = 0;
  int16_t i16 = 0;
  int32_t i32 = 0;
  int64_t i64 = 0;
  uint8_t u8 = 0;
  uint16_t u16 = 0;
  uint32_t u32 = 0;
  uint64_t u64 = 0;

  cmd->addOption("--int8", &i8);
  cmd->addOption("--int16", &i16);
  cmd->addOption("--int32", &i32);
  cmd->addOption("--int64", &i64);
  cmd->addOption("--uint8", &u8);
  cmd->addOption("--uint16", &u16);
  cmd->addOption("--uint32", &u32);
  cmd->addOption("--uint64", &u64);

  std::vector<std::string> args = {"--int8",   "127",     "--int16",  "32000",
                                   "--int32",  "2000000", "--int64",  "9000",
                                   "--uint8",  "255",     "--uint16", "65000",
                                   "--uint32", "4000",    "--uint64", "18000"};

  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(i8, 127);
  EXPECT_EQ(i16, 32000);
  EXPECT_EQ(i32, 2000000);
  EXPECT_EQ(i64, 9000);
  EXPECT_EQ(u8, 255);
  EXPECT_EQ(u16, 65000);
  EXPECT_EQ(u32, 4000U);
  EXPECT_EQ(u64, 18000ULL);
}

TEST_F(CommandTest, ParseFloatingPointTypes) {
  auto cmd = doptions::Command::createCommand("test");
  float f = 0.0f;
  double d = 0.0;

  cmd->addOption("-f,--float", &f);
  cmd->addOption("-d,--double", &d);

  std::vector<std::string> args = {"--float", "3.14", "--double", "2.71828"};

  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_FLOAT_EQ(f, 3.14f);
  EXPECT_DOUBLE_EQ(d, 2.71828);
}

// ============================================================================
// parseCommand Tests - Error Cases
// ============================================================================

TEST_F(CommandTest, ParseUnknownOptionThrows) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-n,--number", &value);

  std::vector<std::string> args = {"--unknown", "42"};
  EXPECT_THROW(cmd->parseCommand(args), doptions::ParseException);
}

TEST_F(CommandTest, ParseInsufficientValuesThrows) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-n,--number", &value);

  std::vector<std::string> args = {"--number"};  // Missing value
  EXPECT_THROW(cmd->parseCommand(args), doptions::ParseException);
}

TEST_F(CommandTest, ParseInvalidIntValueThrows) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-n,--number", &value);

  std::vector<std::string> args = {"--number", "not-a-number"};
  EXPECT_THROW(cmd->parseCommand(args), std::invalid_argument);
}

TEST_F(CommandTest, ParseInvalidFloatValueThrows) {
  auto cmd = doptions::Command::createCommand("test");
  float value = 0.0f;
  cmd->addOption("-f,--float", &value);

  std::vector<std::string> args = {"--float", "invalid"};
  EXPECT_THROW(cmd->parseCommand(args), std::invalid_argument);
}

// ============================================================================
// parseCommand Tests - CRITICAL: Duplicate Options Bug Test
// ============================================================================

TEST_F(CommandTest, ParseDuplicateOptionThrowsLongName) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-n,--number", &value);

  // Same option appears twice with long name
  std::vector<std::string> args = {"--number", "10", "--number", "20"};
  EXPECT_THROW(cmd->parseCommand(args), doptions::ParseException);
}

TEST_F(CommandTest, ParseDuplicateOptionThrowsShortName) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-n,--number", &value);

  // Same option appears twice with short name
  std::vector<std::string> args = {"-n", "10", "-n", "20"};
  EXPECT_THROW(cmd->parseCommand(args), doptions::ParseException);
}

TEST_F(CommandTest, ParseDuplicateOptionThrowsMixedNames) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-n,--number", &value);

  // Same option appears with both short and long names
  std::vector<std::string> args = {"-n", "10", "--number", "20"};
  EXPECT_THROW(cmd->parseCommand(args), doptions::ParseException);
}

TEST_F(CommandTest, ParseDuplicateOptionThrowsReversedMixedNames) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 0;
  cmd->addOption("-n,--number", &value);

  // Same option with long name first, then short
  std::vector<std::string> args = {"--number", "10", "-n", "20"};
  EXPECT_THROW(cmd->parseCommand(args), doptions::ParseException);
}

TEST_F(CommandTest, ParseMultipleDifferentOptionsNoThrow) {
  auto cmd = doptions::Command::createCommand("test");
  int value1 = 0;
  int value2 = 0;
  int value3 = 0;

  cmd->addOption("-a,--alpha", &value1);
  cmd->addOption("-b,--beta", &value2);
  cmd->addOption("-c,--gamma", &value3);

  // Different options should not throw
  std::vector<std::string> args = {"--alpha", "10",      "--beta",
                                   "20",      "--gamma", "30"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(value1, 10);
  EXPECT_EQ(value2, 20);
  EXPECT_EQ(value3, 30);
}

// ============================================================================
// parseCommand Tests - Boundary Values (int8_t, int16_t overflow detection)
// ============================================================================

TEST_F(CommandTest, ParseInt8BoundaryValues) {
  auto cmd = doptions::Command::createCommand("test");
  int8_t value = 0;
  cmd->addOption("-i,--int8", &value);

  // Valid boundary values
  std::vector<std::string> args_max = {"--int8", "127"};
  EXPECT_NO_THROW(cmd->parseCommand(args_max));
  EXPECT_EQ(value, 127);

  std::vector<std::string> args_min = {"--int8", "-128"};
  EXPECT_NO_THROW(cmd->parseCommand(args_min));
  EXPECT_EQ(value, -128);
}

TEST_F(CommandTest, ParseInt8OverflowThrows) {
  auto cmd = doptions::Command::createCommand("test");
  int8_t value = 0;
  cmd->addOption("-i,--int8", &value);

  // Value exceeds INT8_MAX
  std::vector<std::string> args_overflow = {"--int8", "128"};
  EXPECT_THROW(cmd->parseCommand(args_overflow), doptions::ParseException);

  // Value below INT8_MIN
  std::vector<std::string> args_underflow = {"--int8", "-129"};
  EXPECT_THROW(cmd->parseCommand(args_underflow), doptions::ParseException);
}

TEST_F(CommandTest, ParseInt16BoundaryValues) {
  auto cmd = doptions::Command::createCommand("test");
  int16_t value = 0;
  cmd->addOption("-i,--int16", &value);

  // Valid boundary values
  std::vector<std::string> args_max = {"--int16", "32767"};
  EXPECT_NO_THROW(cmd->parseCommand(args_max));
  EXPECT_EQ(value, 32767);

  std::vector<std::string> args_min = {"--int16", "-32768"};
  EXPECT_NO_THROW(cmd->parseCommand(args_min));
  EXPECT_EQ(value, -32768);
}

TEST_F(CommandTest, ParseInt16OverflowThrows) {
  auto cmd = doptions::Command::createCommand("test");
  int16_t value = 0;
  cmd->addOption("-i,--int16", &value);

  // Value exceeds INT16_MAX
  std::vector<std::string> args_overflow = {"--int16", "32768"};
  EXPECT_THROW(cmd->parseCommand(args_overflow), doptions::ParseException);

  // Value below INT16_MIN
  std::vector<std::string> args_underflow = {"--int16", "-32769"};
  EXPECT_THROW(cmd->parseCommand(args_underflow), doptions::ParseException);
}

TEST_F(CommandTest, ParseUint8BoundaryValues) {
  auto cmd = doptions::Command::createCommand("test");
  uint8_t value = 0;
  cmd->addOption("-u,--uint8", &value);

  // Valid boundary values
  std::vector<std::string> args_max = {"--uint8", "255"};
  EXPECT_NO_THROW(cmd->parseCommand(args_max));
  EXPECT_EQ(value, 255);

  std::vector<std::string> args_min = {"--uint8", "0"};
  EXPECT_NO_THROW(cmd->parseCommand(args_min));
  EXPECT_EQ(value, 0);
}

TEST_F(CommandTest, ParseUint8OverflowThrows) {
  auto cmd = doptions::Command::createCommand("test");
  uint8_t value = 0;
  cmd->addOption("-u,--uint8", &value);

  // Value exceeds UINT8_MAX
  std::vector<std::string> args_overflow = {"--uint8", "256"};
  EXPECT_THROW(cmd->parseCommand(args_overflow), doptions::ParseException);

  // Negative value for unsigned type
  std::vector<std::string> args_negative = {"--uint8", "-1"};
  EXPECT_THROW(cmd->parseCommand(args_negative), doptions::ParseException);
}

TEST_F(CommandTest, ParseUint16BoundaryValues) {
  auto cmd = doptions::Command::createCommand("test");
  uint16_t value = 0;
  cmd->addOption("-u,--uint16", &value);

  // Valid boundary values
  std::vector<std::string> args_max = {"--uint16", "65535"};
  EXPECT_NO_THROW(cmd->parseCommand(args_max));
  EXPECT_EQ(value, 65535);

  std::vector<std::string> args_min = {"--uint16", "0"};
  EXPECT_NO_THROW(cmd->parseCommand(args_min));
  EXPECT_EQ(value, 0);
}

TEST_F(CommandTest, ParseUint16OverflowThrows) {
  auto cmd = doptions::Command::createCommand("test");
  uint16_t value = 0;
  cmd->addOption("-u,--uint16", &value);

  // Value exceeds UINT16_MAX
  std::vector<std::string> args_overflow = {"--uint16", "65536"};
  EXPECT_THROW(cmd->parseCommand(args_overflow), doptions::ParseException);

  // Negative value for unsigned type
  std::vector<std::string> args_negative = {"--uint16", "-1"};
  EXPECT_THROW(cmd->parseCommand(args_negative), doptions::ParseException);
}

// ============================================================================
// parseCommand Tests - Complex Real-World Scenarios
// ============================================================================

TEST_F(CommandTest, WebServerConfiguration) {
  auto cmd = doptions::Command::createCommand("webserver");
  std::string host = "localhost";
  int port = 8080;
  int maxConnections = 100;
  double timeout = 30.0;
  bool sslEnabled = false;
  bool verboseLogging = false;
  std::string logFile;

  cmd->addOption("-h,--host", &host);
  cmd->addOption("-p,--port", &port);
  cmd->addOption("--max-connections", &maxConnections);
  cmd->addOption("-t,--timeout", &timeout);
  cmd->addOption("--enable-ssl", &sslEnabled);
  cmd->addOption("-v,--verbose", &verboseLogging);
  cmd->addOption("-l,--log-file", &logFile);

  std::vector<std::string> args = {"--host",
                                   "0.0.0.0",
                                   "-p",
                                   "9000",
                                   "--max-connections",
                                   "500",
                                   "--timeout",
                                   "60.5",
                                   "--enable-ssl",
                                   "-v",
                                   "--log-file",
                                   "/var/log/server.log"};

  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(host, "0.0.0.0");
  EXPECT_EQ(port, 9000);
  EXPECT_EQ(maxConnections, 500);
  EXPECT_DOUBLE_EQ(timeout, 60.5);
  EXPECT_TRUE(sslEnabled);
  EXPECT_TRUE(verboseLogging);
  EXPECT_EQ(logFile, "/var/log/server.log");
}

TEST_F(CommandTest, DatabaseMigrationTool) {
  auto cmd = doptions::Command::createCommand("migrate");
  std::string dbUrl;
  int batchSize = 1000;
  bool dryRun = false;
  bool rollback = false;
  int version = 0;

  cmd->addOption("--database-url", &dbUrl);
  cmd->addOption("--batch-size", &batchSize);
  cmd->addOption("--dry-run", &dryRun);
  cmd->addOption("--rollback", &rollback);
  cmd->addOption("--version", &version);

  std::vector<std::string> args = {"--database-url",
                                   "postgres://localhost:5432/mydb",
                                   "--batch-size",
                                   "5000",
                                   "--dry-run",
                                   "--version",
                                   "42"};

  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(dbUrl, "postgres://localhost:5432/mydb");
  EXPECT_EQ(batchSize, 5000);
  EXPECT_TRUE(dryRun);
  EXPECT_FALSE(rollback);
  EXPECT_EQ(version, 42);
}

TEST_F(CommandTest, ImageProcessingTool) {
  auto cmd = doptions::Command::createCommand("process");
  std::string inputFile;
  std::string outputFile;
  int width = 0;
  int height = 0;
  float quality = 0.9f;
  bool preserveAspectRatio = true;
  std::string format;

  cmd->addOption("-i,--input", &inputFile);
  cmd->addOption("-o,--output", &outputFile);
  cmd->addOption("-w,--width", &width);
  cmd->addOption("-h,--height", &height);
  cmd->addOption("-q,--quality", &quality);
  cmd->addOption("--preserve-aspect", &preserveAspectRatio);
  cmd->addOption("-f,--format", &format);

  std::vector<std::string> args = {
      "-i",       "input.jpg", "-o", "output.png", "--width",           "1920",
      "--height", "1080",      "-q", "0.95",       "--preserve-aspect", "-f",
      "png"};

  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(inputFile, "input.jpg");
  EXPECT_EQ(outputFile, "output.png");
  EXPECT_EQ(width, 1920);
  EXPECT_EQ(height, 1080);
  EXPECT_FLOAT_EQ(quality, 0.95f);
  EXPECT_TRUE(preserveAspectRatio);
  EXPECT_EQ(format, "png");
}

// ============================================================================
// parseCommand Tests - Edge Cases
// ============================================================================

TEST_F(CommandTest, ParseEmptyArgs) {
  auto cmd = doptions::Command::createCommand("test");
  int value = 42;  // Initial value
  cmd->addOption("-n,--number", &value);

  std::vector<std::string> args = {};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(value, 42);  // Value unchanged
}

TEST_F(CommandTest, ParseOnlyBoolOptions) {
  auto cmd = doptions::Command::createCommand("test");
  bool flag1 = false;
  bool flag2 = false;
  bool flag3 = false;

  cmd->addOption("-a,--alpha", &flag1);
  cmd->addOption("-b,--beta", &flag2);
  cmd->addOption("-c,--gamma", &flag3);

  std::vector<std::string> args = {"-a", "-c"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_TRUE(flag1);
  EXPECT_FALSE(flag2);
  EXPECT_TRUE(flag3);
}

TEST_F(CommandTest, ParseStringWithSpaces) {
  auto cmd = doptions::Command::createCommand("test");
  std::string message;
  cmd->addOption("-m,--message", &message);

  std::vector<std::string> args = {"--message", "hello world from test"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(message, "hello world from test");
}

TEST_F(CommandTest, ParseStringWithSpecialCharacters) {
  auto cmd = doptions::Command::createCommand("test");
  std::string path;
  cmd->addOption("-p,--path", &path);

  std::vector<std::string> args = {"--path", "/path/to/file.txt"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(path, "/path/to/file.txt");
}

TEST_F(CommandTest, ParseNegativeNumbers) {
  auto cmd = doptions::Command::createCommand("test");
  int intVal = 0;
  double doubleVal = 0.0;

  cmd->addOption("-i,--integer", &intVal);
  cmd->addOption("-d,--double", &doubleVal);

  std::vector<std::string> args = {"--integer", "-42", "--double", "-3.14"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(intVal, -42);
  EXPECT_DOUBLE_EQ(doubleVal, -3.14);
}

TEST_F(CommandTest, ParseZeroValues) {
  auto cmd = doptions::Command::createCommand("test");
  int intVal = 99;
  double doubleVal = 99.9;

  cmd->addOption("-i,--integer", &intVal);
  cmd->addOption("-d,--double", &doubleVal);

  std::vector<std::string> args = {"--integer", "0", "--double", "0.0"};
  EXPECT_NO_THROW(cmd->parseCommand(args));
  EXPECT_EQ(intVal, 0);
  EXPECT_DOUBLE_EQ(doubleVal, 0.0);
}

// ============================================================================
// parseCommand Tests - Order Independence
// ============================================================================

TEST_F(CommandTest, ParseOptionsInDifferentOrder) {
  auto cmd = doptions::Command::createCommand("test");
  int port = 0;
  std::string host;
  bool verbose = false;

  cmd->addOption("-p,--port", &port);
  cmd->addOption("-h,--host", &host);
  cmd->addOption("-v,--verbose", &verbose);

  // Test different orderings
  std::vector<std::string> args1 = {"-v", "--host", "localhost", "-p", "8080"};
  EXPECT_NO_THROW(cmd->parseCommand(args1));
  EXPECT_EQ(port, 8080);
  EXPECT_EQ(host, "localhost");
  EXPECT_TRUE(verbose);

  // Reset values
  port = 0;
  host = "";
  verbose = false;

  std::vector<std::string> args2 = {"--host", "example.com", "-v", "--port",
                                    "9000"};
  EXPECT_NO_THROW(cmd->parseCommand(args2));
  EXPECT_EQ(port, 9000);
  EXPECT_EQ(host, "example.com");
  EXPECT_TRUE(verbose);
}
