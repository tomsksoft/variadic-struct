#include "utils/compiler.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iterator>
#include <string>
#include <utility>

namespace varstruct { namespace test {

	namespace {

		std::pair<bool, std::string> CompileTarget(const char* targetName)
		{
			char errorFileName[512];
			std::snprintf(errorFileName, sizeof(errorFileName), "%s/%s-error.txt",
				COMPILE_TEST_RESULT_DIR,
				targetName);

			char compileCmd[2048];
			std::snprintf(compileCmd, sizeof(compileCmd), "%s %s 1>\"%s/%s-out.txt\" 2>\"%s\"",
				COMPILE_COMMAND_PREFIX,
				targetName,
				COMPILE_TEST_RESULT_DIR,
				targetName,
				errorFileName);

			std::pair<bool, std::string> result = { true, std::string{} };
			int cmdResult = std::system(compileCmd);

			if (cmdResult != 0) {
				result.first = false;
				std::ifstream errorFile(errorFileName);

				if (errorFile) {
					errorFile.seekg(0, std::ios::end);
					result.second.reserve(errorFile.tellg());
					errorFile.seekg(0, std::ios::beg);

					result.second.assign(std::istreambuf_iterator<char>(errorFile), std::istreambuf_iterator<char>());
				}
			}

			return result;
		}
	}

	bool Compiles(const char* targetName)
	{
		return CompileTarget(targetName).first;
	}

	bool FailsToCompile(const char* targetName, const char* expectedAssert)
	{
		auto [result, error] = CompileTarget(targetName);

		if (!result && expectedAssert) {
			return error.find(expectedAssert) != std::string::npos;
		}

		return result;
	}
}}
