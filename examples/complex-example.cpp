// Complex example - Advanced features with custom types and nested commands
#include <doptions/doptions.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sstream>

// Custom type: LogLevel
enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };

REGISTER_TYPE(LogLevel) {
  std::string lower = str;
  std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

  if (lower == "trace") return LogLevel::TRACE;
  if (lower == "debug") return LogLevel::DEBUG;
  if (lower == "info") return LogLevel::INFO;
  if (lower == "warn" || lower == "warning") return LogLevel::WARN;
  if (lower == "error") return LogLevel::ERROR;
  if (lower == "fatal") return LogLevel::FATAL;

  throw std::invalid_argument("Invalid log level: " + str +
                              " (valid: trace, debug, info, warn, error, fatal)");
}

// Custom type: DatabaseConfig
struct DatabaseConfig {
  std::string host;
  uint16_t port;
  std::string database;
  std::string username;
};

REGISTER_TYPE(DatabaseConfig) {
  // Format: host:port/database@username
  DatabaseConfig config;

  auto atPos = str.find('@');
  if (atPos == std::string::npos) {
    throw std::invalid_argument("Invalid database config format. Expected: host:port/database@username");
  }

  config.username = str.substr(atPos + 1);
  std::string hostPart = str.substr(0, atPos);

  auto slashPos = hostPart.find('/');
  if (slashPos == std::string::npos) {
    throw std::invalid_argument("Missing database name in config");
  }

  config.database = hostPart.substr(slashPos + 1);
  std::string hostPortPart = hostPart.substr(0, slashPos);

  auto colonPos = hostPortPart.find(':');
  if (colonPos == std::string::npos) {
    config.host = hostPortPart;
    config.port = 5432; // Default PostgreSQL port
  } else {
    config.host = hostPortPart.substr(0, colonPos);
    config.port = static_cast<uint16_t>(std::stoi(hostPortPart.substr(colonPos + 1)));
  }

  return config;
}

// Custom type: TimeRange
struct TimeRange {
  int start;
  int end;
};

REGISTER_TYPE(TimeRange) {
  // Format: start-end or start..end
  TimeRange range;

  size_t pos = str.find('-');
  if (pos == std::string::npos) {
    pos = str.find("..");
    if (pos == std::string::npos) {
      throw std::invalid_argument("Invalid time range format. Expected: start-end or start..end");
    }
  }

  range.start = std::stoi(str.substr(0, pos));
  size_t endStart = (str[pos] == '.') ? pos + 2 : pos + 1;
  range.end = std::stoi(str.substr(endStart));

  if (range.start >= range.end) {
    throw std::invalid_argument("Invalid range: start must be less than end");
  }

  return range;
}

// Helper function to convert LogLevel to string
auto logLevelToString(LogLevel level) -> std::string {
  switch (level) {
    case LogLevel::TRACE: return "TRACE";
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARN: return "WARN";
    case LogLevel::ERROR: return "ERROR";
    case LogLevel::FATAL: return "FATAL";
    default: return "UNKNOWN";
  }
}

auto main(int argc, char** argv) -> int {
  // Create application
  auto app = doptions::Application::createApp();

  // Global options
  LogLevel logLevel = LogLevel::INFO;
  std::string logFile;
  bool colorOutput = true;
  bool timestamp = true;

  app.addOption("-l,--log-level", &logLevel);
  app.addOption("--log-file", &logFile);
  app.addOption("--no-color", &colorOutput);  // Will invert the boolean
  app.addOption("--timestamp", &timestamp);

  // ========== Server Command ==========
  bool serverExecuted = false;
  std::string serverHost = "0.0.0.0";
  uint16_t serverPort = 8080;
  int32_t workers = 4;
  int64_t maxConnections = 1000;
  double timeout = 30.0;
  bool enableSsl = false;
  std::string sslCert;
  std::string sslKey;

  auto& serverCmd = app.addCommand("server", &serverExecuted);
  serverCmd->addOption("-h,--host", &serverHost);
  serverCmd->addOption("-p,--port", &serverPort);
  serverCmd->addOption("-w,--workers", &workers);
  serverCmd->addOption("--max-connections", &maxConnections);
  serverCmd->addOption("-t,--timeout", &timeout);
  serverCmd->addOption("--enable-ssl", &enableSsl);
  serverCmd->addOption("--ssl-cert", &sslCert);
  serverCmd->addOption("--ssl-key", &sslKey);

  // ========== Database Command ==========
  bool databaseExecuted = false;
  DatabaseConfig dbConfig;
  std::string operation = "migrate";
  bool dryRun = false;
  int32_t batchSize = 1000;
  TimeRange timeRange{0, 0};

  auto& dbCmd = app.addCommand("database", &databaseExecuted);
  dbCmd->addOption("--config", &dbConfig);
  dbCmd->addOption("--operation", &operation);
  dbCmd->addOption("--dry-run", &dryRun);
  dbCmd->addOption("--batch-size", &batchSize);
  dbCmd->addOption("--time-range", &timeRange);

  // ========== Process Command ==========
  bool processExecuted = false;
  std::string inputFile;
  std::string outputFile;
  std::string format = "json";
  bool compress = false;
  uint8_t compressionLevel = 6;
  int64_t maxSize = 1024 * 1024 * 100;  // 100MB
  bool parallel = false;
  int32_t threads = 0;

  auto& processCmd = app.addCommand("process", &processExecuted);
  processCmd->addOption("-i,--input", &inputFile);
  processCmd->addOption("-o,--output", &outputFile);
  processCmd->addOption("-f,--format", &format);
  processCmd->addOption("-c,--compress", &compress);
  processCmd->addOption("--compression-level", &compressionLevel);
  processCmd->addOption("--max-size", &maxSize);
  processCmd->addOption("--parallel", &parallel);
  processCmd->addOption("--threads", &threads);

  // ========== Analytics Command ==========
  bool analyticsExecuted = false;
  std::string dataSource;
  TimeRange analysisRange{0, 0};
  std::string metrics = "all";
  std::string aggregation = "hourly";
  bool exportCsv = false;
  std::string csvPath;
  double threshold = 0.0;

  auto& analyticsCmd = app.addCommand("analytics", &analyticsExecuted);
  analyticsCmd->addOption("--data-source", &dataSource);
  analyticsCmd->addOption("--range", &analysisRange);
  analyticsCmd->addOption("--metrics", &metrics);
  analyticsCmd->addOption("--aggregation", &aggregation);
  analyticsCmd->addOption("--export-csv", &exportCsv);
  analyticsCmd->addOption("--csv-path", &csvPath);
  analyticsCmd->addOption("--threshold", &threshold);

  try {
    // Parse command line arguments
    app.parse(argc, argv);

    // Fix color output (was inverted)
    colorOutput = !colorOutput;

    // Display global settings
    std::cout << std::string(60, '=') << "\n";
    std::cout << "=== Global Configuration ===\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "Log Level: " << logLevelToString(logLevel) << "\n";
    if (!logFile.empty()) {
      std::cout << "Log File: " << logFile << "\n";
    }
    std::cout << "Color Output: " << (colorOutput ? "enabled" : "disabled") << "\n";
    std::cout << "Timestamps: " << (timestamp ? "enabled" : "disabled") << "\n";
    std::cout << "\n";

    // Execute the appropriate command
    if (serverExecuted) {
      std::cout << std::string(60, '=') << "\n";
      std::cout << "=== Starting Server ===\n";
      std::cout << std::string(60, '=') << "\n";
      std::cout << "Host: " << serverHost << "\n";
      std::cout << "Port: " << serverPort << "\n";
      std::cout << "Workers: " << workers << "\n";
      std::cout << "Max Connections: " << maxConnections << "\n";
      std::cout << "Timeout: " << std::fixed << std::setprecision(1) << timeout << "s\n";

      if (enableSsl) {
        std::cout << "\nSSL Configuration:\n";
        std::cout << "  Enabled: yes\n";
        std::cout << "  Certificate: " << (sslCert.empty() ? "default" : sslCert) << "\n";
        std::cout << "  Key: " << (sslKey.empty() ? "default" : sslKey) << "\n";
      }

      std::cout << "\nServer is running at " << (enableSsl ? "https://" : "http://")
                << serverHost << ":" << serverPort << "\n";
      std::cout << "Press Ctrl+C to stop...\n";

    } else if (databaseExecuted) {
      std::cout << std::string(60, '=') << "\n";
      std::cout << "=== Database Operation ===\n";
      std::cout << std::string(60, '=') << "\n";
      std::cout << "Database: " << dbConfig.database << "\n";
      std::cout << "Host: " << dbConfig.host << ":" << dbConfig.port << "\n";
      std::cout << "Username: " << dbConfig.username << "\n";
      std::cout << "Operation: " << operation << "\n";
      std::cout << "Batch Size: " << batchSize << "\n";

      if (timeRange.end > 0) {
        std::cout << "Time Range: " << timeRange.start << " to " << timeRange.end << "\n";
      }

      if (dryRun) {
        std::cout << "\n[DRY RUN MODE] No changes will be applied\n";
      }

      std::cout << "\nConnecting to database...\n";
      std::cout << "Executing " << operation << "...\n";
      std::cout << "Operation completed successfully!\n";

    } else if (processExecuted) {
      std::cout << std::string(60, '=') << "\n";
      std::cout << "=== Processing Data ===\n";
      std::cout << std::string(60, '=') << "\n";
      std::cout << "Input: " << (inputFile.empty() ? "stdin" : inputFile) << "\n";
      std::cout << "Output: " << (outputFile.empty() ? "stdout" : outputFile) << "\n";
      std::cout << "Format: " << format << "\n";
      std::cout << "Compression: " << (compress ? "enabled" : "disabled") << "\n";

      if (compress) {
        std::cout << "  Level: " << static_cast<int>(compressionLevel) << "\n";
      }

      std::cout << "Max Size: " << (maxSize / (1024 * 1024)) << " MB\n";

      if (parallel) {
        int actualThreads = (threads > 0) ? threads : 4;
        std::cout << "Parallel Processing: enabled (" << actualThreads << " threads)\n";
      }

      std::cout << "\nProcessing data...\n";
      std::cout << "Writing output to " << format << " format...\n";
      std::cout << "Processing completed!\n";

    } else if (analyticsExecuted) {
      std::cout << std::string(60, '=') << "\n";
      std::cout << "=== Running Analytics ===\n";
      std::cout << std::string(60, '=') << "\n";
      std::cout << "Data Source: " << dataSource << "\n";
      std::cout << "Analysis Range: " << analysisRange.start << " to " << analysisRange.end << "\n";
      std::cout << "Metrics: " << metrics << "\n";
      std::cout << "Aggregation: " << aggregation << "\n";

      if (threshold > 0.0) {
        std::cout << "Threshold: " << std::fixed << std::setprecision(2) << threshold << "\n";
      }

      if (exportCsv) {
        std::cout << "CSV Export: " << (csvPath.empty() ? "results.csv" : csvPath) << "\n";
      }

      std::cout << "\nAnalyzing data...\n";
      std::cout << "Computing " << aggregation << " aggregations...\n";
      std::cout << "Analysis complete!\n";

      if (exportCsv) {
        std::cout << "Results exported to " << (csvPath.empty() ? "results.csv" : csvPath) << "\n";
      }

    } else {
      // No command - show usage
      std::cout << "Advanced CLI Application Example\n\n";
      std::cout << "Usage: " << argv[0] << " [global-options] <command> [command-options]\n\n";

      std::cout << "Global Options:\n";
      std::cout << "  -l, --log-level LEVEL    Set log level (trace|debug|info|warn|error|fatal)\n";
      std::cout << "  --log-file FILE          Write logs to file\n";
      std::cout << "  --no-color               Disable colored output\n";
      std::cout << "  --timestamp              Enable timestamps in logs\n\n";

      std::cout << "Commands:\n";
      std::cout << "  server                   Start HTTP/HTTPS server\n";
      std::cout << "  database                 Database operations\n";
      std::cout << "  process                  Process data files\n";
      std::cout << "  analytics                Run analytics on datasets\n\n";

      std::cout << "Run '" << argv[0] << " <command> --help' for command-specific options\n";
    }

  } catch (const doptions::ParseException& e) {
    std::cerr << "Parse Error: " << e.what() << "\n";
    return 1;
  } catch (const std::invalid_argument& e) {
    std::cerr << "Invalid Argument: " << e.what() << "\n";
    return 1;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}

/*
 * Usage examples:
 *
 * # Start server with SSL
 * ./complex_example --log-level debug server -p 443 --enable-ssl --workers 8
 *
 * # Database migration
 * ./complex_example database --config localhost:5432/mydb@admin --operation migrate --batch-size 5000
 *
 * # Database operation with time range
 * ./complex_example database --config db.host:3306/logs@user --time-range 1000..2000 --dry-run
 *
 * # Process data with compression
 * ./complex_example -l trace process -i data.json -o output.xml -f xml -c --compression-level 9 --parallel
 *
 * # Run analytics with CSV export
 * ./complex_example analytics --data-source metrics.db --range 0..1000 --aggregation daily --export-csv --csv-path report.csv
 *
 * # Complex combined usage
 * ./complex_example --log-file app.log --log-level warn --timestamp server -h 127.0.0.1 -p 9000 -w 16 --max-connections 5000 --timeout 60.5
 */
