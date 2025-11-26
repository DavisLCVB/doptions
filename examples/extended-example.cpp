// Extended example - Application with commands and various option types
#include <doptions/doptions.hpp>
#include <iomanip>
#include <iostream>

auto main(int argc, char** argv) -> int {
  // Create application
  auto app = doptions::Application::createApp();

  // Global options (available for all commands)
  bool debug = false;
  std::string configFile = "config.json";

  app.addOption("-d,--debug", &debug);
  app.addOption("-c,--config", &configFile);

  // Build command
  bool buildExecuted = false;
  std::string buildType = "Debug";
  int jobs = 1;
  bool clean = false;

  auto& buildCmd = app.addCommand("build", &buildExecuted);
  buildCmd->addOption("--type", &buildType);
  buildCmd->addOption("-j,--jobs", &jobs);
  buildCmd->addOption("--clean", &clean);

  // Test command
  bool testExecuted = false;
  std::string filter;
  bool coverage = false;
  int timeout = 60;

  auto& testCmd = app.addCommand("test", &testExecuted);
  testCmd->addOption("--filter", &filter);
  testCmd->addOption("--coverage", &coverage);
  testCmd->addOption("--timeout", &timeout);

  // Deploy command
  bool deployExecuted = false;
  std::string environment = "staging";
  std::string host;
  uint16_t port = 8080;
  bool dryRun = false;

  auto& deployCmd = app.addCommand("deploy", &deployExecuted);
  deployCmd->addOption("-e,--environment", &environment);
  deployCmd->addOption("-h,--host", &host);
  deployCmd->addOption("-p,--port", &port);
  deployCmd->addOption("--dry-run", &dryRun);

  try {
    // Parse command line arguments
    app.parse(argc, argv);

    // Show global settings if debug is enabled
    if (debug) {
      std::cout << "=== Debug Mode ===\n";
      std::cout << "Config file: " << configFile << "\n\n";
    }

    // Execute the appropriate command
    if (buildExecuted) {
      std::cout << "=== Building Project ===\n";
      std::cout << "Build type: " << buildType << "\n";
      std::cout << "Jobs: " << jobs << "\n";
      std::cout << "Clean build: " << (clean ? "yes" : "no") << "\n";

      if (clean) {
        std::cout << "\nCleaning build directory...\n";
      }
      std::cout << "Compiling with " << jobs << " parallel jobs...\n";
      std::cout << "Build completed successfully!\n";

    } else if (testExecuted) {
      std::cout << "=== Running Tests ===\n";
      if (!filter.empty()) {
        std::cout << "Filter: " << filter << "\n";
      }
      std::cout << "Coverage: " << (coverage ? "enabled" : "disabled") << "\n";
      std::cout << "Timeout: " << timeout << "s\n";

      std::cout << "\nRunning test suite...\n";
      if (coverage) {
        std::cout << "Collecting coverage data...\n";
      }
      std::cout << "All tests passed!\n";

    } else if (deployExecuted) {
      std::cout << "=== Deploying Application ===\n";
      std::cout << "Environment: " << environment << "\n";
      std::cout << "Host: " << (host.empty() ? "default" : host) << "\n";
      std::cout << "Port: " << port << "\n";
      std::cout << "Dry run: " << (dryRun ? "yes" : "no") << "\n";

      if (dryRun) {
        std::cout << "\n[DRY RUN] Would deploy to " << environment << "\n";
      } else {
        std::cout << "\nDeploying to " << environment << "...\n";
        std::cout << "Deployment successful!\n";
      }

    } else {
      // No command executed - show usage
      std::cout << "Usage: " << argv[0]
                << " [options] <command> [command-options]\n\n";
      std::cout << "Global Options:\n";
      std::cout << "  -d, --debug           Enable debug mode\n";
      std::cout << "  -c, --config FILE     Configuration file (default: "
                   "config.json)\n\n";
      std::cout << "Commands:\n";
      std::cout << "  build                 Build the project\n";
      std::cout << "    --type TYPE         Build type: Debug|Release "
                   "(default: Debug)\n";
      std::cout
          << "    -j, --jobs N        Number of parallel jobs (default: 1)\n";
      std::cout << "    --clean             Clean before building\n\n";
      std::cout << "  test                  Run tests\n";
      std::cout << "    --filter PATTERN    Run only tests matching pattern\n";
      std::cout << "    --coverage          Enable coverage reporting\n";
      std::cout << "    --timeout SECONDS   Test timeout (default: 60)\n\n";
      std::cout << "  deploy                Deploy application\n";
      std::cout
          << "    -e, --environment   Target environment (default: staging)\n";
      std::cout << "    -h, --host HOST     Target host\n";
      std::cout << "    -p, --port PORT     Target port (default: 8080)\n";
      std::cout
          << "    --dry-run           Simulate deployment without changes\n";
    }

  } catch (const doptions::ParseException& e) {
    std::cerr << "Error: " << e.what() << "\n";
    std::cerr << "Try '" << argv[0] << " --help' for more information.\n";
    return 1;
  } catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}

/*
 * Usage examples:
 *
 * # Build in Debug mode with 4 jobs
 * ./extended_example build --type Debug -j 4
 *
 * # Build in Release mode with clean
 * ./extended_example --debug build --type Release --clean
 *
 * # Run tests with filter and coverage
 * ./extended_example test --filter "TestSuite.*" --coverage
 *
 * # Deploy to production with custom port
 * ./extended_example deploy -e production -p 9000
 *
 * # Dry run deployment
 * ./extended_example --config prod.json deploy -e production --dry-run
 */
