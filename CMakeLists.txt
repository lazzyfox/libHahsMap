cmake_minimum_required(VERSION 3.9)
project(libHashMap VERSION 0.0.1 DESCRIPTION "C++ Compile time hash map library")

option(BUILD_TESTS "Build the unit tests when BUILD_TESTING is enabled." ON)
option(BUILD_DOC "Build the unit tests when BUILD_TESTING is enabled." ON)

set(DEFAULT_BUILD_TYPE "Release")
set (CMAKE_CXX_COMPILER_VERSION 13)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER ${CXX_COMPILER})
set(CMAKE_C_COMPILER ${C_COMPILER})

include(GNUInstallDirs)


add_library(libHashMap INTERFACE)
target_compile_features(libHashMap INTERFACE cxx_std_23)


install(TARGETS libHashMap
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


target_include_directories(libHashMap INTERFACE 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("lib_Hash_Map_version.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)



install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DESTINATION include)


if (BUILD_TESTS)
  message(STATUS "Making unit tests")
  include(CTest)
  enable_testing()
  find_package(GTest CONFIG REQUIRED)
  add_subdirectory(tests)
  target_include_directories(libhashmap_tests PRIVATE tests/include)
endif (BUILD_TESTS)

if (BUILD_DOC)
  message(STATUS "Making documentation")
  find_package(Doxygen)
  if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
  else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
  endif (DOXYGEN_FOUND)
endif (BUILD_DOC)

