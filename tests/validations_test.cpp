#include <gtest/gtest.h>
#include <doptions/validations.hpp>
#include <stdexcept>

// Test fixture for NameValidations tests
class NameValidationsTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Reset to default options before each test
    doptions::NameValidationConfig defaultOpts;
    doptions::NameValidations::setConfig(defaultOpts);
  }

  void TearDown() override {}
};

// ============================================================================
// Basic Name Validation Tests
// ============================================================================

TEST_F(NameValidationsTest, ValidSimpleNames) {
  EXPECT_NO_THROW(doptions::NameValidations::validateName("name"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("x"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("abc"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("longname"));
}

TEST_F(NameValidationsTest, ValidNamesWithNumbers) {
  EXPECT_NO_THROW(doptions::NameValidations::validateName("name123"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("test1"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("x1y2z3"));
}

TEST_F(NameValidationsTest, ValidNamesWithDashes) {
  EXPECT_NO_THROW(doptions::NameValidations::validateName("my-name"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("multi-word-name"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("a-b-c"));
}

TEST_F(NameValidationsTest, ValidNamesWithUnderscores) {
  EXPECT_NO_THROW(doptions::NameValidations::validateName("my_name"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("multi_word_name"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("a_b_c"));
}

TEST_F(NameValidationsTest, EmptyNameThrows) {
  EXPECT_THROW(doptions::NameValidations::validateName(""),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, NameStartingWithNumberThrows) {
  EXPECT_THROW(doptions::NameValidations::validateName("1name"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("2test"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("9abc"),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, NameStartingWithDashThrows) {
  EXPECT_THROW(doptions::NameValidations::validateName("-name"),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, NameStartingWithUnderscoreThrows) {
  EXPECT_THROW(doptions::NameValidations::validateName("_name"),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, NameWithInvalidCharactersThrows) {
  EXPECT_THROW(doptions::NameValidations::validateName("name@test"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("name!"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("name#"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("name$"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("name%"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("name&"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("name*"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("name+"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("name="),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, NameWithSpacesThrows) {
  EXPECT_THROW(doptions::NameValidations::validateName("my name"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("test name"),
               std::invalid_argument);
}

// ============================================================================
// Dots Configuration Tests
// ============================================================================

TEST_F(NameValidationsTest, DotsNotAllowedByDefault) {
  EXPECT_THROW(doptions::NameValidations::validateName("my.name"),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, DotsAllowedWhenEnabled) {
  doptions::NameValidationConfig opts;
  opts.nameContainsDots = true;
  doptions::NameValidations::setConfig(opts);

  EXPECT_NO_THROW(doptions::NameValidations::validateName("my.name"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("test.option"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("a.b.c"));
}

TEST_F(NameValidationsTest, DotsStillRequireAlphaFirst) {
  doptions::NameValidationConfig opts;
  opts.nameContainsDots = true;
  doptions::NameValidations::setConfig(opts);

  EXPECT_THROW(doptions::NameValidations::validateName(".name"),
               std::invalid_argument);
}

// ============================================================================
// Dashes Configuration Tests
// ============================================================================

TEST_F(NameValidationsTest, DashesAllowedByDefault) {
  EXPECT_NO_THROW(doptions::NameValidations::validateName("my-name"));
}

TEST_F(NameValidationsTest, DashesNotAllowedWhenDisabled) {
  doptions::NameValidationConfig opts;
  opts.nameContainsDashes = false;
  doptions::NameValidations::setConfig(opts);

  EXPECT_THROW(doptions::NameValidations::validateName("my-name"),
               std::invalid_argument);
}

// ============================================================================
// Underscores Configuration Tests
// ============================================================================

TEST_F(NameValidationsTest, UnderscoresAllowedByDefault) {
  EXPECT_NO_THROW(doptions::NameValidations::validateName("my_name"));
}

TEST_F(NameValidationsTest, UnderscoresNotAllowedWhenDisabled) {
  doptions::NameValidationConfig opts;
  opts.nameContainsUnderscores = false;
  doptions::NameValidations::setConfig(opts);

  EXPECT_THROW(doptions::NameValidations::validateName("my_name"),
               std::invalid_argument);
}

// ============================================================================
// Combined Configuration Tests
// ============================================================================

TEST_F(NameValidationsTest, AllSpecialCharactersEnabled) {
  doptions::NameValidationConfig opts;
  opts.nameContainsDots = true;
  opts.nameContainsDashes = true;
  opts.nameContainsUnderscores = true;
  doptions::NameValidations::setConfig(opts);

  EXPECT_NO_THROW(doptions::NameValidations::validateName("my-name_test.opt"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("a_b-c.d"));
}

TEST_F(NameValidationsTest, AllSpecialCharactersDisabled) {
  doptions::NameValidationConfig opts;
  opts.nameContainsDots = false;
  opts.nameContainsDashes = false;
  opts.nameContainsUnderscores = false;
  doptions::NameValidations::setConfig(opts);

  // Only alphanumeric allowed
  EXPECT_NO_THROW(doptions::NameValidations::validateName("myname"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("test123"));

  // All special chars should fail
  EXPECT_THROW(doptions::NameValidations::validateName("my-name"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("my_name"),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateName("my.name"),
               std::invalid_argument);
}

// ============================================================================
// Size Validation Tests
// ============================================================================

TEST_F(NameValidationsTest, ShortNameValidSizes) {
  // Short names: 1-3 characters (default limit)
  EXPECT_NO_THROW(doptions::NameValidations::validateSize("a", true));
  EXPECT_NO_THROW(doptions::NameValidations::validateSize("ab", true));
  EXPECT_NO_THROW(doptions::NameValidations::validateSize("abc", true));
}

TEST_F(NameValidationsTest, ShortNameTooLong) {
  // 4 characters exceeds default short limit of 3
  EXPECT_THROW(doptions::NameValidations::validateSize("abcd", true),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateSize("abcde", true),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, ShortNameEmpty) {
  EXPECT_THROW(doptions::NameValidations::validateSize("", true),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, LongNameValidSizes) {
  // Long names: 4-100 characters (default limits)
  EXPECT_NO_THROW(doptions::NameValidations::validateSize("abcd", false));
  EXPECT_NO_THROW(doptions::NameValidations::validateSize("longname", false));
  EXPECT_NO_THROW(
      doptions::NameValidations::validateSize("verylongnamehere", false));
}

TEST_F(NameValidationsTest, LongNameTooShort) {
  // 3 or less characters is too short for long name
  EXPECT_THROW(doptions::NameValidations::validateSize("a", false),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateSize("ab", false),
               std::invalid_argument);
  EXPECT_THROW(doptions::NameValidations::validateSize("abc", false),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, LongNameEmpty) {
  EXPECT_THROW(doptions::NameValidations::validateSize("", false),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, LongNameExactlyAtLimit) {
  // Create a name exactly 100 characters (default long limit)
  std::string exactLimit(100, 'a');
  EXPECT_NO_THROW(doptions::NameValidations::validateSize(exactLimit, false));
}

TEST_F(NameValidationsTest, LongNameExceedsLimit) {
  // Create a name that exceeds 100 characters
  std::string tooLong(101, 'a');
  EXPECT_THROW(doptions::NameValidations::validateSize(tooLong, false),
               std::invalid_argument);
}

// ============================================================================
// Custom Size Limits Tests
// ============================================================================

TEST_F(NameValidationsTest, CustomShortNameLimit) {
  doptions::NameValidationConfig opts;
  opts.shortNameLimit = 5;
  doptions::NameValidations::setConfig(opts);

  EXPECT_NO_THROW(doptions::NameValidations::validateSize("abcde", true));
  EXPECT_THROW(doptions::NameValidations::validateSize("abcdef", true),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, CustomLongNameLimit) {
  doptions::NameValidationConfig opts;
  opts.longNameLimit = 10;
  doptions::NameValidations::setConfig(opts);

  std::string tenChars(10, 'a');
  std::string elevenChars(11, 'a');

  EXPECT_NO_THROW(doptions::NameValidations::validateSize(tenChars, false));
  EXPECT_THROW(doptions::NameValidations::validateSize(elevenChars, false),
               std::invalid_argument);
}

TEST_F(NameValidationsTest, CustomBothLimits) {
  doptions::NameValidationConfig opts;
  opts.shortNameLimit = 2;
  opts.longNameLimit = 20;
  doptions::NameValidations::setConfig(opts);

  // Short: max 2
  EXPECT_NO_THROW(doptions::NameValidations::validateSize("ab", true));
  EXPECT_THROW(doptions::NameValidations::validateSize("abc", true),
               std::invalid_argument);

  // Long: min 3 (shortLimit + 1), max 20
  EXPECT_NO_THROW(doptions::NameValidations::validateSize("abc", false));
  EXPECT_NO_THROW(
      doptions::NameValidations::validateSize(std::string(20, 'a'), false));
  EXPECT_THROW(
      doptions::NameValidations::validateSize(std::string(21, 'a'), false),
      std::invalid_argument);
}

// ============================================================================
// validChar Tests
// ============================================================================

TEST_F(NameValidationsTest, ValidCharAlphabetic) {
  EXPECT_TRUE(doptions::NameValidations::validChar('a'));
  EXPECT_TRUE(doptions::NameValidations::validChar('z'));
  EXPECT_TRUE(doptions::NameValidations::validChar('A'));
  EXPECT_TRUE(doptions::NameValidations::validChar('Z'));
}

TEST_F(NameValidationsTest, ValidCharNumeric) {
  // Numeric valid when not first
  EXPECT_TRUE(doptions::NameValidations::validChar('0', false));
  EXPECT_TRUE(doptions::NameValidations::validChar('9', false));

  // Numeric invalid when first
  EXPECT_FALSE(doptions::NameValidations::validChar('0', true));
  EXPECT_FALSE(doptions::NameValidations::validChar('9', true));
}

TEST_F(NameValidationsTest, ValidCharDashWhenEnabled) {
  EXPECT_TRUE(doptions::NameValidations::validChar('-', false));
  EXPECT_FALSE(doptions::NameValidations::validChar('-', true));

  doptions::NameValidationConfig opts;
  opts.nameContainsDashes = false;
  doptions::NameValidations::setConfig(opts);

  EXPECT_FALSE(doptions::NameValidations::validChar('-', false));
}

TEST_F(NameValidationsTest, ValidCharUnderscoreWhenEnabled) {
  EXPECT_TRUE(doptions::NameValidations::validChar('_', false));
  EXPECT_FALSE(doptions::NameValidations::validChar('_', true));

  doptions::NameValidationConfig opts;
  opts.nameContainsUnderscores = false;
  doptions::NameValidations::setConfig(opts);

  EXPECT_FALSE(doptions::NameValidations::validChar('_', false));
}

TEST_F(NameValidationsTest, ValidCharDotWhenEnabled) {
  EXPECT_FALSE(doptions::NameValidations::validChar('.', false));
  EXPECT_FALSE(doptions::NameValidations::validChar('.', true));

  doptions::NameValidationConfig opts;
  opts.nameContainsDots = true;
  doptions::NameValidations::setConfig(opts);

  EXPECT_TRUE(doptions::NameValidations::validChar('.', false));
  EXPECT_FALSE(doptions::NameValidations::validChar('.', true));
}

TEST_F(NameValidationsTest, ValidCharInvalidSpecialChars) {
  EXPECT_FALSE(doptions::NameValidations::validChar('@', false));
  EXPECT_FALSE(doptions::NameValidations::validChar('!', false));
  EXPECT_FALSE(doptions::NameValidations::validChar('#', false));
  EXPECT_FALSE(doptions::NameValidations::validChar('$', false));
  EXPECT_FALSE(doptions::NameValidations::validChar('%', false));
  EXPECT_FALSE(doptions::NameValidations::validChar('&', false));
  EXPECT_FALSE(doptions::NameValidations::validChar('*', false));
}

// ============================================================================
// Real-world Use Cases
// ============================================================================

TEST_F(NameValidationsTest, CLIOptionNamesDefault) {
  // Common CLI option names should work with defaults
  EXPECT_NO_THROW(doptions::NameValidations::validateName("help"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("version"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("verbose"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("output-file"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("max-depth"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("dry-run"));
}

TEST_F(NameValidationsTest, PythonStyleNaming) {
  // Python-style naming with underscores
  EXPECT_NO_THROW(doptions::NameValidations::validateName("output_file"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("max_connections"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("enable_ssl"));
}

TEST_F(NameValidationsTest, DotNotationForHierarchy) {
  doptions::NameValidationConfig opts;
  opts.nameContainsDots = true;
  doptions::NameValidations::setConfig(opts);

  // Hierarchical naming like "server.host", "db.port"
  EXPECT_NO_THROW(doptions::NameValidations::validateName("server.host"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("db.port"));
  EXPECT_NO_THROW(doptions::NameValidations::validateName("app.config.file"));
}
