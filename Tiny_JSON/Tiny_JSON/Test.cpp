#include "gtest/gtest.h"
#include "Value.h"
#include "Parser.h"

//literal
#define TEST_LITERAL(expect, content)\
	do {\
        Value v;\
		v.parse(content);\
		EXPECT_EQ(expect, v.get_type());\
	} while (0)

TEST(TestParse, HandlesLiteral)
{
    TEST_LITERAL(kType::Null, "null");
    TEST_LITERAL(kType::True, "true");
    TEST_LITERAL(kType::False, "false");
}

//number
#define TEST_NUMBER(expect, content)\
	do {\
        Value v;\
		v.parse(content);\
		EXPECT_EQ(kType::Number, v.get_type());\
		EXPECT_DOUBLE_EQ(expect, v.get_number());\
	} while(0)

TEST(TestParse, HandlesNumber)
{
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	TEST_NUMBER(0.0, "1e-10000");
}

//string
#define TEST_STRING(expect, content)\
	do {\
        Value v;\
		v.parse(content);\
		EXPECT_EQ(kType::String, v.get_type());\
		EXPECT_EQ(0, memcmp(expect, v.get_string().c_str(), v.get_string().size()));\
	}while (0)

TEST(TestParse, HandlesString)
{
	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");
	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ \b \f \n \r \t", "\"\\\" \\\\ \\b \\f \\n \\r \\t\"");
	TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
	TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
	TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
	TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}

//array
TEST(TestParse, HandlesArray)
{
	Value v;
	v.parse("[]");
	EXPECT_EQ(kType::Array, v.get_type());
	EXPECT_EQ(0, v.get_array_size());
	
	v.parse("[ null , false , true , 123 , \"abc\" ]");
	EXPECT_EQ(kType::Array, v.get_type());
	EXPECT_EQ(5, v.get_array_size());
	EXPECT_EQ(kType::Null, v.get_array_element(0).get_type());
	EXPECT_EQ(kType::False, v.get_array_element(1).get_type());
	EXPECT_EQ(kType::True, v.get_array_element(2).get_type());
	EXPECT_EQ(kType::Number, v.get_array_element(3).get_type());
	EXPECT_EQ(kType::String, v.get_array_element(4).get_type());
	EXPECT_DOUBLE_EQ(123.0, v.get_array_element(3).get_number());
	EXPECT_STREQ("abc", v.get_array_element(4).get_string().c_str());

	v.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]");
	EXPECT_EQ(kType::Array, v.get_type());
	EXPECT_EQ(4, v.get_array_size());
	for (int i = 0; i < 4; ++i) {
		Value a = v.get_array_element(i);
		EXPECT_EQ(kType::Array, a.get_type());
		EXPECT_EQ(i, a.get_array_size());
		for (int j = 0; j < i; ++j) {
			Value e = a.get_array_element(j);
			EXPECT_EQ(kType::Number, e.get_type());
			EXPECT_EQ((double)j, e.get_number());

		}
	}
	
}

//object
TEST(TestParse, HandlesObject)
{
	Value v;
	v.parse("{}");
	EXPECT_EQ(kType::Object, v.get_type());

	v.parse(" { "
			"\"n\" : null , "
			"\"f\" : false , "
			"\"t\" : true , "
			"\"i\" : 123 , "
			"\"s\" : \"abc\", "
			"\"a\" : [ 1, 2, 3 ],"
			"\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
			" } ");
	EXPECT_EQ(kType::Object, v.get_type());
	EXPECT_EQ(kType::Null, v.get_object_value("n").get_type());
	EXPECT_EQ(kType::False, v.get_object_value("f").get_type());
	EXPECT_EQ(kType::True, v.get_object_value("t").get_type());
	EXPECT_EQ(kType::Number, v.get_object_value("i").get_type());
	EXPECT_EQ(kType::String, v.get_object_value("s").get_type());
	EXPECT_EQ(kType::Array, v.get_object_value("a").get_type());
	EXPECT_EQ(kType::Object, v.get_object_value("o").get_type());

	for (int i = 0; i < 3; ++i) {
		Value e = v.get_object_value("a").get_array_element(i);
		EXPECT_EQ(kType::Number, e.get_type());
		EXPECT_DOUBLE_EQ(i + 1.0, e.get_number());
	}

	Value o = v.get_object_value("o");
	EXPECT_EQ(kType::Object, o.get_type());
	for (auto& member : o.get_object()) {
		Value ov = member.second;
		EXPECT_EQ(kType::Number, ov.get_type());
	}
}
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}