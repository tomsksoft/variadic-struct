#include <varstruct/struct.h>
#include <gtest/gtest.h>

#include <string>
#include <tuple>

using namespace varstruct;

class TraitsTest: public ::testing::Test {
protected:
	struct UserType { };
	struct SimpleDescriptor { };

	using RootStruct = Varstruct<Field<F_NAME("field0"), int, SimpleDescriptor>>;
	using NodeStruct = Varstruct<RootStruct, Field<F_NAME("field1"), std::string>>;
	using LeafStruct = Varstruct<NodeStruct, Field<F_NAME("field2"), const volatile UserType**, SimpleDescriptor>>;
};

TEST_F(TraitsTest, Is_Struct_Trait)
{
	bool emptyIsStruct = is_struct_v<EmptyStruct>;
	bool rootIsStruct = is_struct_v<RootStruct>;
	bool nodeIsStruct = is_struct_v<NodeStruct>;
	bool intIsStruct = is_struct_v<int>;
	bool customIsStruct = is_struct_v<const UserType>;
	bool pointerIsStruct = is_struct_v<std::string*>;
	bool refIsStruct = is_struct_v<char&>;

	EXPECT_TRUE(emptyIsStruct);
	EXPECT_TRUE(rootIsStruct);
	EXPECT_TRUE(nodeIsStruct);
	EXPECT_FALSE(intIsStruct);
	EXPECT_FALSE(customIsStruct);
	EXPECT_FALSE(pointerIsStruct);
	EXPECT_FALSE(refIsStruct);
}

TEST_F(TraitsTest, Size_Trait)
{
	auto emptySize = size_v<EmptyStruct>;
	auto rootSize = size_v<RootStruct>;
	auto nodeSize = size_v<NodeStruct>;
	auto leafSize = size_v<LeafStruct>;

	EXPECT_EQ(emptySize, 0);
	EXPECT_EQ(rootSize, 1);
	EXPECT_EQ(nodeSize, 2);
	EXPECT_EQ(leafSize, 3);
}

TEST_F(TraitsTest, Tuple_Size_Trait)
{
	auto emptySize = std::tuple_size_v<EmptyStruct>;
	auto leafSize = std::tuple_size_v<LeafStruct>;

	EXPECT_EQ(emptySize, 0);
	EXPECT_EQ(leafSize, 3);
}

TEST_F(TraitsTest, Parent_Trait)
{
	bool rootIsNodeParent = std::is_same_v<parent_t<NodeStruct>, RootStruct>;
	bool nodeIsLeafParent = std::is_same_v<parent_t<LeafStruct>, NodeStruct>;
	bool rootIsLeafParentParent = std::is_same_v<parent_t<parent_t<LeafStruct>>, RootStruct>;

	EXPECT_TRUE(rootIsNodeParent);
	EXPECT_TRUE(nodeIsLeafParent);
	EXPECT_TRUE(rootIsLeafParentParent);
}

TEST_F(TraitsTest, Has_Field_Trait)
{
	bool emptyHasField0 = has_field_v<F_NAME("field0"), EmptyStruct>;
	bool leafHasField0 = has_field_v<F_NAME("field0"), LeafStruct>;
	bool leafHasField1 = has_field_v<F_NAME("field1"), LeafStruct>;
	bool leafHasField2 = has_field_v<F_NAME("field2"), LeafStruct>;
	bool leafHasField3 = has_field_v<F_NAME("field3"), LeafStruct>;
	bool nodeHasField2 = has_field_v<F_NAME("field2"), NodeStruct>;

	EXPECT_FALSE(emptyHasField0);
	EXPECT_TRUE(leafHasField0);
	EXPECT_TRUE(leafHasField1);
	EXPECT_TRUE(leafHasField2);
	EXPECT_FALSE(leafHasField3);
	EXPECT_FALSE(nodeHasField2);
}

TEST_F(TraitsTest, Get_Field_Trait)
{
	bool isLeafField1_Traits = std::is_same_v<get_field_t<1, LeafStruct>,
	                                          FieldTraits<F_NAME("field1"), std::string, NullDescriptor>>;
	auto leafField1_Name = field_name_v<1, LeafStruct>;
	auto leafField1_Id = field_id_v<1, LeafStruct>;
	bool isLeafField2_Type = std::is_same_v<field_type_t<2, LeafStruct>, const volatile UserType**>;
	bool isNodeField1_Descriptor = std::is_same_v<field_descriptor_t<1, NodeStruct>, NullDescriptor>;

	EXPECT_TRUE(isLeafField1_Traits);
	EXPECT_STREQ(leafField1_Name, "field1");
	EXPECT_EQ(FnvHash(leafField1_Name), leafField1_Id);
	EXPECT_TRUE(isLeafField2_Type);
	EXPECT_TRUE(isNodeField1_Descriptor);
}

TEST_F(TraitsTest, Tuple_Element_Trait)
{
	bool isLeafField2_Type = std::is_same_v<std::tuple_element_t<2, LeafStruct>, const volatile UserType**>;
	EXPECT_TRUE(isLeafField2_Type);
}

TEST_F(TraitsTest, Find_Field_Trait)
{
	bool isRootField0_Traits = std::is_same_v<find_field_t<F_NAME("field0"), RootStruct>,
	                                          FieldTraits<F_NAME("field0"), int, SimpleDescriptor>>;
	bool isNodeField0_Traits = std::is_same_v<find_field_t<F_NAME("field0"), NodeStruct>,
	                                          FieldTraits<F_NAME("field0"), int, SimpleDescriptor>>;

	EXPECT_TRUE(isRootField0_Traits);
	EXPECT_TRUE(isNodeField0_Traits);
}
