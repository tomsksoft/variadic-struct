#include "varstruct/struct.h"
#include "utils/compiler.h"

#include <gtest/gtest.h>

#include <ostream>
#include <string>

namespace varstruct { namespace test {

	using namespace varstruct::literal;

	class VarstructTest: public ::testing::Test {
	protected:
		struct ValueWithDefault {
			explicit ValueWithDefault(int value = -1): value_(value)
			{ }

			bool operator==(const ValueWithDefault& rhs) const
			{
				return value_ == rhs.value_;
			}

			friend std::ostream& operator<<(std::ostream& os, const ValueWithDefault& val)
			{
				os << val.value_;
			}

			int value_;
		};

		struct MoveInfo {
			MoveInfo(): wasMoved_(false)
			{ }

			MoveInfo(const MoveInfo&): wasMoved_(false)
			{ }

			MoveInfo(MoveInfo&&): wasMoved_(true)
			{ }

			MoveInfo& operator=(const MoveInfo&)
			{
				wasMoved_ = false;
				return *this;
			}

			MoveInfo& operator=(MoveInfo&&)
			{
				wasMoved_ = true;
				return *this;
			}

			bool wasMoved_ ;
		};

		using MoveStruct =
			Varstruct<
				Varstruct<Field<F_NAME("field0"), MoveInfo>,
				          Field<F_NAME("field1"), MoveInfo>>,
				Field<F_NAME("field2"), MoveInfo>,
				Field<F_NAME("field3"), MoveInfo>>;

		using BaseStruct =
			Varstruct<
				Field<F_NAME("field0"), ValueWithDefault>,
				Field<F_NAME("field1"), unsigned short>>;

		using ChildStruct =
			Varstruct<
				BaseStruct,
				Field<F_NAME("field2"), std::string>>;
	};

	TEST_F(VarstructTest, All_Base_Fields_Unset_In_Empty_Constructor)
	{
		BaseStruct base;
		bool field0_IsSet = base.isSet<"field0"_>();
		bool field1_IsSet = base.isSet<"field1"_>();

		EXPECT_FALSE(field0_IsSet);
		EXPECT_FALSE(field1_IsSet);
	}

	TEST_F(VarstructTest, All_Child_Fields_Unset_In_Empty_Constructor)
	{
		ChildStruct child;
		bool field0_IsSet = child.isSet<"field0"_>();
		bool field1_IsSet = child.isSet<"field1"_>();
		bool field2_IsSet = child.isSet<"field2"_>();

		EXPECT_FALSE(field0_IsSet);
		EXPECT_FALSE(field1_IsSet);
		EXPECT_FALSE(field2_IsSet);
	}

	TEST_F(VarstructTest, All_Base_Fields_Set_When_Default_Initialized)
	{
		BaseStruct base(Default_Initialized);
		bool field0_IsSet = base.isSet<"field0"_>();
		bool field1_IsSet = base.isSet<"field1"_>();
		auto field0_Value = base.value<"field0"_>();
		auto field1_Value = base.value<"field1"_>();

		EXPECT_TRUE(field0_IsSet);
		EXPECT_TRUE(field1_IsSet);
		EXPECT_EQ(field0_Value, decltype(field0_Value){});
		EXPECT_EQ(field1_Value, decltype(field1_Value){});
	}

	TEST_F(VarstructTest, All_Child_Fields_Set_When_Default_Initialized)
	{
		ChildStruct child(Default_Initialized);
		bool field0_IsSet = child.isSet<"field0"_>();
		bool field1_IsSet = child.isSet<"field1"_>();
		bool field2_IsSet = child.isSet<"field2"_>();
		auto field0_Value = child.value<"field0"_>();
		auto field1_Value = child.value<"field1"_>();
		auto field2_Value = child.value<"field2"_>();

		EXPECT_TRUE(field0_IsSet);
		EXPECT_TRUE(field1_IsSet);
		EXPECT_TRUE(field2_IsSet);
		EXPECT_EQ(field0_Value, decltype(field0_Value){});
		EXPECT_EQ(field1_Value, decltype(field1_Value){});
		EXPECT_EQ(field2_Value, decltype(field2_Value){});
	}

	TEST_F(VarstructTest, Only_Initialized_Base_Fields_Set_In_Partial_Initialization)
	{
		const BaseStruct base(ValueWithDefault(1));
		bool field0_IsSet = base.isSet<"field0"_>();
		bool field1_IsSet = base.isSet<"field1"_>();
		auto field0_Value = base.value<"field0"_>();

		EXPECT_TRUE(field0_IsSet);
		EXPECT_FALSE(field1_IsSet);
		EXPECT_EQ(field0_Value, ValueWithDefault(1));
	}

	TEST_F(VarstructTest, Only_Initialized_Child_Fields_Set_In_Partial_Initialization)
	{
		const ChildStruct child(Default_Value, Null_Value, "hello");
		bool field0_IsSet = child.isSet<"field0"_>();
		bool field1_IsSet = child.isSet<"field1"_>();
		bool field2_IsSet = child.isSet<"field2"_>();
		auto field0_Value = child.value<"field0"_>();
		auto field2_Value = child.value<"field2"_>();

		EXPECT_TRUE(field0_IsSet);
		EXPECT_FALSE(field1_IsSet);
		EXPECT_TRUE(field2_IsSet);
		EXPECT_EQ(field0_Value, decltype(field0_Value){});
		EXPECT_STREQ(field2_Value.c_str(), "hello");
	}

	TEST_F(VarstructTest, Rvalues_Moved_In_Constructor)
	{
		MoveInfo notMoved;
		MoveStruct mv(MoveInfo{}, notMoved, MoveInfo{}, notMoved);
		bool field0_IsMoved = mv.value<"field0"_>().wasMoved_;
		bool field1_IsMoved = mv.value<"field1"_>().wasMoved_;
		bool field2_IsMoved = mv.value<"field2"_>().wasMoved_;
		bool field3_IsMoved = mv.value<"field3"_>().wasMoved_;

		EXPECT_TRUE(field0_IsMoved);
		EXPECT_FALSE(field1_IsMoved);
		EXPECT_TRUE(field2_IsMoved);
		EXPECT_FALSE(field3_IsMoved);
	}

	TEST_F(VarstructTest, Fields_Moved_In_Move_Constructor)
	{
		MoveStruct from;
		MoveStruct to(std::move(from));
		bool field0_IsMoved = to.value<"field0"_>().wasMoved_;
		bool field1_IsMoved = to.value<"field1"_>().wasMoved_;
		bool field2_IsMoved = to.value<"field2"_>().wasMoved_;
		bool field3_IsMoved = to.value<"field3"_>().wasMoved_;

		EXPECT_TRUE(field0_IsMoved);
		EXPECT_TRUE(field1_IsMoved);
		EXPECT_TRUE(field2_IsMoved);
		EXPECT_TRUE(field3_IsMoved);
	}

	TEST_F(VarstructTest, Fields_Moved_In_Move_Assignment)
	{
		MoveStruct from, to;
		to = std::move(from);
		bool field0_IsMoved = to.value<"field0"_>().wasMoved_;
		bool field1_IsMoved = to.value<"field1"_>().wasMoved_;
		bool field2_IsMoved = to.value<"field2"_>().wasMoved_;
		bool field3_IsMoved = to.value<"field3"_>().wasMoved_;

		EXPECT_TRUE(field0_IsMoved);
		EXPECT_TRUE(field1_IsMoved);
		EXPECT_TRUE(field2_IsMoved);
		EXPECT_TRUE(field3_IsMoved);
	}

	TEST_F(VarstructTest, Fields_Moved_From_Rvalue_Varstruct)
	{
		MoveInfo movedFromBase(MoveStruct{}.value<"field0"_>());
		MoveInfo movedFromChild(MoveStruct{}.value<"field3"_>());

		EXPECT_TRUE(movedFromBase.wasMoved_);
		EXPECT_TRUE(movedFromChild.wasMoved_);
	}

	TEST_F(VarstructTest, Fields_Not_Initialized_In_Value_Method)
	{
		BaseStruct base;
		base.value<"field1"_>() = 1;
		bool field1_isSet = base.isSet<"field1"_>();
		auto field1_Value =  base.value<"field1"_>();

		EXPECT_FALSE(field1_isSet);
		EXPECT_EQ(field1_Value, 1);
	}

	TEST_F(VarstructTest, Fields_Initialized_In_Set_Method)
	{
		ChildStruct child;
		child.set<"field1"_>(1);
		child.set<"field2"_>("hello");
		bool field1_IsSet = child.isSet<"field1"_>();
		bool field2_IsSet = child.isSet<"field2"_>();
		auto field1_Value = child.value<"field1"_>();
		auto field2_Value = child.value<"field2"_>();

		EXPECT_TRUE(field1_IsSet);
		EXPECT_TRUE(field2_IsSet);
		EXPECT_EQ(field1_Value, 1);
		EXPECT_STREQ(field2_Value.c_str(), "hello");
	}

	TEST_F(VarstructTest, Set_Method_Checks_For_Integer_Overflow)
	{
		BaseStruct base;

		bool setField1_ToLargeValueResult = base.set<"field1"_>(100000);
		bool field1_IsSet = base.isSet<"field1"_>();

		EXPECT_FALSE(setField1_ToLargeValueResult);
		EXPECT_FALSE(field1_IsSet);

		base.isSet<"field1"_>() = true;
		bool setField1_ToNegativeValueResult = base.set<"field1"_>(static_cast<short>(-1));
		field1_IsSet = base.isSet<"field1"_>();

		EXPECT_FALSE(setField1_ToNegativeValueResult);
		EXPECT_FALSE(field1_IsSet);
	}

	TEST_F(VarstructTest, Structured_Binding_Works_For_Varstruct)
	{
		ChildStruct child(Default_Value, 1001, "hello");
		const auto& [f1, f2, f3] = child;

		EXPECT_EQ(f1, ValueWithDefault{});
		EXPECT_EQ(f2, 1001);
		EXPECT_STREQ(f3.c_str(), "hello");
	}
}}
