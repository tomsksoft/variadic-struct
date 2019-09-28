#include "types.h"

auto foo() { return ChildStruct{}.set<"field0"_>(0xFFFFFFFF); }
