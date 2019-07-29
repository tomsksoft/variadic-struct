# downloading googletest

set(gtest_dir "${CMAKE_CURRENT_BINARY_DIR}/download/gtest")
configure_file(../cmake/CMakeLists-gtest.txt.in "${gtest_dir}/CMakeLists.txt")

execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
	RESULT_VARIABLE result
	WORKING_DIRECTORY "${gtest_dir}")

if (result)
	message(FATAL_ERROR "Configure step for googletest failed")
endif()

execute_process(COMMAND "${CMAKE_COMMAND}" --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY "${gtest_dir}")

if (result)
	message(FATAL_ERROR "Build step for googletest failed")
endif()

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest to our build (this defines gtest and gtest_main targets)
add_subdirectory("${gtest_dir}/src"
                 "${gtest_dir}/build"
                 EXCLUDE_FROM_ALL)

# adding tests
