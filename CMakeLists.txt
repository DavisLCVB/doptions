cmake_minimum_required(VERSION 3.30)

project(doptions VERSION 0.1.0  LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples" OFF)
  option(BUILD_TESTS "Build tests" OFF)
endif()

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(BUILD_EXAMPLES)
  set(EXAMPLES
    doptions-example
    basic-example
    extended-example
    complex-example
  )
  foreach(example ${EXAMPLES})
    add_executable(${example} examples/${example}.cpp)
    target_link_libraries(${example} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
  endforeach()
endif()

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${PROJECT_NAME}Targets FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

configure_package_config_file(
  cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
