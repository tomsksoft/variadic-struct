#include "types.h"

auto foo() { return ChildStruct{}.value<"unknown"_>(); }
