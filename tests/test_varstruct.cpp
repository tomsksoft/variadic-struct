#include <varstruct/struct.h>
#include <gtest/gtest.h>

#include <array>
#include <ostream>
#include <string>
#include <tuple>
#include <vector>

using namespace varstruct;

class VarstructTest: public ::testing::Test {
protected:
	struct ValueWithDefault {
		ValueWithDefault(int value = 1001): value_(value)
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
			Varstruct<Field<F_NAME("field0"), MoveInfo>>,
			Field<F_NAME("field1"), MoveInfo>>;

	using RootStruct =
		Varstruct<
			Field<F_NAME("field0"), ValueWithDefault>,
			Field<F_NAME("field1"), short>>;

	using LeafStruct =
		Varstruct<
			RootStruct,
			Field<F_NAME("field2"), std::vector<std::array<int, 3>>>>;

protected:
	void SetUp() override
	{
		cleanRoot_ = RootStruct{};
		defaultRoot_ = RootStruct(Default_Initialized);
		partialRoot_ = RootStruct{ ValueWithDefault(1002) };

		cleanLeaf_ = LeafStruct{};
		defaultLeaf_ = LeafStruct{Default_Initialized};
		partialLeaf_ = LeafStruct{ ValueWithDefault(1003) };

		MoveStruct original{ MoveInfo{}, MoveInfo{} };
		copiedFrom_ = original;
		movedFrom_ = std::move(original);
	}

protected:
	RootStruct cleanRoot_;
	RootStruct defaultRoot_;
	RootStruct partialRoot_;

	LeafStruct cleanLeaf_;
	LeafStruct defaultLeaf_;
	LeafStruct partialLeaf_;

	MoveStruct copiedFrom_;
	MoveStruct movedFrom_;
};

TEST_F(VarstructTest, All_Fields_Unset_In_Empty_Constructor)
{
	bool rootField0_IsSet = cleanRoot_.isSet<"field0"_>();
	bool rootField1_IsSet = cleanRoot_.isSet<"field1"_>();

	EXPECT_FALSE(rootField0_IsSet);
	EXPECT_FALSE(rootField1_IsSet);

	bool leafField0_IsSet = cleanLeaf_.isSet<"field0"_>();
	bool leafField1_IsSet = cleanLeaf_.isSet<"field1"_>();
	bool leafField2_IsSet = cleanLeaf_.isSet<"field2"_>();

	EXPECT_FALSE(leafField0_IsSet);
	EXPECT_FALSE(leafField1_IsSet);
	EXPECT_FALSE(leafField2_IsSet);
}

TEST_F(VarstructTest, All_Fields_Set_When_Default_Initialized)
{
	bool rootField0_IsSet = defaultRoot_.isSet<"field0"_>();
	bool rootField1_IsSet = defaultRoot_.isSet<"field1"_>();
	auto rootField0_Value = defaultRoot_.value<"field0"_>();

	EXPECT_TRUE(rootField0_IsSet);
	EXPECT_TRUE(rootField1_IsSet);
	EXPECT_EQ(rootField0_Value, ValueWithDefault{});

	bool leafField0_IsSet = defaultLeaf_.isSet<"field0"_>();
	bool leafField1_IsSet = defaultLeaf_.isSet<"field1"_>();
	bool leafField2_IsSet = defaultLeaf_.isSet<"field2"_>();
	auto leafField0_Value = defaultLeaf_.value<"field0"_>();

	EXPECT_TRUE(leafField0_IsSet);
	EXPECT_TRUE(leafField1_IsSet);
	EXPECT_TRUE(leafField2_IsSet);
	EXPECT_EQ(leafField0_Value, ValueWithDefault{});
}
/*
TEST_F(VarstructTest, Only_Initialized_Fields_Set_In_Partial_Initialization)
{
	bool rootField1_IsSet = partialRoot_.isSet<"field1"_>();
	bool rootField2_IsSet = partialRoot_.isSet<"field2"_>();
	bool rootField1_IsHello = defaultRoot_.value<"field1"_>() == "hello";

	EXPECT_TRUE(rootField1_IsSet)
	EXPECT_FALSE(rootField2_IsSet);
	EXPECT_TRUE(rootField1_IsHello);

	bool leafField1_IsSet = defaultLeaf_.isSet<"field1"_>();
	bool leafField2_IsSet = defaultLeaf_.isSet<"field2"_>();
	bool leafField3_IsSet = defaultLeaf_.isSet<"field3"_>();
	bool leafField1_IsDefault = defaultLeaf_.value<"field1"_>() == field_type_t<F_NAME("field1")>{};
	bool leafField2_IsDefault = defaultLeaf_.value<"field2"_>() == field_type_t<F_NAME("field2")>{};
	bool leafField3_IsDefault = defaultLeaf_.value<"field3"_>() == field_type_t<F_NAME("field3")>{};

	EXPECT_TRUE(leafField1_IsSet);
	EXPECT_TRUE(leafField2_IsSet);
	EXPECT_TRUE(leafField3_IsSet);
	EXPECT_TRUE(leafField1_IsDefault);
	EXPECT_TRUE(leafField2_IsDefault);
	EXPECT_TRUE(leafField3_IsDefault);

}

TEST_F(VarstructTest, RvalueIsMovedInConstructor)
{
	MoveInfo copied;
	MoveStruct mv{ MoveInfo{}, copied };
}*/
