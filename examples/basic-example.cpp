// Basic example - Simple application with a few options
#include <doptions/doptions.hpp>
#include <iostream>

auto main(int argc, char** argv) -> int {
  // Create application
  auto app = doptions::Application::createApp();

  // Define variables to store parsed values
  std::string name = "World";
  int count = 1;
  bool verbose = false;

  // Add options
  app.addOption("-n,--name", &name);
  app.addOption("-c,--count", &count);
  app.addOption("-v,--verbose", &verbose);

  try {
    // Parse command line arguments
    app.parse(argc, argv);

    // Use the parsed values
    if (verbose) {
      std::cout << "Running in verbose mode\n";
      std::cout << "Name: " << name << "\n";
      std::cout << "Count: " << count << "\n";
    }

    // Main logic
    for (int i = 0; i < count; ++i) {
      std::cout << "Hello, " << name << "!\n";
    }

  } catch (const doptions::ParseException& e) {
    std::cerr << "Error: " << e.what() << "\n";
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
 * ./basic_example
 * Output: Hello, World!
 *
 * ./basic_example --name Alice --count 3
 * Output:
 *   Hello, Alice!
 *   Hello, Alice!
 *   Hello, Alice!
 *
 * ./basic_example -n Bob -c 2 -v
 * Output:
 *   Running in verbose mode
 *   Name: Bob
 *   Count: 2
 *   Hello, Bob!
 *   Hello, Bob!
 */
