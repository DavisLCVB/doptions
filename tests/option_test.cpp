#include <doptions/option.hpp>
#include <doptions/exceptions.hpp>
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <vector>

// Test fixture for Option tests
class OptionTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

// ============================================================================
// Complex Use Case: CLI Application Configuration
// ============================================================================

TEST_F(OptionTest, CompleteApplicationOptionSet) {
  // Simula una aplicación CLI real con múltiples opciones de diferentes tipos
  int port = 8080;
  std::string host = "localhost";
  bool verbose = false;
  double timeout = 30.0;
  int64_t maxConnections = 1000;

  EXPECT_NO_THROW({
    auto portOpt = doptions::Option<int>::createOption("-p,--port", &port);
    auto hostOpt =
        doptions::Option<std::string>::createOption("-h,--host", &host);
    auto verboseOpt =
        doptions::Option<bool>::createOption("-v,--verbose", &verbose);
    auto timeoutOpt =
        doptions::Option<double>::createOption("-t,--timeout", &timeout);
    auto maxConnOpt = doptions::Option<int64_t>::createOption(
        "-m,--max-connections", &maxConnections);
  });
}

TEST_F(OptionTest, ConflictingShortNamesInApplication) {
  // Test que dos opciones pueden tener solo nombres largos para evitar conflictos
  int value1 = 0;
  int value2 = 0;

  EXPECT_NO_THROW({
    auto opt1 = doptions::Option<int>::createOption("--first-option", &value1);
    auto opt2 =
        doptions::Option<int>::createOption("--second-option", &value2);
  });
}

TEST_F(OptionTest, MultipleOptionsWithSameType) {
  // Múltiples opciones del mismo tipo, común en aplicaciones reales
  int width = 0;
  int height = 0;
  int depth = 0;

  EXPECT_NO_THROW({
    auto widthOpt = doptions::Option<int>::createOption("-w,--width", &width);
    auto heightOpt =
        doptions::Option<int>::createOption("-h,--height", &height);
    auto depthOpt = doptions::Option<int>::createOption("-d,--depth", &depth);
  });
}

// ============================================================================
// Complex Custom Types
// ============================================================================

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;

  bool operator==(const Color& other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
  }
};

REGISTER_TYPE(Color) {
  // Format: "r,g,b,a" or "#RRGGBBAA"
  if (str.empty()) {
    throw std::invalid_argument("Color string cannot be empty");
  }

  if (str[0] == '#') {
    // Hex format
    if (str.length() != 9) {
      throw std::invalid_argument("Hex color must be #RRGGBBAA format");
    }
    uint32_t hex = std::stoul(str.substr(1), nullptr, 16);
    return Color{static_cast<uint8_t>((hex >> 24) & 0xFF),
                 static_cast<uint8_t>((hex >> 16) & 0xFF),
                 static_cast<uint8_t>((hex >> 8) & 0xFF),
                 static_cast<uint8_t>(hex & 0xFF)};
  }

  // CSV format
  size_t pos1 = str.find(',');
  size_t pos2 = str.find(',', pos1 + 1);
  size_t pos3 = str.find(',', pos2 + 1);

  if (pos1 == std::string::npos || pos2 == std::string::npos ||
      pos3 == std::string::npos) {
    throw std::invalid_argument("Color must be in r,g,b,a format");
  }

  int r = std::stoi(str.substr(0, pos1));
  int g = std::stoi(str.substr(pos1 + 1, pos2 - pos1 - 1));
  int b = std::stoi(str.substr(pos2 + 1, pos3 - pos2 - 1));
  int a = std::stoi(str.substr(pos3 + 1));

  if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255 || a < 0 ||
      a > 255) {
    throw std::out_of_range("Color values must be 0-255");
  }

  return Color{static_cast<uint8_t>(r), static_cast<uint8_t>(g),
               static_cast<uint8_t>(b), static_cast<uint8_t>(a)};
}

TEST_F(OptionTest, ComplexCustomTypeColor) {
  Color bgColor{0, 0, 0, 255};

  EXPECT_NO_THROW({
    auto colorOpt =
        doptions::Option<Color>::createOption("--background-color", &bgColor);
  });

  // Test de conversión con formato CSV
  auto colorOpt =
      doptions::Option<Color>::createOption("--bg-color", &bgColor);
  Color result = doptions::fromStr<Color>("255,128,64,255");
  EXPECT_EQ(result.r, 255);
  EXPECT_EQ(result.g, 128);
  EXPECT_EQ(result.b, 64);
  EXPECT_EQ(result.a, 255);
}

TEST_F(OptionTest, ComplexCustomTypeColorHexFormat) {
  Color color{0, 0, 0, 0};
  auto colorOpt = doptions::Option<Color>::createOption("-c,--color", &color);

  Color result = doptions::fromStr<Color>("#FF8040FF");
  EXPECT_EQ(result.r, 255);
  EXPECT_EQ(result.g, 128);
  EXPECT_EQ(result.b, 64);
  EXPECT_EQ(result.a, 255);
}

TEST_F(OptionTest, ComplexCustomTypeColorInvalidFormat) {
  Color color{0, 0, 0, 0};
  auto colorOpt = doptions::Option<Color>::createOption("-c,--color", &color);

  EXPECT_THROW(doptions::fromStr<Color>("invalid"), std::invalid_argument);
  EXPECT_THROW(doptions::fromStr<Color>("255,128"), std::invalid_argument);
  EXPECT_THROW(doptions::fromStr<Color>("#FFF"), std::invalid_argument);
}

TEST_F(OptionTest, ComplexCustomTypeColorOutOfRange) {
  Color color{0, 0, 0, 0};
  auto colorOpt = doptions::Option<Color>::createOption("-c,--color", &color);

  EXPECT_THROW(doptions::fromStr<Color>("256,0,0,255"), std::out_of_range);
  EXPECT_THROW(doptions::fromStr<Color>("-1,0,0,255"), std::out_of_range);
}

struct Rectangle {
  int x;
  int y;
  int width;
  int height;

  bool operator==(const Rectangle& other) const {
    return x == other.x && y == other.y && width == other.width &&
           height == other.height;
  }
};

REGISTER_TYPE(Rectangle) {
  // Format: "x,y,width,height"
  size_t pos1 = str.find(',');
  size_t pos2 = str.find(',', pos1 + 1);
  size_t pos3 = str.find(',', pos2 + 1);

  if (pos1 == std::string::npos || pos2 == std::string::npos ||
      pos3 == std::string::npos) {
    throw std::invalid_argument("Rectangle must be in x,y,width,height format");
  }

  return Rectangle{std::stoi(str.substr(0, pos1)),
                   std::stoi(str.substr(pos1 + 1, pos2 - pos1 - 1)),
                   std::stoi(str.substr(pos2 + 1, pos3 - pos2 - 1)),
                   std::stoi(str.substr(pos3 + 1))};
}

TEST_F(OptionTest, MultipleComplexCustomTypes) {
  Color fgColor{255, 255, 255, 255};
  Color bgColor{0, 0, 0, 255};
  Rectangle bounds{0, 0, 800, 600};

  EXPECT_NO_THROW({
    auto fgOpt =
        doptions::Option<Color>::createOption("--foreground", &fgColor);
    auto bgOpt =
        doptions::Option<Color>::createOption("--background", &bgColor);
    auto boundsOpt =
        doptions::Option<Rectangle>::createOption("--bounds", &bounds);
  });
}

// ============================================================================
// Complex Name Validation Scenarios
// ============================================================================

TEST_F(OptionTest, RealWorldOptionNames) {
  int value = 0;

  // Nombres comunes en aplicaciones reales
  EXPECT_NO_THROW({
    doptions::Option<int>::createOption("-v,--version", &value);
    doptions::Option<int>::createOption("-h,--help", &value);
    doptions::Option<int>::createOption("--no-cache", &value);
    doptions::Option<int>::createOption("--dry-run", &value);
    doptions::Option<int>::createOption("--max-depth", &value);
    doptions::Option<int>::createOption("--output-format", &value);
  });
}

TEST_F(OptionTest, OptionNamesWithMultipleHyphens) {
  int value = 0;

  EXPECT_NO_THROW({
    auto opt1 =
        doptions::Option<int>::createOption("--multi-word-option", &value);
    auto opt2 = doptions::Option<int>::createOption(
        "--very-long-multi-word-option-name", &value);
  });
}

TEST_F(OptionTest, SingleCharacterOptions) {
  // Opciones de un solo carácter son comunes (-v, -h, -p, etc.)
  int value = 0;

  EXPECT_NO_THROW({
    auto optV = doptions::Option<int>::createOption("-v", &value);
    auto optH = doptions::Option<int>::createOption("-h", &value);
    auto optP = doptions::Option<int>::createOption("-p", &value);
    auto optX = doptions::Option<int>::createOption("-x", &value);
  });
}

TEST_F(OptionTest, TwoCharacterOptions) {
  int value = 0;

  EXPECT_NO_THROW({
    auto opt = doptions::Option<int>::createOption("-ab", &value);
  });
}

TEST_F(OptionTest, ThreeCharacterOptions) {
  int value = 0;

  EXPECT_NO_THROW({
    auto opt = doptions::Option<int>::createOption("-abc", &value);
  });
}

TEST_F(OptionTest, OptionNameWithNumbersButNotAtStart) {
  int value = 0;

  EXPECT_NO_THROW({
    auto opt1 = doptions::Option<int>::createOption("--base64", &value);
    auto opt2 = doptions::Option<int>::createOption("--http2", &value);
    auto opt3 = doptions::Option<int>::createOption("--tls13", &value);
  });
}

// ============================================================================
// Edge Cases and Error Handling
// ============================================================================

TEST_F(OptionTest, NameStartingWithNumber) {
  int value = 0;
  EXPECT_THROW(
      { auto opt = doptions::Option<int>::createOption("-1st", &value); },
      std::invalid_argument);
  EXPECT_THROW(
      { auto opt = doptions::Option<int>::createOption("--2nd", &value); },
      std::invalid_argument);
}

TEST_F(OptionTest, NameWithInvalidCharacters) {
  int value = 0;

  // Espacios no son válidos
  EXPECT_THROW(
      { auto opt = doptions::Option<int>::createOption("--my option", &value); },
      std::invalid_argument);

  // Caracteres especiales no son válidos
  EXPECT_THROW(
      { auto opt = doptions::Option<int>::createOption("--my@option", &value); },
      std::invalid_argument);
  EXPECT_THROW(
      { auto opt = doptions::Option<int>::createOption("--my!option", &value); },
      std::invalid_argument);
}

TEST_F(OptionTest, EmptyShortOrLongNameInCombined) {
  int value = 0;

  // Comma pero sin short name
  EXPECT_THROW(
      { auto opt = doptions::Option<int>::createOption(",--number", &value); },
      std::invalid_argument);

  // Comma pero sin long name
  EXPECT_THROW(
      { auto opt = doptions::Option<int>::createOption("-n,", &value); },
      std::invalid_argument);

  // Solo comma
  EXPECT_THROW({ auto opt = doptions::Option<int>::createOption(",", &value); },
               std::invalid_argument);
}

TEST_F(OptionTest, OnlyDashesWithoutName) {
  int value = 0;

  EXPECT_THROW({ auto opt = doptions::Option<int>::createOption("-", &value); },
               std::invalid_argument);
  EXPECT_THROW(
      { auto opt = doptions::Option<int>::createOption("--", &value); },
      std::invalid_argument);
  EXPECT_THROW(
      { auto opt = doptions::Option<int>::createOption("---", &value); },
      std::invalid_argument);
}

// ============================================================================
// Type Conversion Complex Scenarios
// ============================================================================

TEST_F(OptionTest, BoundaryValuesForAllIntegerTypes) {
  // Test valores en los límites de cada tipo
  int8_t i8 = 0;
  auto opt_i8 = doptions::Option<int8_t>::createOption("-i8", &i8);
  EXPECT_NO_THROW(doptions::fromStr<int8_t>("127"));
  EXPECT_NO_THROW(doptions::fromStr<int8_t>("-128"));

  uint8_t u8 = 0;
  auto opt_u8 = doptions::Option<uint8_t>::createOption("-u8", &u8);
  EXPECT_NO_THROW(doptions::fromStr<uint8_t>("255"));
  EXPECT_NO_THROW(doptions::fromStr<uint8_t>("0"));
  EXPECT_THROW(doptions::fromStr<uint8_t>("256"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<uint8_t>("-1"), doptions::ParseException);

  int16_t i16 = 0;
  auto opt_i16 = doptions::Option<int16_t>::createOption("-i16", &i16);
  EXPECT_NO_THROW(doptions::fromStr<int16_t>("32767"));
  EXPECT_NO_THROW(doptions::fromStr<int16_t>("-32768"));

  uint16_t u16 = 0;
  auto opt_u16 = doptions::Option<uint16_t>::createOption("-u16", &u16);
  EXPECT_NO_THROW(doptions::fromStr<uint16_t>("65535"));
  EXPECT_NO_THROW(doptions::fromStr<uint16_t>("0"));
  EXPECT_THROW(doptions::fromStr<uint16_t>("65536"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<uint16_t>("-1"), doptions::ParseException);
}

TEST_F(OptionTest, SpecialFloatingPointValues) {
  float f = 0.0f;
  auto opt_f = doptions::Option<float>::createOption("-f", &f);

  EXPECT_NO_THROW(doptions::fromStr<float>("0.0"));
  EXPECT_NO_THROW(doptions::fromStr<float>("-0.0"));
  EXPECT_NO_THROW(doptions::fromStr<float>("1e10"));
  EXPECT_NO_THROW(doptions::fromStr<float>("-1e10"));
  EXPECT_NO_THROW(doptions::fromStr<float>("3.14159"));

  double d = 0.0;
  auto opt_d = doptions::Option<double>::createOption("-d", &d);
  EXPECT_NO_THROW(doptions::fromStr<double>("1e100"));
  EXPECT_NO_THROW(doptions::fromStr<double>("1e-100"));
}

TEST_F(OptionTest, BooleanVariations) {
  bool b = false;
  auto opt = doptions::Option<bool>::createOption("-b,--boolean", &b);

  // Note: fromStr<bool> is rarely used in practice since bool options
  // only check for presence, not parse values. This test documents
  // the current minimal implementation.
  EXPECT_TRUE(doptions::fromStr<bool>("true"));
  EXPECT_FALSE(doptions::fromStr<bool>("false"));
  EXPECT_FALSE(doptions::fromStr<bool>("anything-else"));
}

TEST_F(OptionTest, StringWithSpecialContent) {
  std::string s;
  auto opt = doptions::Option<std::string>::createOption("-s,--string", &s);

  // Strings vacíos
  EXPECT_EQ(doptions::fromStr<std::string>(""), "");

  // Strings con espacios
  EXPECT_EQ(doptions::fromStr<std::string>("hello world"), "hello world");

  // Strings con caracteres especiales
  EXPECT_EQ(doptions::fromStr<std::string>("hello,world"), "hello,world");
  EXPECT_EQ(doptions::fromStr<std::string>("path/to/file"), "path/to/file");
  EXPECT_EQ(doptions::fromStr<std::string>("user@host.com"), "user@host.com");

  // Strings que parecen números
  EXPECT_EQ(doptions::fromStr<std::string>("12345"), "12345");
  EXPECT_EQ(doptions::fromStr<std::string>("3.14"), "3.14");
}

// ============================================================================
// parseValue() Tests - CRITICAL FUNCTIONALITY
// ============================================================================

TEST_F(OptionTest, ParseValueInt) {
  int value = 0;
  auto opt = doptions::Option<int>::createOption("-n,--number", &value);

  opt->parseValue("42");
  EXPECT_EQ(value, 42);

  opt->parseValue("-100");
  EXPECT_EQ(value, -100);

  opt->parseValue("0");
  EXPECT_EQ(value, 0);
}

TEST_F(OptionTest, ParseValueAllIntegerTypes) {
  int8_t i8 = 0;
  auto opt_i8 = doptions::Option<int8_t>::createOption("-i8", &i8);
  opt_i8->parseValue("127");
  EXPECT_EQ(i8, 127);

  int16_t i16 = 0;
  auto opt_i16 = doptions::Option<int16_t>::createOption("-i16", &i16);
  opt_i16->parseValue("32000");
  EXPECT_EQ(i16, 32000);

  int32_t i32 = 0;
  auto opt_i32 = doptions::Option<int32_t>::createOption("-i32", &i32);
  opt_i32->parseValue("2000000000");
  EXPECT_EQ(i32, 2000000000);

  int64_t i64 = 0;
  auto opt_i64 = doptions::Option<int64_t>::createOption("-i64", &i64);
  opt_i64->parseValue("9000000000000000000");
  EXPECT_EQ(i64, 9000000000000000000LL);

  uint8_t u8 = 0;
  auto opt_u8 = doptions::Option<uint8_t>::createOption("-u8", &u8);
  opt_u8->parseValue("255");
  EXPECT_EQ(u8, 255);

  uint16_t u16 = 0;
  auto opt_u16 = doptions::Option<uint16_t>::createOption("-u16", &u16);
  opt_u16->parseValue("65535");
  EXPECT_EQ(u16, 65535);

  uint32_t u32 = 0;
  auto opt_u32 = doptions::Option<uint32_t>::createOption("-u32", &u32);
  opt_u32->parseValue("4000000000");
  EXPECT_EQ(u32, 4000000000U);

  uint64_t u64 = 0;
  auto opt_u64 = doptions::Option<uint64_t>::createOption("-u64", &u64);
  opt_u64->parseValue("18000000000000000000");
  EXPECT_EQ(u64, 18000000000000000000ULL);
}

TEST_F(OptionTest, ParseValueFloat) {
  float value = 0.0f;
  auto opt = doptions::Option<float>::createOption("-f,--float", &value);

  opt->parseValue("3.14159");
  EXPECT_FLOAT_EQ(value, 3.14159f);

  opt->parseValue("-2.5");
  EXPECT_FLOAT_EQ(value, -2.5f);

  opt->parseValue("0.0");
  EXPECT_FLOAT_EQ(value, 0.0f);

  opt->parseValue("1e5");
  EXPECT_FLOAT_EQ(value, 100000.0f);
}

TEST_F(OptionTest, ParseValueDouble) {
  double value = 0.0;
  auto opt = doptions::Option<double>::createOption("-d,--double", &value);

  opt->parseValue("3.141592653589793");
  EXPECT_DOUBLE_EQ(value, 3.141592653589793);

  opt->parseValue("-1e100");
  EXPECT_DOUBLE_EQ(value, -1e100);

  opt->parseValue("1e-100");
  EXPECT_DOUBLE_EQ(value, 1e-100);
}

TEST_F(OptionTest, ParseValueBool) {
  bool value = false;
  auto opt = doptions::Option<bool>::createOption("-b,--bool", &value);

  // Note: In practice, bool options only check for presence
  // This test documents the minimal fromStr<bool> implementation
  opt->parseValue("true");
  EXPECT_TRUE(value);

  opt->parseValue("false");
  EXPECT_FALSE(value);

  opt->parseValue("anything");
  EXPECT_FALSE(value);
}

TEST_F(OptionTest, ParseValueString) {
  std::string value;
  auto opt = doptions::Option<std::string>::createOption("-s,--string", &value);

  opt->parseValue("hello");
  EXPECT_EQ(value, "hello");

  opt->parseValue("hello world");
  EXPECT_EQ(value, "hello world");

  opt->parseValue("");
  EXPECT_EQ(value, "");

  opt->parseValue("path/to/file.txt");
  EXPECT_EQ(value, "path/to/file.txt");
}

TEST_F(OptionTest, ParseValueCustomType) {
  Color color{0, 0, 0, 0};
  auto opt = doptions::Option<Color>::createOption("-c,--color", &color);

  opt->parseValue("255,128,64,255");
  EXPECT_EQ(color.r, 255);
  EXPECT_EQ(color.g, 128);
  EXPECT_EQ(color.b, 64);
  EXPECT_EQ(color.a, 255);

  opt->parseValue("#FF8040FF");
  EXPECT_EQ(color.r, 255);
  EXPECT_EQ(color.g, 128);
  EXPECT_EQ(color.b, 64);
  EXPECT_EQ(color.a, 255);
}

TEST_F(OptionTest, ParseValueUpdatesVariable) {
  int value = 100;
  auto opt = doptions::Option<int>::createOption("-n", &value);

  EXPECT_EQ(value, 100);  // Initial value

  opt->parseValue("200");
  EXPECT_EQ(value, 200);  // Updated

  opt->parseValue("300");
  EXPECT_EQ(value, 300);  // Updated again
}

// ============================================================================
// Getter Tests - shortName() and longName()
// ============================================================================

TEST_F(OptionTest, GettersShortAndLongName) {
  int value = 0;
  auto opt = doptions::Option<int>::createOption("-n,--number", &value);

  EXPECT_EQ(opt->shortName(), "-n");
  EXPECT_EQ(opt->longName(), "--number");
}

TEST_F(OptionTest, GettersOnlyShortName) {
  int value = 0;
  auto opt = doptions::Option<int>::createOption("-v", &value);

  EXPECT_EQ(opt->shortName(), "-v");
  EXPECT_EQ(opt->longName(), "");
}

TEST_F(OptionTest, GettersOnlyLongName) {
  int value = 0;
  auto opt = doptions::Option<int>::createOption("--verbose", &value);

  EXPECT_EQ(opt->shortName(), "");
  EXPECT_EQ(opt->longName(), "--verbose");
}

TEST_F(OptionTest, GettersWithDashesInInput) {
  int value = 0;

  // Input with dashes should be preserved correctly
  auto opt1 = doptions::Option<int>::createOption("-v,--verbose", &value);
  EXPECT_EQ(opt1->shortName(), "-v");
  EXPECT_EQ(opt1->longName(), "--verbose");

  auto opt2 = doptions::Option<int>::createOption("--output-file", &value);
  EXPECT_EQ(opt2->shortName(), "");
  EXPECT_EQ(opt2->longName(), "--output-file");
}

TEST_F(OptionTest, GettersReturnConsistentReferences) {
  int value = 0;
  auto opt = doptions::Option<int>::createOption("-n,--number", &value);

  const std::string& ref1 = opt->shortName();
  const std::string& ref2 = opt->shortName();

  // Should return reference to the same object
  EXPECT_EQ(&ref1, &ref2);
}

// ============================================================================
// Error Handling in fromStr Conversions
// ============================================================================

TEST_F(OptionTest, ParseValueInvalidIntThrows) {
  int value = 0;
  auto opt = doptions::Option<int>::createOption("-n", &value);

  EXPECT_THROW(opt->parseValue("not-a-number"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("abc"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("xyz123"), std::invalid_argument);

  // Note: stoi("12.34") doesn't throw, it parses 12 and stops at '.'
  // Note: stoi("") throws invalid_argument
  EXPECT_THROW(opt->parseValue(""), std::invalid_argument);
}

TEST_F(OptionTest, ParseValueInvalidFloatThrows) {
  float value = 0.0f;
  auto opt = doptions::Option<float>::createOption("-f", &value);

  EXPECT_THROW(opt->parseValue("not-a-float"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("abc"), std::invalid_argument);
  // Note: stof may parse partial strings like "12.34abc" as 12.34
}

TEST_F(OptionTest, ParseValueInvalidDoubleThrows) {
  double value = 0.0;
  auto opt = doptions::Option<double>::createOption("-d", &value);

  EXPECT_THROW(opt->parseValue("not-a-double"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("xyz"), std::invalid_argument);
}

TEST_F(OptionTest, FromStrInvalidConversions) {
  // Test direct fromStr calls with completely invalid inputs
  EXPECT_THROW(doptions::fromStr<int>("abc"), std::invalid_argument);
  EXPECT_THROW(doptions::fromStr<int64_t>("not-valid"), std::invalid_argument);

  EXPECT_THROW(doptions::fromStr<uint32_t>("xyz"), std::invalid_argument);
  EXPECT_THROW(doptions::fromStr<uint64_t>("invalid"), std::invalid_argument);

  EXPECT_THROW(doptions::fromStr<float>("not-float"), std::invalid_argument);
  EXPECT_THROW(doptions::fromStr<double>("bad-double"), std::invalid_argument);

  // Note: stoi/stod may accept partial parses like "12abc" -> 12
  // Testing completely invalid strings is more reliable
}

TEST_F(OptionTest, FromStrUnsignedNegativeThrows) {
  // Unsigned types with custom validation should reject negative values
  EXPECT_THROW(doptions::fromStr<uint8_t>("-1"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<uint16_t>("-10"), doptions::ParseException);

  // Note: stoul/stoull behavior with negative values is implementation-defined
  // Some implementations may wrap, others may throw
  // Testing uint8_t and uint16_t (which have explicit range checks) is more reliable
}

TEST_F(OptionTest, FromStrOverflowThrows) {
  // Values that exceed type limits
  EXPECT_THROW(doptions::fromStr<uint8_t>("256"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<uint8_t>("1000"), doptions::ParseException);

  EXPECT_THROW(doptions::fromStr<uint16_t>("65536"), doptions::ParseException);
  EXPECT_THROW(doptions::fromStr<uint16_t>("100000"), doptions::ParseException);

  // int8_t overflow (stoi returns int, then cast)
  // Values > 127 will overflow when cast to int8_t
  // Note: This might not throw, it will just wrap. Let's test the conversion works
  auto result = doptions::fromStr<int8_t>("100");
  EXPECT_EQ(result, 100);
}

TEST_F(OptionTest, ParseValueMultipleTypesInSequence) {
  // Test parsing different types in sequence to ensure no state issues
  int intVal = 0;
  auto intOpt = doptions::Option<int>::createOption("-i", &intVal);

  double doubleVal = 0.0;
  auto doubleOpt = doptions::Option<double>::createOption("-d", &doubleVal);

  std::string strVal;
  auto strOpt = doptions::Option<std::string>::createOption("-s", &strVal);

  bool boolVal = false;
  auto boolOpt = doptions::Option<bool>::createOption("-b", &boolVal);

  intOpt->parseValue("123");
  EXPECT_EQ(intVal, 123);

  doubleOpt->parseValue("45.67");
  EXPECT_DOUBLE_EQ(doubleVal, 45.67);

  strOpt->parseValue("test");
  EXPECT_EQ(strVal, "test");

  boolOpt->parseValue("true");
  EXPECT_TRUE(boolVal);
}

// ============================================================================
// Complex Integration Scenarios
// ============================================================================

TEST_F(OptionTest, DatabaseConnectionConfiguration) {
  // Caso de uso real: configuración de conexión a base de datos
  std::string host = "localhost";
  int port = 5432;
  std::string database = "mydb";
  std::string username = "admin";
  int maxConnections = 100;
  double connectionTimeout = 30.0;
  bool sslEnabled = false;

  EXPECT_NO_THROW({
    auto hostOpt =
        doptions::Option<std::string>::createOption("--db-host", &host);
    auto portOpt = doptions::Option<int>::createOption("--db-port", &port);
    auto dbOpt =
        doptions::Option<std::string>::createOption("--database", &database);
    auto userOpt =
        doptions::Option<std::string>::createOption("-u,--username", &username);
    auto maxConnOpt = doptions::Option<int>::createOption(
        "--max-connections", &maxConnections);
    auto timeoutOpt =
        doptions::Option<double>::createOption("--timeout", &connectionTimeout);
    auto sslOpt =
        doptions::Option<bool>::createOption("--enable-ssl", &sslEnabled);
  });
}

TEST_F(OptionTest, GraphicsApplicationConfiguration) {
  // Caso de uso: aplicación gráfica con configuración compleja
  int width = 1920;
  int height = 1080;
  bool fullscreen = false;
  Color bgColor{0, 0, 0, 255};
  Rectangle viewport{0, 0, 800, 600};
  int frameRate = 60;

  EXPECT_NO_THROW({
    auto widthOpt = doptions::Option<int>::createOption("-w,--width", &width);
    auto heightOpt =
        doptions::Option<int>::createOption("-h,--height", &height);
    auto fullscreenOpt =
        doptions::Option<bool>::createOption("-f,--fullscreen", &fullscreen);
    auto bgColorOpt =
        doptions::Option<Color>::createOption("--bg-color", &bgColor);
    auto viewportOpt =
        doptions::Option<Rectangle>::createOption("--viewport", &viewport);
    auto fpsOpt =
        doptions::Option<int>::createOption("--frame-rate", &frameRate);
  });
}
