cmake_minimum_required(VERSION 3.12)
project(SIT CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the library target
add_library(SIT INTERFACE)
target_include_directories(SIT INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sit>
    $<INSTALL_INTERFACE:sit>
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/SITConfigVersion.cmake"
    VERSION $ENV{SIT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(TARGETS SIT
    EXPORT SITTargets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/SITConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/SITConfig.cmake"
    INSTALL_DESTINATION lib/cmake/SIT
)

install(EXPORT SITTargets DESTINATION lib/cmake/SIT)
install(FILES "${PROJECT_BINARY_DIR}/SITConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/SITConfig.cmake"
        DESTINATION lib/cmake/SIT)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/sit/ DESTINATION /usr/local/include/sst/sit)
