#include "types.h"

auto foo() { return ChildStruct{}.isSet<"unknown"_>(); }
