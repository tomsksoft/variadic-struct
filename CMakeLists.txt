cmake_minimum_required(VERSION 3.13)

project(variadic-struct
	VERSION 0.1
	DESCRIPTION "Header-only template library providing struct-like type with introspection support and useful utilities for it."
	HOMEPAGE_URL "https://github.com/tomsksoft/variadic-struct"
	LANGUAGES CXX)

#-----------------------------------------------------------------------------
# sources
#-----------------------------------------------------------------------------

include(GNUInstallDirs)

set(sources
	varstruct/field.h
	varstruct/literal.h
	varstruct/struct.h
	varstruct/traits.h)

set(build_sources ${sources})
set(install_sources ${sources})
list(TRANSFORM build_sources PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/include/")
list(TRANSFORM install_sources PREPEND "${CMAKE_INSTALL_INCLUDEDIR}/")

#-----------------------------------------------------------------------------
# target
#-----------------------------------------------------------------------------

add_library(varstruct INTERFACE)

# allows to use project in a uniform manner whether target is linked through find_package or add_subdirectory
add_library(Tomsksoft::varstruct ALIAS varstruct)

# adds library header files to IDE
target_sources(varstruct
	INTERFACE
		"$<BUILD_INTERFACE:${build_sources}>"
		"$<INSTALL_INTERFACE:${install_sources}>")

target_include_directories(varstruct
	INTERFACE
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_compile_features(varstruct
	INTERFACE
		cxx_std_17)

include(cmake/CheckFeatures.cmake)
check_string_literal_operator_template(HAVE_STRING_LITERAL_OPERATOR_TEMPLATE)

target_compile_definitions(varstruct
	INTERFACE
		VARSTRUCT_USE_STRING_LITERAL_OPERATOR_TEMPLATE=${HAVE_STRING_LITERAL_OPERATOR_TEMPLATE})

#-----------------------------------------------------------------------------
# install
#-----------------------------------------------------------------------------

include(CMakePackageConfigHelpers)

set(package_config_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(package_config_prefix ${PROJECT_NAME})

configure_package_config_file(
	cmake/${package_config_prefix}-config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/${package_config_prefix}-config.cmake"
	INSTALL_DESTINATION "${package_config_dir}"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(${package_config_prefix}-config-version.cmake
	COMPATIBILITY SameMajorVersion)

install(TARGETS varstruct EXPORT ${package_config_prefix}-targets)
install(DIRECTORY include/varstruct DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(EXPORT ${package_config_prefix}-targets
	FILE ${package_config_prefix}-targets.cmake
	DESTINATION "${package_config_dir}"
	NAMESPACE Tomsksoft::)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/${package_config_prefix}-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/${package_config_prefix}-config-version.cmake"
	DESTINATION "${package_config_dir}")

#-----------------------------------------------------------------------------
# tests
#-----------------------------------------------------------------------------

include(CTest)

if (BUILD_TESTING)
	enable_testing()
	add_subdirectory(test)
endif()
