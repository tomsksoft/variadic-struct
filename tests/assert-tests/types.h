#include <varstruct/struct.h>

using namespace varstruct;
using namespace varstruct::literal;

struct UserType { };
using BaseStruct = Varstruct<Field<F_NAME("field0"), short>;
using ChildStruct = Varstruct<BaseStruct, Field<F_NAME("field1"), int>>;
