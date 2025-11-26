#include <gtest/gtest.h>
#include <doptions/application.hpp>
#include <doptions/exceptions.hpp>
#include <stdexcept>
#include <string>
#include <vector>

// Test fixture for Application tests
class ApplicationTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

// ============================================================================
// Basic Application Creation Tests
// ============================================================================

TEST_F(ApplicationTest, CreateApplication) {
  EXPECT_NO_THROW({ auto app = doptions::Application::createApp(); });
}

// ============================================================================
// addOption Tests
// ============================================================================

TEST_F(ApplicationTest, AddSingleOption) {
  auto app = doptions::Application::createApp();
  int value = 0;

  EXPECT_NO_THROW({ app.addOption("-v,--value", &value); });
}

TEST_F(ApplicationTest, AddMultipleOptions) {
  auto app = doptions::Application::createApp();
  int port = 0;
  std::string host;
  bool verbose = false;

  EXPECT_NO_THROW({
    app.addOption("-p,--port", &port);
    app.addOption("-h,--host", &host);
    app.addOption("-v,--verbose", &verbose);
  });
}

TEST_F(ApplicationTest, AddOptionsDifferentTypes) {
  auto app = doptions::Application::createApp();
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
    app.addOption("--int8", &i8);
    app.addOption("--int16", &i16);
    app.addOption("--int32", &i32);
    app.addOption("--int64", &i64);
    app.addOption("--uint8", &u8);
    app.addOption("--uint16", &u16);
    app.addOption("--uint32", &u32);
    app.addOption("--uint64", &u64);
    app.addOption("--float", &f);
    app.addOption("--double", &d);
    app.addOption("--bool", &b);
    app.addOption("--string", &s);
  });
}

// ============================================================================
// addCommand Tests
// ============================================================================

TEST_F(ApplicationTest, AddSingleCommand) {
  auto app = doptions::Application::createApp();
  bool executed = false;

  EXPECT_NO_THROW({ app.addCommand("test", &executed); });
}

TEST_F(ApplicationTest, AddMultipleCommands) {
  auto app = doptions::Application::createApp();
  bool exec1 = false;
  bool exec2 = false;
  bool exec3 = false;

  EXPECT_NO_THROW({
    app.addCommand("build", &exec1);
    app.addCommand("test", &exec2);
    app.addCommand("deploy", &exec3);
  });
}

TEST_F(ApplicationTest, AddCommandWithOptions) {
  auto app = doptions::Application::createApp();
  bool executed = false;
  int value = 0;

  auto& cmd = app.addCommand("process", &executed);
  EXPECT_NO_THROW({ cmd->addOption("-v,--value", &value); });
}

// ============================================================================
// parse Tests - Application Options Only
// ============================================================================

TEST_F(ApplicationTest, ParseSingleIntOption) {
  auto app = doptions::Application::createApp();
  int value = 0;
  app.addOption("-n,--number", &value);

  const char* argv[] = {"app", "--number", "42"};
  EXPECT_NO_THROW(app.parse(3, const_cast<char**>(argv)));
  EXPECT_EQ(value, 42);
}

TEST_F(ApplicationTest, ParseSingleIntOptionShortName) {
  auto app = doptions::Application::createApp();
  int value = 0;
  app.addOption("-n,--number", &value);

  const char* argv[] = {"app", "-n", "100"};
  EXPECT_NO_THROW(app.parse(3, const_cast<char**>(argv)));
  EXPECT_EQ(value, 100);
}

TEST_F(ApplicationTest, ParseSingleStringOption) {
  auto app = doptions::Application::createApp();
  std::string value;
  app.addOption("-s,--string", &value);

  const char* argv[] = {"app", "--string", "hello"};
  EXPECT_NO_THROW(app.parse(3, const_cast<char**>(argv)));
  EXPECT_EQ(value, "hello");
}

TEST_F(ApplicationTest, ParseSingleBoolOption) {
  auto app = doptions::Application::createApp();
  bool value = false;
  app.addOption("-v,--verbose", &value);

  const char* argv[] = {"app", "--verbose"};
  EXPECT_NO_THROW(app.parse(2, const_cast<char**>(argv)));
  EXPECT_TRUE(value);
}

TEST_F(ApplicationTest, ParseMultipleOptions) {
  auto app = doptions::Application::createApp();
  int port = 0;
  std::string host;
  bool verbose = false;

  app.addOption("-p,--port", &port);
  app.addOption("-h,--host", &host);
  app.addOption("-v,--verbose", &verbose);

  const char* argv[] = {"app", "--port", "8080", "--host",
                        "localhost", "--verbose"};
  EXPECT_NO_THROW(app.parse(6, const_cast<char**>(argv)));
  EXPECT_EQ(port, 8080);
  EXPECT_EQ(host, "localhost");
  EXPECT_TRUE(verbose);
}

TEST_F(ApplicationTest, ParseMultipleOptionsMixedShortLong) {
  auto app = doptions::Application::createApp();
  int port = 0;
  std::string host;
  bool verbose = false;

  app.addOption("-p,--port", &port);
  app.addOption("-h,--host", &host);
  app.addOption("-v,--verbose", &verbose);

  const char* argv[] = {"app", "-p", "9000", "--host", "example.com", "-v"};
  EXPECT_NO_THROW(app.parse(6, const_cast<char**>(argv)));
  EXPECT_EQ(port, 9000);
  EXPECT_EQ(host, "example.com");
  EXPECT_TRUE(verbose);
}

// ============================================================================
// parse Tests - Commands
// ============================================================================

TEST_F(ApplicationTest, ParseSimpleCommand) {
  auto app = doptions::Application::createApp();
  bool executed = false;

  app.addCommand("build", &executed);

  const char* argv[] = {"app", "build"};
  EXPECT_NO_THROW(app.parse(2, const_cast<char**>(argv)));
  EXPECT_TRUE(executed);
}

TEST_F(ApplicationTest, ParseCommandWithOptions) {
  auto app = doptions::Application::createApp();
  bool executed = false;
  int value = 0;

  auto& cmd = app.addCommand("process", &executed);
  cmd->addOption("-v,--value", &value);

  const char* argv[] = {"app", "process", "--value", "42"};
  EXPECT_NO_THROW(app.parse(4, const_cast<char**>(argv)));
  EXPECT_TRUE(executed);
  EXPECT_EQ(value, 42);
}

TEST_F(ApplicationTest, ParseCommandWithMultipleOptions) {
  auto app = doptions::Application::createApp();
  bool executed = false;
  int port = 0;
  std::string host;
  bool verbose = false;

  auto& cmd = app.addCommand("serve", &executed);
  cmd->addOption("-p,--port", &port);
  cmd->addOption("-h,--host", &host);
  cmd->addOption("-v,--verbose", &verbose);

  const char* argv[] = {"app", "serve", "--port", "8080",
                        "--host", "0.0.0.0", "-v"};
  EXPECT_NO_THROW(app.parse(7, const_cast<char**>(argv)));
  EXPECT_TRUE(executed);
  EXPECT_EQ(port, 8080);
  EXPECT_EQ(host, "0.0.0.0");
  EXPECT_TRUE(verbose);
}

TEST_F(ApplicationTest, ParseGlobalOptionsBeforeCommand) {
  auto app = doptions::Application::createApp();
  bool verbose = false;
  bool executed = false;
  int value = 0;

  app.addOption("-v,--verbose", &verbose);
  auto& cmd = app.addCommand("process", &executed);
  cmd->addOption("--value", &value);

  const char* argv[] = {"app", "--verbose", "process", "--value", "42"};
  EXPECT_NO_THROW(app.parse(5, const_cast<char**>(argv)));
  EXPECT_TRUE(verbose);
  EXPECT_TRUE(executed);
  EXPECT_EQ(value, 42);
}

// ============================================================================
// parse Tests - Error Cases
// ============================================================================

TEST_F(ApplicationTest, ParseUnknownOptionThrows) {
  auto app = doptions::Application::createApp();
  int value = 0;
  app.addOption("-n,--number", &value);

  const char* argv[] = {"app", "--unknown", "42"};
  EXPECT_THROW(app.parse(3, const_cast<char**>(argv)),
               doptions::ParseException);
}

TEST_F(ApplicationTest, ParseInsufficientValuesThrows) {
  auto app = doptions::Application::createApp();
  int value = 0;
  app.addOption("-n,--number", &value);

  const char* argv[] = {"app", "--number"};
  EXPECT_THROW(app.parse(2, const_cast<char**>(argv)),
               doptions::ParseException);
}

TEST_F(ApplicationTest, ParseInvalidIntValueThrows) {
  auto app = doptions::Application::createApp();
  int value = 0;
  app.addOption("-n,--number", &value);

  const char* argv[] = {"app", "--number", "not-a-number"};
  EXPECT_THROW(app.parse(3, const_cast<char**>(argv)), std::invalid_argument);
}

TEST_F(ApplicationTest, ParseDuplicateOptionThrowsLongName) {
  auto app = doptions::Application::createApp();
  int value = 0;
  app.addOption("-n,--number", &value);

  const char* argv[] = {"app", "--number", "10", "--number", "20"};
  EXPECT_THROW(app.parse(5, const_cast<char**>(argv)),
               doptions::ParseException);
}

TEST_F(ApplicationTest, ParseDuplicateOptionThrowsShortName) {
  auto app = doptions::Application::createApp();
  int value = 0;
  app.addOption("-n,--number", &value);

  const char* argv[] = {"app", "-n", "10", "-n", "20"};
  EXPECT_THROW(app.parse(5, const_cast<char**>(argv)),
               doptions::ParseException);
}

TEST_F(ApplicationTest, ParseDuplicateOptionThrowsMixed) {
  auto app = doptions::Application::createApp();
  int value = 0;
  app.addOption("-n,--number", &value);

  const char* argv[] = {"app", "-n", "10", "--number", "20"};
  EXPECT_THROW(app.parse(5, const_cast<char**>(argv)),
               doptions::ParseException);
}

TEST_F(ApplicationTest, ParseUnknownCommandThrows) {
  auto app = doptions::Application::createApp();
  bool executed = false;
  app.addCommand("build", &executed);

  const char* argv[] = {"app", "unknown"};
  EXPECT_THROW(app.parse(2, const_cast<char**>(argv)),
               doptions::ParseException);
}

// ============================================================================
// parse Tests - Edge Cases
// ============================================================================

TEST_F(ApplicationTest, ParseEmptyArgs) {
  auto app = doptions::Application::createApp();
  int value = 42;
  app.addOption("-n,--number", &value);

  const char* argv[] = {"app"};
  EXPECT_NO_THROW(app.parse(1, const_cast<char**>(argv)));
  EXPECT_EQ(value, 42);  // Value unchanged
}

TEST_F(ApplicationTest, ParseOnlyBoolOptions) {
  auto app = doptions::Application::createApp();
  bool flag1 = false;
  bool flag2 = false;
  bool flag3 = false;

  app.addOption("-a,--alpha", &flag1);
  app.addOption("-b,--beta", &flag2);
  app.addOption("-c,--gamma", &flag3);

  const char* argv[] = {"app", "-a", "-c"};
  EXPECT_NO_THROW(app.parse(3, const_cast<char**>(argv)));
  EXPECT_TRUE(flag1);
  EXPECT_FALSE(flag2);
  EXPECT_TRUE(flag3);
}

TEST_F(ApplicationTest, ParseStringWithSpaces) {
  auto app = doptions::Application::createApp();
  std::string message;
  app.addOption("-m,--message", &message);

  const char* argv[] = {"app", "--message", "hello world from test"};
  EXPECT_NO_THROW(app.parse(3, const_cast<char**>(argv)));
  EXPECT_EQ(message, "hello world from test");
}

TEST_F(ApplicationTest, ParseNegativeNumbers) {
  auto app = doptions::Application::createApp();
  int intVal = 0;
  double doubleVal = 0.0;

  app.addOption("-i,--integer", &intVal);
  app.addOption("-d,--double", &doubleVal);

  const char* argv[] = {"app", "--integer", "-42", "--double", "-3.14"};
  EXPECT_NO_THROW(app.parse(5, const_cast<char**>(argv)));
  EXPECT_EQ(intVal, -42);
  EXPECT_DOUBLE_EQ(doubleVal, -3.14);
}

TEST_F(ApplicationTest, ParseZeroValues) {
  auto app = doptions::Application::createApp();
  int intVal = 99;
  double doubleVal = 99.9;

  app.addOption("-i,--integer", &intVal);
  app.addOption("-d,--double", &doubleVal);

  const char* argv[] = {"app", "--integer", "0", "--double", "0.0"};
  EXPECT_NO_THROW(app.parse(5, const_cast<char**>(argv)));
  EXPECT_EQ(intVal, 0);
  EXPECT_DOUBLE_EQ(doubleVal, 0.0);
}

TEST_F(ApplicationTest, MultipleCommandsOnlyOneExecutes) {
  auto app = doptions::Application::createApp();
  bool exec1 = false;
  bool exec2 = false;
  bool exec3 = false;

  app.addCommand("build", &exec1);
  app.addCommand("test", &exec2);
  app.addCommand("deploy", &exec3);

  const char* argv[] = {"app", "test"};
  EXPECT_NO_THROW(app.parse(2, const_cast<char**>(argv)));
  EXPECT_FALSE(exec1);
  EXPECT_TRUE(exec2);
  EXPECT_FALSE(exec3);
}

TEST_F(ApplicationTest, CommandStopsOptionParsing) {
  auto app = doptions::Application::createApp();
  bool globalVerbose = false;
  bool executed = false;
  bool cmdVerbose = false;

  app.addOption("-v,--verbose", &globalVerbose);
  auto& cmd = app.addCommand("process", &executed);
  cmd->addOption("-v,--verbose", &cmdVerbose);

  // -v before command is global, -v after command is for the command
  const char* argv[] = {"app", "-v", "process", "-v"};
  EXPECT_NO_THROW(app.parse(4, const_cast<char**>(argv)));
  EXPECT_TRUE(globalVerbose);
  EXPECT_TRUE(executed);
  EXPECT_TRUE(cmdVerbose);
}

// ============================================================================
// Real-World Scenarios
// ============================================================================

TEST_F(ApplicationTest, GitLikeInterface) {
  auto app = doptions::Application::createApp();
  bool globalVerbose = false;
  bool commitExecuted = false;
  std::string message;
  bool amend = false;

  app.addOption("-v,--verbose", &globalVerbose);

  auto& commit = app.addCommand("commit", &commitExecuted);
  commit->addOption("-m,--message", &message);
  commit->addOption("--amend", &amend);

  const char* argv[] = {"git", "--verbose", "commit", "-m",
                        "Initial commit", "--amend"};
  EXPECT_NO_THROW(app.parse(6, const_cast<char**>(argv)));
  EXPECT_TRUE(globalVerbose);
  EXPECT_TRUE(commitExecuted);
  EXPECT_EQ(message, "Initial commit");
  EXPECT_TRUE(amend);
}

TEST_F(ApplicationTest, DockerLikeInterface) {
  auto app = doptions::Application::createApp();
  bool runExecuted = false;
  int port = 0;
  std::string image;
  bool detached = false;
  std::string name;

  auto& run = app.addCommand("start", &runExecuted);
  run->addOption("-p,--port", &port);
  run->addOption("-d,--detach", &detached);
  run->addOption("--name", &name);
  run->addOption("--image", &image);

  const char* argv[] = {"docker", "start", "-d", "--name", "myapp",
                        "-p", "8080", "--image", "nginx:latest"};
  EXPECT_NO_THROW(app.parse(9, const_cast<char**>(argv)));
  EXPECT_TRUE(runExecuted);
  EXPECT_TRUE(detached);
  EXPECT_EQ(name, "myapp");
  EXPECT_EQ(port, 8080);
  EXPECT_EQ(image, "nginx:latest");
}

TEST_F(ApplicationTest, BuildSystemInterface) {
  auto app = doptions::Application::createApp();
  bool buildExecuted = false;
  bool testExecuted = false;
  bool cleanExecuted = false;

  std::string buildType;
  int jobs = 1;
  bool verbose = false;
  std::string filter;

  auto& build = app.addCommand("build", &buildExecuted);
  build->addOption("--type", &buildType);
  build->addOption("-j,--jobs", &jobs);
  build->addOption("-v,--verbose", &verbose);

  auto& test = app.addCommand("test", &testExecuted);
  test->addOption("--filter", &filter);

  app.addCommand("clean", &cleanExecuted);

  const char* argv[] = {"build-tool", "build", "--type", "Release",
                        "-j", "4", "-v"};
  EXPECT_NO_THROW(app.parse(7, const_cast<char**>(argv)));
  EXPECT_TRUE(buildExecuted);
  EXPECT_FALSE(testExecuted);
  EXPECT_FALSE(cleanExecuted);
  EXPECT_EQ(buildType, "Release");
  EXPECT_EQ(jobs, 4);
  EXPECT_TRUE(verbose);
}

TEST_F(ApplicationTest, DatabaseMigrationTool) {
  auto app = doptions::Application::createApp();
  bool migrateExecuted = false;
  bool rollbackExecuted = false;

  std::string dbUrl;
  int batchSize = 1000;
  bool dryRun = false;
  int targetVersion = 0;

  auto& migrate = app.addCommand("migrate", &migrateExecuted);
  migrate->addOption("--database-url", &dbUrl);
  migrate->addOption("--batch-size", &batchSize);
  migrate->addOption("--dry-run", &dryRun);

  auto& rollback = app.addCommand("rollback", &rollbackExecuted);
  rollback->addOption("--database-url", &dbUrl);
  rollback->addOption("--target-version", &targetVersion);

  const char* argv[] = {"db-tool", "migrate", "--database-url",
                        "postgres://localhost:5432/mydb",
                        "--batch-size", "5000", "--dry-run"};
  EXPECT_NO_THROW(app.parse(7, const_cast<char**>(argv)));
  EXPECT_TRUE(migrateExecuted);
  EXPECT_FALSE(rollbackExecuted);
  EXPECT_EQ(dbUrl, "postgres://localhost:5432/mydb");
  EXPECT_EQ(batchSize, 5000);
  EXPECT_TRUE(dryRun);
}

TEST_F(ApplicationTest, CLIWithBothGlobalAndCommandOptions) {
  auto app = doptions::Application::createApp();

  // Global options
  std::string configFile;
  bool debug = false;

  // Command options
  bool serveExecuted = false;
  int port = 0;
  std::string host;

  app.addOption("-c,--config", &configFile);
  app.addOption("--debug", &debug);

  auto& serve = app.addCommand("serve", &serveExecuted);
  serve->addOption("-p,--port", &port);
  serve->addOption("-h,--host", &host);

  const char* argv[] = {"app", "--config", "/etc/app.conf", "--debug",
                        "serve", "-p", "8080", "--host", "0.0.0.0"};
  EXPECT_NO_THROW(app.parse(9, const_cast<char**>(argv)));
  EXPECT_EQ(configFile, "/etc/app.conf");
  EXPECT_TRUE(debug);
  EXPECT_TRUE(serveExecuted);
  EXPECT_EQ(port, 8080);
  EXPECT_EQ(host, "0.0.0.0");
}

// ============================================================================
// Boundary Values Tests
// ============================================================================

TEST_F(ApplicationTest, ParseInt8BoundaryValues) {
  auto app = doptions::Application::createApp();
  int8_t value = 0;
  app.addOption("-i,--int8", &value);

  const char* argv_max[] = {"app", "--int8", "127"};
  EXPECT_NO_THROW(app.parse(3, const_cast<char**>(argv_max)));
  EXPECT_EQ(value, 127);

  const char* argv_min[] = {"app", "--int8", "-128"};
  EXPECT_NO_THROW(app.parse(3, const_cast<char**>(argv_min)));
  EXPECT_EQ(value, -128);
}

TEST_F(ApplicationTest, ParseInt8OverflowThrows) {
  auto app = doptions::Application::createApp();
  int8_t value = 0;
  app.addOption("-i,--int8", &value);

  const char* argv_overflow[] = {"app", "--int8", "128"};
  EXPECT_THROW(app.parse(3, const_cast<char**>(argv_overflow)),
               doptions::ParseException);

  const char* argv_underflow[] = {"app", "--int8", "-129"};
  EXPECT_THROW(app.parse(3, const_cast<char**>(argv_underflow)),
               doptions::ParseException);
}

TEST_F(ApplicationTest, ParseAllIntegerTypes) {
  auto app = doptions::Application::createApp();
  int8_t i8 = 0;
  int16_t i16 = 0;
  int32_t i32 = 0;
  int64_t i64 = 0;
  uint8_t u8 = 0;
  uint16_t u16 = 0;
  uint32_t u32 = 0;
  uint64_t u64 = 0;

  app.addOption("--int8", &i8);
  app.addOption("--int16", &i16);
  app.addOption("--int32", &i32);
  app.addOption("--int64", &i64);
  app.addOption("--uint8", &u8);
  app.addOption("--uint16", &u16);
  app.addOption("--uint32", &u32);
  app.addOption("--uint64", &u64);

  const char* argv[] = {"app", "--int8", "127", "--int16", "32000",
                        "--int32", "2000000", "--int64", "9000",
                        "--uint8", "255", "--uint16", "65000",
                        "--uint32", "4000", "--uint64", "18000"};

  EXPECT_NO_THROW(app.parse(17, const_cast<char**>(argv)));
  EXPECT_EQ(i8, 127);
  EXPECT_EQ(i16, 32000);
  EXPECT_EQ(i32, 2000000);
  EXPECT_EQ(i64, 9000);
  EXPECT_EQ(u8, 255);
  EXPECT_EQ(u16, 65000);
  EXPECT_EQ(u32, 4000U);
  EXPECT_EQ(u64, 18000ULL);
}

TEST_F(ApplicationTest, ParseFloatingPointTypes) {
  auto app = doptions::Application::createApp();
  float f = 0.0f;
  double d = 0.0;

  app.addOption("-f,--float", &f);
  app.addOption("-d,--double", &d);

  const char* argv[] = {"app", "--float", "3.14", "--double", "2.71828"};

  EXPECT_NO_THROW(app.parse(5, const_cast<char**>(argv)));
  EXPECT_FLOAT_EQ(f, 3.14f);
  EXPECT_DOUBLE_EQ(d, 2.71828);
}
