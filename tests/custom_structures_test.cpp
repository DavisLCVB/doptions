#include <doptions/option.hpp>
#include <doptions/exceptions.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Test fixture for Custom Structures tests
class CustomStructuresTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
};

// ============================================================================
// Container Types - Vector
// ============================================================================

struct IntVector {
  std::vector<int> values;

  bool operator==(const IntVector& other) const {
    return values == other.values;
  }
};

REGISTER_TYPE(IntVector) {
  // Format: "[1,2,3,4,5]"
  if (str.empty() || str.front() != '[' || str.back() != ']') {
    throw std::invalid_argument("IntVector must be in [v1,v2,...] format");
  }

  std::string content = str.substr(1, str.length() - 2);
  if (content.empty()) {
    return IntVector{{}}; // Empty vector
  }

  std::vector<int> result;
  std::stringstream ss(content);
  std::string item;

  while (std::getline(ss, item, ',')) {
    // Trim whitespace
    item.erase(0, item.find_first_not_of(" \t"));
    item.erase(item.find_last_not_of(" \t") + 1);
    if (!item.empty()) {
      result.push_back(std::stoi(item));
    }
  }

  return IntVector{result};
}

TEST_F(CustomStructuresTest, VectorOfIntegers) {
  IntVector vec{};
  auto opt = doptions::Option<IntVector>::createOption("--values", &vec);

  opt->parseValue("[1,2,3,4,5]");
  EXPECT_EQ(vec.values.size(), 5);
  EXPECT_EQ(vec.values[0], 1);
  EXPECT_EQ(vec.values[4], 5);
}

TEST_F(CustomStructuresTest, VectorEmpty) {
  IntVector vec{};
  auto opt = doptions::Option<IntVector>::createOption("--values", &vec);

  opt->parseValue("[]");
  EXPECT_EQ(vec.values.size(), 0);
}

TEST_F(CustomStructuresTest, VectorWithSpaces) {
  IntVector vec{};
  auto opt = doptions::Option<IntVector>::createOption("--values", &vec);

  opt->parseValue("[ 1 , 2 , 3 ]");
  EXPECT_EQ(vec.values.size(), 3);
  EXPECT_EQ(vec.values[0], 1);
  EXPECT_EQ(vec.values[1], 2);
  EXPECT_EQ(vec.values[2], 3);
}

TEST_F(CustomStructuresTest, VectorInvalidFormat) {
  IntVector vec{};
  auto opt = doptions::Option<IntVector>::createOption("--values", &vec);

  EXPECT_THROW(opt->parseValue("1,2,3"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("[1,2,3"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("1,2,3]"), std::invalid_argument);
}

// ============================================================================
// Container Types - String Vector
// ============================================================================

struct StringVector {
  std::vector<std::string> values;

  bool operator==(const StringVector& other) const {
    return values == other.values;
  }
};

REGISTER_TYPE(StringVector) {
  // Format: "[foo,bar,baz]"
  if (str.empty() || str.front() != '[' || str.back() != ']') {
    throw std::invalid_argument("StringVector must be in [v1,v2,...] format");
  }

  std::string content = str.substr(1, str.length() - 2);
  if (content.empty()) {
    return StringVector{{}};
  }

  std::vector<std::string> result;
  std::stringstream ss(content);
  std::string item;

  while (std::getline(ss, item, ',')) {
    // Trim whitespace
    item.erase(0, item.find_first_not_of(" \t"));
    item.erase(item.find_last_not_of(" \t") + 1);
    if (!item.empty()) {
      result.push_back(item);
    }
  }

  return StringVector{result};
}

TEST_F(CustomStructuresTest, VectorOfStrings) {
  StringVector vec{};
  auto opt = doptions::Option<StringVector>::createOption("--names", &vec);

  opt->parseValue("[alice,bob,charlie]");
  EXPECT_EQ(vec.values.size(), 3);
  EXPECT_EQ(vec.values[0], "alice");
  EXPECT_EQ(vec.values[1], "bob");
  EXPECT_EQ(vec.values[2], "charlie");
}

TEST_F(CustomStructuresTest, VectorOfStringsWithSpaces) {
  StringVector vec{};
  auto opt = doptions::Option<StringVector>::createOption("--names", &vec);

  opt->parseValue("[ alpha , beta , gamma ]");
  EXPECT_EQ(vec.values.size(), 3);
  EXPECT_EQ(vec.values[0], "alpha");
  EXPECT_EQ(vec.values[1], "beta");
  EXPECT_EQ(vec.values[2], "gamma");
}

// ============================================================================
// Container Types - Key-Value Pairs (Map-like)
// ============================================================================

struct KeyValuePairs {
  std::map<std::string, std::string> data;

  bool operator==(const KeyValuePairs& other) const {
    return data == other.data;
  }
};

REGISTER_TYPE(KeyValuePairs) {
  // Format: "{key1:val1,key2:val2}"
  if (str.empty() || str.front() != '{' || str.back() != '}') {
    throw std::invalid_argument("KeyValuePairs must be in {k1:v1,k2:v2} format");
  }

  std::string content = str.substr(1, str.length() - 2);
  std::map<std::string, std::string> result;

  if (content.empty()) {
    return KeyValuePairs{result};
  }

  std::stringstream ss(content);
  std::string pair;

  while (std::getline(ss, pair, ',')) {
    size_t colon = pair.find(':');
    if (colon == std::string::npos) {
      throw std::invalid_argument("Invalid key:value pair format");
    }

    std::string key = pair.substr(0, colon);
    std::string value = pair.substr(colon + 1);

    // Trim whitespace
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);

    if (key.empty()) {
      throw std::invalid_argument("Key cannot be empty");
    }

    result[key] = value;
  }

  return KeyValuePairs{result};
}

TEST_F(CustomStructuresTest, KeyValuePairs) {
  KeyValuePairs kvp{};
  auto opt = doptions::Option<KeyValuePairs>::createOption("--config", &kvp);

  opt->parseValue("{host:localhost,port:8080,db:mydb}");
  EXPECT_EQ(kvp.data.size(), 3);
  EXPECT_EQ(kvp.data["host"], "localhost");
  EXPECT_EQ(kvp.data["port"], "8080");
  EXPECT_EQ(kvp.data["db"], "mydb");
}

TEST_F(CustomStructuresTest, KeyValuePairsEmpty) {
  KeyValuePairs kvp{};
  auto opt = doptions::Option<KeyValuePairs>::createOption("--config", &kvp);

  opt->parseValue("{}");
  EXPECT_EQ(kvp.data.size(), 0);
}

TEST_F(CustomStructuresTest, KeyValuePairsWithSpaces) {
  KeyValuePairs kvp{};
  auto opt = doptions::Option<KeyValuePairs>::createOption("--config", &kvp);

  opt->parseValue("{ name : test , value : 42 }");
  EXPECT_EQ(kvp.data.size(), 2);
  EXPECT_EQ(kvp.data["name"], "test");
  EXPECT_EQ(kvp.data["value"], "42");
}

TEST_F(CustomStructuresTest, KeyValuePairsInvalidFormat) {
  KeyValuePairs kvp{};
  auto opt = doptions::Option<KeyValuePairs>::createOption("--config", &kvp);

  EXPECT_THROW(opt->parseValue("{key1,key2}"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("{:value}"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("key:value"), std::invalid_argument);
}

// ============================================================================
// Nested Structures - Point and Polygon
// ============================================================================

struct Point {
  double x;
  double y;

  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
};

REGISTER_TYPE(Point) {
  // Format: "(x,y)"
  if (str.empty() || str.front() != '(' || str.back() != ')') {
    throw std::invalid_argument("Point must be in (x,y) format");
  }

  std::string content = str.substr(1, str.length() - 2);
  size_t comma = content.find(',');

  if (comma == std::string::npos) {
    throw std::invalid_argument("Point must have x and y coordinates");
  }

  std::string xStr = content.substr(0, comma);
  std::string yStr = content.substr(comma + 1);

  return Point{std::stod(xStr), std::stod(yStr)};
}

struct Polygon {
  std::vector<Point> vertices;

  bool operator==(const Polygon& other) const {
    return vertices == other.vertices;
  }
};

REGISTER_TYPE(Polygon) {
  // Format: "[(x1,y1),(x2,y2),(x3,y3)]"
  if (str.empty() || str.front() != '[' || str.back() != ']') {
    throw std::invalid_argument("Polygon must be in [(x,y),...] format");
  }

  std::string content = str.substr(1, str.length() - 2);
  std::vector<Point> vertices;

  if (content.empty()) {
    return Polygon{vertices};
  }

  size_t pos = 0;
  while (pos < content.length()) {
    size_t start = content.find('(', pos);
    if (start == std::string::npos) break;

    size_t end = content.find(')', start);
    if (end == std::string::npos) {
      throw std::invalid_argument("Malformed point in polygon");
    }

    std::string pointStr = content.substr(start, end - start + 1);
    vertices.push_back(doptions::fromStr<Point>(pointStr));

    pos = end + 1;
  }

  if (vertices.size() < 3) {
    throw std::invalid_argument("Polygon must have at least 3 vertices");
  }

  return Polygon{vertices};
}

TEST_F(CustomStructuresTest, NestedPoint) {
  Point p{};
  auto opt = doptions::Option<Point>::createOption("--point", &p);

  opt->parseValue("(3.14,2.71)");
  EXPECT_DOUBLE_EQ(p.x, 3.14);
  EXPECT_DOUBLE_EQ(p.y, 2.71);
}

TEST_F(CustomStructuresTest, NestedPolygon) {
  Polygon poly{};
  auto opt = doptions::Option<Polygon>::createOption("--polygon", &poly);

  opt->parseValue("[(0,0),(1,0),(1,1),(0,1)]");
  EXPECT_EQ(poly.vertices.size(), 4);
  EXPECT_DOUBLE_EQ(poly.vertices[0].x, 0.0);
  EXPECT_DOUBLE_EQ(poly.vertices[0].y, 0.0);
  EXPECT_DOUBLE_EQ(poly.vertices[2].x, 1.0);
  EXPECT_DOUBLE_EQ(poly.vertices[2].y, 1.0);
}

TEST_F(CustomStructuresTest, PolygonTriangle) {
  Polygon poly{};
  auto opt = doptions::Option<Polygon>::createOption("--polygon", &poly);

  opt->parseValue("[(0,0),(5,0),(2.5,4.33)]");
  EXPECT_EQ(poly.vertices.size(), 3);
}

TEST_F(CustomStructuresTest, PolygonTooFewVertices) {
  Polygon poly{};
  auto opt = doptions::Option<Polygon>::createOption("--polygon", &poly);

  EXPECT_THROW(opt->parseValue("[(0,0),(1,1)]"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("[(0,0)]"), std::invalid_argument);
}

// ============================================================================
// Complex Nested Structure - Network Configuration
// ============================================================================

struct IPAddress {
  uint8_t octets[4];

  bool operator==(const IPAddress& other) const {
    return octets[0] == other.octets[0] && octets[1] == other.octets[1] &&
           octets[2] == other.octets[2] && octets[3] == other.octets[3];
  }
};

REGISTER_TYPE(IPAddress) {
  // Format: "192.168.1.1"
  std::vector<uint8_t> parts;
  std::stringstream ss(str);
  std::string part;

  while (std::getline(ss, part, '.')) {
    int value = std::stoi(part);
    if (value < 0 || value > 255) {
      throw std::out_of_range("IP octet must be 0-255");
    }
    parts.push_back(static_cast<uint8_t>(value));
  }

  if (parts.size() != 4) {
    throw std::invalid_argument("IP address must have 4 octets");
  }

  IPAddress ip;
  ip.octets[0] = parts[0];
  ip.octets[1] = parts[1];
  ip.octets[2] = parts[2];
  ip.octets[3] = parts[3];
  return ip;
}

struct NetworkConfig {
  IPAddress address;
  IPAddress netmask;
  IPAddress gateway;

  bool operator==(const NetworkConfig& other) const {
    return address == other.address && netmask == other.netmask &&
           gateway == other.gateway;
  }
};

REGISTER_TYPE(NetworkConfig) {
  // Format: "address:192.168.1.10,netmask:255.255.255.0,gateway:192.168.1.1"
  std::map<std::string, std::string> config;
  std::stringstream ss(str);
  std::string pair;

  while (std::getline(ss, pair, ',')) {
    size_t colon = pair.find(':');
    if (colon == std::string::npos) {
      throw std::invalid_argument("Invalid network config format");
    }

    std::string key = pair.substr(0, colon);
    std::string value = pair.substr(colon + 1);

    config[key] = value;
  }

  if (config.find("address") == config.end() ||
      config.find("netmask") == config.end() ||
      config.find("gateway") == config.end()) {
    throw std::invalid_argument(
        "NetworkConfig must have address, netmask, and gateway");
  }

  NetworkConfig result;
  result.address = doptions::fromStr<IPAddress>(config["address"]);
  result.netmask = doptions::fromStr<IPAddress>(config["netmask"]);
  result.gateway = doptions::fromStr<IPAddress>(config["gateway"]);

  return result;
}

TEST_F(CustomStructuresTest, IPAddressBasic) {
  IPAddress ip{};
  auto opt = doptions::Option<IPAddress>::createOption("--ip-address", &ip);

  opt->parseValue("192.168.1.1");
  EXPECT_EQ(ip.octets[0], 192);
  EXPECT_EQ(ip.octets[1], 168);
  EXPECT_EQ(ip.octets[2], 1);
  EXPECT_EQ(ip.octets[3], 1);
}

TEST_F(CustomStructuresTest, IPAddressInvalidOctet) {
  IPAddress ip{};
  auto opt = doptions::Option<IPAddress>::createOption("--ip-address", &ip);

  EXPECT_THROW(opt->parseValue("256.0.0.1"), std::out_of_range);
  EXPECT_THROW(opt->parseValue("192.168.1.-1"), std::out_of_range);
}

TEST_F(CustomStructuresTest, IPAddressInvalidFormat) {
  IPAddress ip{};
  auto opt = doptions::Option<IPAddress>::createOption("--ip-address", &ip);

  EXPECT_THROW(opt->parseValue("192.168.1"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("192.168.1.1.1"), std::invalid_argument);
}

TEST_F(CustomStructuresTest, NetworkConfigComplete) {
  NetworkConfig config{};
  auto opt = doptions::Option<NetworkConfig>::createOption("--network", &config);

  opt->parseValue(
      "address:192.168.1.100,netmask:255.255.255.0,gateway:192.168.1.1");

  EXPECT_EQ(config.address.octets[0], 192);
  EXPECT_EQ(config.address.octets[3], 100);
  EXPECT_EQ(config.netmask.octets[0], 255);
  EXPECT_EQ(config.netmask.octets[3], 0);
  EXPECT_EQ(config.gateway.octets[3], 1);
}

TEST_F(CustomStructuresTest, NetworkConfigMissingField) {
  NetworkConfig config{};
  auto opt = doptions::Option<NetworkConfig>::createOption("--network", &config);

  EXPECT_THROW(opt->parseValue("address:192.168.1.1,netmask:255.255.255.0"),
               std::invalid_argument);
}

// ============================================================================
// Range Structure
// ============================================================================

struct Range {
  int min;
  int max;

  bool operator==(const Range& other) const {
    return min == other.min && max == other.max;
  }
};

REGISTER_TYPE(Range) {
  // Format: "min..max" or "min-max"
  size_t separator = str.find("..");
  if (separator == std::string::npos) {
    separator = str.find('-', 1);  // Start from 1 to allow negative numbers
    if (separator == std::string::npos) {
      throw std::invalid_argument("Range must be in min..max or min-max format");
    }
  }

  std::string minStr = str.substr(0, separator);
  std::string maxStr;

  if (str[separator] == '.') {
    maxStr = str.substr(separator + 2);  // Skip ".."
  } else {
    maxStr = str.substr(separator + 1);  // Skip "-"
  }

  int minVal = std::stoi(minStr);
  int maxVal = std::stoi(maxStr);

  if (minVal > maxVal) {
    throw std::invalid_argument("Range min must be <= max");
  }

  return Range{minVal, maxVal};
}

TEST_F(CustomStructuresTest, RangeDoubleDot) {
  Range range{};
  auto opt = doptions::Option<Range>::createOption("--range", &range);

  opt->parseValue("1..100");
  EXPECT_EQ(range.min, 1);
  EXPECT_EQ(range.max, 100);
}

TEST_F(CustomStructuresTest, RangeDash) {
  Range range{};
  auto opt = doptions::Option<Range>::createOption("--range", &range);

  opt->parseValue("10-50");
  EXPECT_EQ(range.min, 10);
  EXPECT_EQ(range.max, 50);
}

TEST_F(CustomStructuresTest, RangeNegativeValues) {
  Range range{};
  auto opt = doptions::Option<Range>::createOption("--range", &range);

  opt->parseValue("-10..10");
  EXPECT_EQ(range.min, -10);
  EXPECT_EQ(range.max, 10);
}

TEST_F(CustomStructuresTest, RangeInvalid) {
  Range range{};
  auto opt = doptions::Option<Range>::createOption("--range", &range);

  EXPECT_THROW(opt->parseValue("100..10"), std::invalid_argument);
}

// ============================================================================
// Enumeration-like Structure
// ============================================================================

struct LogLevel {
  enum Level { DEBUG, INFO, WARNING, ERROR, CRITICAL };
  Level level;

  bool operator==(const LogLevel& other) const { return level == other.level; }
};

REGISTER_TYPE(LogLevel) {
  std::string upper = str;
  std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

  if (upper == "DEBUG") return LogLevel{LogLevel::DEBUG};
  if (upper == "INFO") return LogLevel{LogLevel::INFO};
  if (upper == "WARNING" || upper == "WARN") return LogLevel{LogLevel::WARNING};
  if (upper == "ERROR") return LogLevel{LogLevel::ERROR};
  if (upper == "CRITICAL" || upper == "CRIT") return LogLevel{LogLevel::CRITICAL};

  throw std::invalid_argument("Invalid log level: " + str);
}

TEST_F(CustomStructuresTest, LogLevelCaseInsensitive) {
  LogLevel level{};
  auto opt = doptions::Option<LogLevel>::createOption("--log-level", &level);

  opt->parseValue("debug");
  EXPECT_EQ(level.level, LogLevel::DEBUG);

  opt->parseValue("ERROR");
  EXPECT_EQ(level.level, LogLevel::ERROR);

  opt->parseValue("WaRnInG");
  EXPECT_EQ(level.level, LogLevel::WARNING);
}

TEST_F(CustomStructuresTest, LogLevelAlias) {
  LogLevel level{};
  auto opt = doptions::Option<LogLevel>::createOption("--log-level", &level);

  opt->parseValue("warn");
  EXPECT_EQ(level.level, LogLevel::WARNING);

  opt->parseValue("crit");
  EXPECT_EQ(level.level, LogLevel::CRITICAL);
}

TEST_F(CustomStructuresTest, LogLevelInvalid) {
  LogLevel level{};
  auto opt = doptions::Option<LogLevel>::createOption("--log-level", &level);

  EXPECT_THROW(opt->parseValue("invalid"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("trace"), std::invalid_argument);
}

// ============================================================================
// Duration Structure
// ============================================================================

struct Duration {
  uint64_t milliseconds;

  bool operator==(const Duration& other) const {
    return milliseconds == other.milliseconds;
  }
};

REGISTER_TYPE(Duration) {
  // Format: "10s", "500ms", "5m", "2h", "1d"
  if (str.empty()) {
    throw std::invalid_argument("Duration cannot be empty");
  }

  size_t i = 0;
  while (i < str.length() && std::isdigit(str[i])) {
    i++;
  }

  if (i == 0 || i == str.length()) {
    throw std::invalid_argument(
        "Duration must be number followed by unit (ms, s, m, h, d)");
  }

  uint64_t value = std::stoull(str.substr(0, i));
  std::string unit = str.substr(i);

  uint64_t multiplier = 1;
  if (unit == "ms") {
    multiplier = 1;
  } else if (unit == "s") {
    multiplier = 1000;
  } else if (unit == "m") {
    multiplier = 60000;
  } else if (unit == "h") {
    multiplier = 3600000;
  } else if (unit == "d") {
    multiplier = 86400000;
  } else {
    throw std::invalid_argument("Invalid duration unit: " + unit);
  }

  return Duration{value * multiplier};
}

TEST_F(CustomStructuresTest, DurationMilliseconds) {
  Duration dur{};
  auto opt = doptions::Option<Duration>::createOption("--timeout", &dur);

  opt->parseValue("500ms");
  EXPECT_EQ(dur.milliseconds, 500);
}

TEST_F(CustomStructuresTest, DurationSeconds) {
  Duration dur{};
  auto opt = doptions::Option<Duration>::createOption("--timeout", &dur);

  opt->parseValue("30s");
  EXPECT_EQ(dur.milliseconds, 30000);
}

TEST_F(CustomStructuresTest, DurationMinutes) {
  Duration dur{};
  auto opt = doptions::Option<Duration>::createOption("--timeout", &dur);

  opt->parseValue("5m");
  EXPECT_EQ(dur.milliseconds, 300000);
}

TEST_F(CustomStructuresTest, DurationHours) {
  Duration dur{};
  auto opt = doptions::Option<Duration>::createOption("--timeout", &dur);

  opt->parseValue("2h");
  EXPECT_EQ(dur.milliseconds, 7200000);
}

TEST_F(CustomStructuresTest, DurationDays) {
  Duration dur{};
  auto opt = doptions::Option<Duration>::createOption("--timeout", &dur);

  opt->parseValue("1d");
  EXPECT_EQ(dur.milliseconds, 86400000);
}

TEST_F(CustomStructuresTest, DurationInvalidUnit) {
  Duration dur{};
  auto opt = doptions::Option<Duration>::createOption("--timeout", &dur);

  EXPECT_THROW(opt->parseValue("10x"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("5w"), std::invalid_argument);
}

TEST_F(CustomStructuresTest, DurationInvalidFormat) {
  Duration dur{};
  auto opt = doptions::Option<Duration>::createOption("--timeout", &dur);

  EXPECT_THROW(opt->parseValue("ms"), std::invalid_argument);
  EXPECT_THROW(opt->parseValue("10"), std::invalid_argument);
}

// ============================================================================
// Complex Real-World Scenario - Server Configuration
// ============================================================================

struct ServerConfig {
  IPAddress bindAddress;
  int port;
  Duration timeout;
  LogLevel logLevel;
  std::vector<std::string> allowedHosts;

  bool operator==(const ServerConfig& other) const {
    return bindAddress == other.bindAddress && port == other.port &&
           timeout == other.timeout && logLevel == other.logLevel &&
           allowedHosts == other.allowedHosts;
  }
};

TEST_F(CustomStructuresTest, ComplexServerConfiguration) {
  // This test demonstrates using multiple custom types together
  IPAddress addr{};
  auto addrOpt =
      doptions::Option<IPAddress>::createOption("--bind-address", &addr);

  int port = 0;
  auto portOpt = doptions::Option<int>::createOption("-p,--port", &port);

  Duration timeout{};
  auto timeoutOpt =
      doptions::Option<Duration>::createOption("--timeout", &timeout);

  LogLevel logLevel{};
  auto logLevelOpt =
      doptions::Option<LogLevel>::createOption("--log-level", &logLevel);

  StringVector hosts{};
  auto hostsOpt =
      doptions::Option<StringVector>::createOption("--allowed-hosts", &hosts);

  // Simulate parsing command-line arguments
  addrOpt->parseValue("0.0.0.0");
  portOpt->parseValue("8080");
  timeoutOpt->parseValue("30s");
  logLevelOpt->parseValue("info");
  hostsOpt->parseValue("[localhost,example.com,trusted.org]");

  // Verify all values were parsed correctly
  EXPECT_EQ(addr.octets[0], 0);
  EXPECT_EQ(addr.octets[3], 0);
  EXPECT_EQ(port, 8080);
  EXPECT_EQ(timeout.milliseconds, 30000);
  EXPECT_EQ(logLevel.level, LogLevel::INFO);
  EXPECT_EQ(hosts.values.size(), 3);
  EXPECT_EQ(hosts.values[0], "localhost");
}

// ============================================================================
// Advanced Container - Set
// ============================================================================

struct IntSet {
  std::set<int> values;

  bool operator==(const IntSet& other) const { return values == other.values; }
};

REGISTER_TYPE(IntSet) {
  // Format: "{1,2,3,4,5}" - automatically handles duplicates
  if (str.empty() || str.front() != '{' || str.back() != '}') {
    throw std::invalid_argument("IntSet must be in {v1,v2,...} format");
  }

  std::string content = str.substr(1, str.length() - 2);
  std::set<int> result;

  if (content.empty()) {
    return IntSet{result};
  }

  std::stringstream ss(content);
  std::string item;

  while (std::getline(ss, item, ',')) {
    item.erase(0, item.find_first_not_of(" \t"));
    item.erase(item.find_last_not_of(" \t") + 1);
    if (!item.empty()) {
      result.insert(std::stoi(item));
    }
  }

  return IntSet{result};
}

TEST_F(CustomStructuresTest, SetOfIntegers) {
  IntSet set{};
  auto opt = doptions::Option<IntSet>::createOption("--unique-values", &set);

  opt->parseValue("{1,2,3,4,5}");
  EXPECT_EQ(set.values.size(), 5);
  EXPECT_TRUE(set.values.count(3) > 0);
}

TEST_F(CustomStructuresTest, SetRemovesDuplicates) {
  IntSet set{};
  auto opt = doptions::Option<IntSet>::createOption("--unique-values", &set);

  opt->parseValue("{1,2,2,3,3,3,4}");
  EXPECT_EQ(set.values.size(), 4);  // Duplicates removed
}

TEST_F(CustomStructuresTest, SetSorted) {
  IntSet set{};
  auto opt = doptions::Option<IntSet>::createOption("--unique-values", &set);

  opt->parseValue("{5,1,3,2,4}");
  EXPECT_EQ(set.values.size(), 5);

  // std::set maintains sorted order
  auto it = set.values.begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
}
