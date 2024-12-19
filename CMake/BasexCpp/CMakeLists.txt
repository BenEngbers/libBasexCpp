cmake_minimum_required(VERSION 3.14)
project(BasexCpp
    VERSION 2.1.0
    DESCRIPTION "Template for C++ library built with CMake"
    LANGUAGES CXX)

#----------------------------------------------------------------------------------------------------------------------
# general settings and options
#----------------------------------------------------------------------------------------------------------------------

include(cmake/utils.cmake)
include(GNUInstallDirs)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)

set(BASEXCPP_SHARED_LIBS ON)
option(BASEXCPP_INSTALL "Generate target for installing BasexCpp" ${is_top_level})
set_if_undefined(BASEXCPP_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/BasexCpp" CACHE STRING
    "Install path for BasexCpp package-related CMake files")

if(DEFINED BASEXCPP_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ${BASEXCPP_SHARED_LIBS})
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set_if_undefined(CMAKE_CXX_VISIBILITY_PRESET hidden)
set_if_undefined(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

add_library(BasexCpp) # initialized below
add_library(BasexCpp::BasexCpp ALIAS BasexCpp)

#----------------------------------------------------------------------------------------------------------------------
# BasexCpp dependencies
#----------------------------------------------------------------------------------------------------------------------

include(FindPkgConfig)
pkg_check_modules(CURLPP REQUIRED curlpp)
include(FindCURL)
find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
  message(FATAL_ERROR "Could not find CURL")
else()
  target_link_libraries(${PROJECT_NAME} ${CURLPP_LDFLAGS})
endif()

#----------------------------------------------------------------------------------------------------------------------
# BasexCpp sources
#----------------------------------------------------------------------------------------------------------------------

include(GenerateExportHeader)
set(export_file_name "export_shared.h")

if(NOT BUILD_SHARED_LIBS)
    set(export_file_name "export_static.h")
endif()

# generate_export_header(BasexCpp EXPORT_FILE_NAME include/BasexCpp/${export_file_name}
generate_export_header(BasexCpp EXPORT_FILE_NAME include/${export_file_name}
    )

target_include_directories(BasexCpp PRIVATE src)

set(public_headers
#    include/BasexCpp/export.h
#    include/BasexCpp/Base.h
#    include/BasexCpp/BasexClient.h
#    include/BasexCpp/QueryObject.h
#    include/BasexCpp/ResponseObj.h
#    include/BasexCpp/BasexCpp.h

    include/BasexCpp/export.h
#    include/BasexCpp/Base.h
    include/BasexClient.h
    include/QueryObject.h
    include/ResponseObj.h
    include/BasexCpp.h
)

set(sources
    ${public_headers}
    src/Base.cpp
    src/BasexClient.cpp
    src/BasexSocket.cpp
    src/ClientUtils.cpp
    src/QueryObject.cpp
    src/ResponseObj.cpp

    src/BasexSocket.h
    )
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

#----------------------------------------------------------------------------------------------------------------------
# BasexCpp target
#----------------------------------------------------------------------------------------------------------------------

include(CMakePackageConfigHelpers)

target_sources(BasexCpp PRIVATE ${sources})
target_compile_definitions(BasexCpp PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:BASEXCPP_STATIC_DEFINE>")

target_include_directories(BasexCpp
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(BasexCpp PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
    )

if(BASEXCPP_INSTALL AND NOT CMAKE_SKIP_INSTALL_RULES)
    configure_package_config_file(cmake/BasexCpp-config.cmake.in BasexCpp-config.cmake
        INSTALL_DESTINATION "${BASEXCPP_INSTALL_CMAKEDIR}")

    write_basic_package_version_file(BasexCpp-config-version.cmake
        COMPATIBILITY SameMajorVersion)

    install(TARGETS BasexCpp EXPORT BasexCpp_export
        RUNTIME COMPONENT BasexCpp
        LIBRARY COMPONENT BasexCpp NAMELINK_COMPONENT BasexCpp-dev
        ARCHIVE COMPONENT BasexCpp-dev
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
    install(DIRECTORY include/
        TYPE INCLUDE
        COMPONENT BasexCpp-dev)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/${export_file_name}"
        COMPONENT BasexCpp-dev
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    set(targets_file "BasexCpp-shared-targets.cmake")

    if(NOT BUILD_SHARED_LIBS)
        set(targets_file "BasexCpp-static-targets.cmake")
    endif()

    install(EXPORT BasexCpp_export
        COMPONENT BasexCpp-dev
        FILE "${targets_file}"
        DESTINATION "${BASEXCPP_INSTALL_CMAKEDIR}"
        NAMESPACE BasexCpp::)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/BasexCpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BasexCpp-config-version.cmake"
        COMPONENT BasexCpp-dev
        DESTINATION "${BASEXCPP_INSTALL_CMAKEDIR}")

    if(MSVC)
        set(pdb_file "")
        set(pdb_file_destination "")

        if(BUILD_SHARED_LIBS)
            set(pdb_file "$<TARGET_PDB_FILE:BasexCpp>")
            set(pdb_file_destination "${CMAKE_INSTALL_BINDIR}")
        else()
            # TARGET_PDB_FILE does not work for pdb file generated for static library build, determining it manually
            set(pdb_file "$<TARGET_FILE_DIR:BasexCpp>/$<TARGET_FILE_PREFIX:BasexCpp>$<TARGET_FILE_BASE_NAME:BasexCpp>.pdb")
            set(pdb_file_destination "${CMAKE_INSTALL_LIBDIR}")
        endif()

        install(FILES "${pdb_file}"
            COMPONENT BasexCpp-dev
            CONFIGURATIONS Debug RelWithDebInfo
            DESTINATION "${pdb_file_destination}"
            OPTIONAL)
    endif()
endif()

#----------------------------------------------------------------------------------------------------------------------
# other targets
#----------------------------------------------------------------------------------------------------------------------

if(BASEXCPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BASEXCPP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BASEXCPP_BUILD_DOCS)
  if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    find_package(Doxygen REQUIRED)
    if (DOXYGEN_FOUND)
      set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/libBasexCpp.in)
      set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/libBasexCpp.doxyfile)

      # request to configure the file
      configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

      # note the option ALL which allows to build the docs together with the application
      add_custom_target( doc_doxygen ALL
          COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
          COMMENT "Generating API documentation with Doxygen"
          VERBATIM )
    else (DOXYGEN_FOUND)
      message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
    doxygen_add_docs(docs include)
  endif()
endif()
