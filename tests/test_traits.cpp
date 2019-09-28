#include "varstruct/struct.h"
#include "utils/compiler.h"

#include <gtest/gtest.h>

#include <string>
#include <type_traits>

namespace varstruct { namespace test {

	class TraitsTest: public ::testing::Test {
	protected:
		struct UserType { };
		struct SimpleDescriptor { };

		using BaseStruct = Varstruct<
			Field<F_NAME("field0"), int, SimpleDescriptor>>;

		using ChildStruct = Varstruct<
			BaseStruct,
			Field<F_NAME("field1"), std::string>>;

		using FinalStruct = Varstruct<
			ChildStruct,
			Field<F_NAME("field2"), const volatile UserType**, SimpleDescriptor>>;
	};

	TEST_F(TraitsTest, Is_Struct_Trait)
	{
		bool emptyIsStruct = is_struct_v<EmptyStruct>;
		bool baseIsStruct = is_struct_v<BaseStruct>;
		bool childIsStruct = is_struct_v<ChildStruct>;
		bool intIsStruct = is_struct_v<int>;
		bool customIsStruct = is_struct_v<UserType>;
		bool pointerIsStruct = is_struct_v<std::string*>;

		EXPECT_TRUE(emptyIsStruct);
		EXPECT_TRUE(baseIsStruct);
		EXPECT_TRUE(childIsStruct);
		EXPECT_FALSE(intIsStruct);
		EXPECT_FALSE(customIsStruct);
		EXPECT_FALSE(pointerIsStruct);
	}

	TEST_F(TraitsTest, Size_Trait)
	{
		auto emptySize = size_v<EmptyStruct>;
		auto baseSize = size_v<BaseStruct>;
		auto childSize = size_v<ChildStruct>;
		auto finalSize = size_v<FinalStruct>;

		EXPECT_EQ(emptySize, 0);
		EXPECT_EQ(baseSize, 1);
		EXPECT_EQ(childSize, 2);
		EXPECT_EQ(finalSize, 3);
	}

	TEST_F(TraitsTest, Tuple_Size_Trait)
	{
		auto emptySize = std::tuple_size_v<EmptyStruct>;
		auto finalSize = std::tuple_size_v<FinalStruct>;

		EXPECT_EQ(emptySize, 0);
		EXPECT_EQ(finalSize, 3);
	}

	TEST_F(TraitsTest, Parent_Trait)
	{
		bool baseIsChildParent = std::is_same_v<parent_t<ChildStruct>, BaseStruct>;
		bool childIsFinalParent = std::is_same_v<parent_t<FinalStruct>, ChildStruct>;
		bool baseIsFinalParentParent = std::is_same_v<parent_t<parent_t<FinalStruct>>, BaseStruct>;

		EXPECT_TRUE(baseIsChildParent);
		EXPECT_TRUE(childIsFinalParent);
		EXPECT_TRUE(baseIsFinalParentParent);
	}

	TEST_F(TraitsTest, Has_Field_Trait)
	{
		bool emptyHasField0 = has_field_v<F_NAME("field0"), EmptyStruct>;
		bool finalHasField0 = has_field_v<F_NAME("field0"), FinalStruct>;
		bool finalHasField1 = has_field_v<F_NAME("field1"), FinalStruct>;
		bool finalHasField2 = has_field_v<F_NAME("field2"), FinalStruct>;
		bool finalHasField3 = has_field_v<F_NAME("field3"), FinalStruct>;
		bool childHasField2 = has_field_v<F_NAME("field2"), ChildStruct>;

		EXPECT_FALSE(emptyHasField0);
		EXPECT_TRUE(finalHasField0);
		EXPECT_TRUE(finalHasField1);
		EXPECT_TRUE(finalHasField2);
		EXPECT_FALSE(finalHasField3);
		EXPECT_FALSE(childHasField2);
	}

	TEST_F(TraitsTest, Get_Field_Trait)
	{
		bool isFinalField1_Traits = std::is_same_v<get_field_t<1, FinalStruct>,
		                                           FieldTraits<F_NAME("field1"), std::string, NullDescriptor>>;
		auto finalField1_Name = field_name_v<1, FinalStruct>;
		auto finalField1_Id = field_id_v<1, FinalStruct>;
		bool isFinalField2_Type = std::is_same_v<field_type_t<2, FinalStruct>, const volatile UserType**>;
		bool isChildField1_Descriptor = std::is_same_v<field_descriptor_t<1, ChildStruct>, NullDescriptor>;

		EXPECT_TRUE(isFinalField1_Traits);
		EXPECT_STREQ(finalField1_Name, "field1");
		EXPECT_EQ(FnvHash(finalField1_Name), finalField1_Id);
		EXPECT_TRUE(isFinalField2_Type);
		EXPECT_TRUE(isChildField1_Descriptor);
	}

	TEST_F(TraitsTest, Tuple_Element_Trait)
	{
		bool isFinalField2_Type = std::is_same_v<std::tuple_element_t<2, FinalStruct>, const volatile UserType**>;
		EXPECT_TRUE(isFinalField2_Type);
	}

	TEST_F(TraitsTest, Find_Field_Trait)
	{
		bool isBaseField0_Traits = std::is_same_v<find_field_t<F_NAME("field0"), BaseStruct>,
		                                          FieldTraits<F_NAME("field0"), int, SimpleDescriptor>>;
		bool isChildField0_Traits = std::is_same_v<find_field_t<F_NAME("field0"), ChildStruct>,
		                                           FieldTraits<F_NAME("field0"), int, SimpleDescriptor>>;

		EXPECT_TRUE(isBaseField0_Traits);
		EXPECT_TRUE(isChildField0_Traits);
	}
}}
