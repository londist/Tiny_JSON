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

#define TEST_ROUNDTRIP(content)\
	do {\
        Value v;\
		v.parse(content);\
		std::string str;\
		v.stringify(str);\
		EXPECT_STREQ(content, str.c_str());\
	} while(0)

TEST(TestRound, HandlesStringifyNumber)
{
	TEST_ROUNDTRIP("0");
	TEST_ROUNDTRIP("-0");
	TEST_ROUNDTRIP("1");
	TEST_ROUNDTRIP("-1");
	TEST_ROUNDTRIP("1.5");
	TEST_ROUNDTRIP("-1.5");
	TEST_ROUNDTRIP("3.25");
	TEST_ROUNDTRIP("1e+20");
	TEST_ROUNDTRIP("1.234e+20");
	TEST_ROUNDTRIP("1.234e-20");

	TEST_ROUNDTRIP("1.0000000000000002"); /* the smallest number > 1 */
	TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
	TEST_ROUNDTRIP("-4.9406564584124654e-324");
	TEST_ROUNDTRIP("2.2250738585072009e-308");  /* Max subnormal double */
	TEST_ROUNDTRIP("-2.2250738585072009e-308");
	TEST_ROUNDTRIP("2.2250738585072014e-308");  /* Min normal positive double */
	TEST_ROUNDTRIP("-2.2250738585072014e-308");
	TEST_ROUNDTRIP("1.7976931348623157e+308");  /* Max double */
	TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

TEST(TestRound, HandlesStringifyString)
{
	TEST_ROUNDTRIP("\"\"");
	TEST_ROUNDTRIP("\"Hello\"");
	TEST_ROUNDTRIP("\"Hello\\nWorld\"");
	TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
	TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

TEST(TestRound, HandlesStringifyArray)
{
	TEST_ROUNDTRIP("[]");
	TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

TEST(TestRound, HandlesStringifObject)
{
	TEST_ROUNDTRIP("{}");
	TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"a\":[1,2,3],\"s\":\"abc\",\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}

TEST(TestRound, HandlesStringifLiteral)
{
	TEST_ROUNDTRIP("null");
	TEST_ROUNDTRIP("false");
	TEST_ROUNDTRIP("true");
}

TEST(TestAccess, HandlesNull)
{
	Value v;
	v.set_string("a");
	v.set_type(kType::False);
	EXPECT_EQ(kType::False, v.get_type());
}

TEST(TestAccess, HandlesTrue)
{
	Value v;
	v.set_string("a");
	v.set_type(kType::True);
	EXPECT_EQ(kType::True, v.get_type());
}

TEST(TestAccess, HandlesNumber)
{
	Value v;
	v.set_string("a");
	v.set_number(1234.5);
	EXPECT_DOUBLE_EQ(1234.5, v.get_number());
}

TEST(TestAccess, HandlesString)
{
	Value v;
	v.set_string("");
	EXPECT_STREQ("", v.get_string().c_str());
	v.set_string("Hello");
	EXPECT_STREQ("Hello", v.get_string().c_str());
}

TEST(TestAccess, HandlesArray)
{
	Value a, e;
	std::vector<Value> v;
	for (size_t j = 0; j < 5; j += 5) {
		a.set_array(v);
		EXPECT_EQ(0, a.get_array_size());
		for (int i = 0; i < 10; ++i) {
			e.set_number(i);
			a.array_push_back(e);
		}

		EXPECT_EQ(10, a.get_array_size());
		for (int i = 0; i < 10; ++i)
			EXPECT_EQ(static_cast<double>(i), a.get_array_element(i).get_number());
	}
	a.array_pop_back();
	EXPECT_EQ(9, a.get_array_size());
	for (int i = 0; i < 9; ++i)
		EXPECT_EQ(static_cast<double>(i), a.get_array_element(i).get_number());

	a.array_erase(4, 0);
	EXPECT_EQ(9, a.get_array_size());
	for (int i = 0; i < 9; ++i)
		EXPECT_EQ(static_cast<double>(i), a.get_array_element(i).get_number());

	a.array_erase(8, 1);
	EXPECT_EQ(8, a.get_array_size());
	for (int i = 0; i < 8; ++i)
		EXPECT_EQ(static_cast<double>(i), a.get_array_element(i).get_number());

	a.array_erase(0, 2);
	EXPECT_EQ(6, a.get_array_size());
	for (int i = 0; i < 6; ++i)
		EXPECT_EQ(static_cast<double>(i) + 2, a.get_array_element(i).get_number());

	for (int i = 0; i < 2; ++i) {
		e.set_number(i);
		a.array_insert(e, i);
	}

	EXPECT_EQ(8, a.get_array_size());
	for (int i = 0; i < 8; ++i) {
		EXPECT_EQ(static_cast<double>(i), a.get_array_element(i).get_number());
	}

	e.set_string("Hello");
	a.array_push_back(e);

	a.array_clear();
	EXPECT_EQ(0, a.get_array_size());
}

TEST(TestAccess, HandlesArray)
{

}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}