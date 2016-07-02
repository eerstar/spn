#include "Json.h"
#include "UnitTest.h"

UNIT_TEST(Json, Value)
{
	Json j;
	const Json& cj = j;

	UNIT_ASSERT_EQUAL(cj.AsInt32(), 0);
	UNIT_ASSERT_EQUAL(cj.AsString(), "");

	j = 12345;
	UNIT_ASSERT_EQUAL(cj.AsInt32(), 12345);
	UNIT_ASSERT_EQUAL(cj.AsString(), "12345");

	j = "123\t\"456abc";
	UNIT_ASSERT_EQUAL(cj.AsInt32(), 123);
	UNIT_ASSERT_EQUAL(cj.AsString(), "123\t\"456abc");

	j = "+23";
	UNIT_ASSERT_EQUAL(cj.AsInt32(), 23);
	UNIT_ASSERT_EQUAL(cj.AsString(), "+23");

	j = "-34";
	UNIT_ASSERT_EQUAL(cj.AsInt32(), -34);
	UNIT_ASSERT_EQUAL(cj.AsString(), "-34");

	j = "3.14159";
	UNIT_ASSERT_EQUAL(cj.AsFloat(), 3.14159);
	UNIT_ASSERT_EQUAL(cj.AsString(), "3.14159");

	j = "-2.718281828";
	UNIT_ASSERT_EQUAL(cj.AsFloat(), -2.718281828);
	UNIT_ASSERT_EQUAL(cj.AsString(), "-2.718281828");

	j = "1\"2'3\\4/5\b6\f7\n8\r9\t0";
	UNIT_ASSERT_EQUAL(cj.AsString(), "1\"2'3\\4/5\b6\f7\n8\r9\t0");

	j.Clear();
	UNIT_ASSERT_EQUAL(cj.AsInt32(), 0);
	UNIT_ASSERT_EQUAL(cj.AsString(), "");
}

UNIT_TEST(Json, Generation)
{
	Json j;
	const Json& cj = j;

	UNIT_ASSERT_EQUAL(cj.Dump(), "null");

	j = true;
	UNIT_ASSERT_EQUAL(cj.Dump(), "true");

	j = false;
	UNIT_ASSERT_EQUAL(cj.Dump(), "false");

	j = 0;
	UNIT_ASSERT_EQUAL(cj.Dump(), "0");

	j = 123;
	UNIT_ASSERT_EQUAL(cj.Dump(), "123");

	j = -456;
	UNIT_ASSERT_EQUAL(cj.Dump(), "-456");

	j = "abcd";
	UNIT_ASSERT_EQUAL(cj.Dump(), "\"abcd\"");

	j = "1\"2'3\\4/5\b6\f7\n8\r9\t0";
	UNIT_ASSERT_EQUAL(cj.Dump(), "\"1\\\"2'3\\\\4\\/5\\b6\\f7\\n8\\r9\\t0\"");

	j[0] = true;
	UNIT_ASSERT_EQUAL(cj.Dump(), "[true]");

	j[1] = "xyz";
	UNIT_ASSERT_EQUAL(cj.Dump(), "[true,\"xyz\"]");

	j += "12345";
	UNIT_ASSERT_EQUAL(cj.Dump(), "[true,\"xyz\",\"12345\"]");

	j += 789;
	UNIT_ASSERT_EQUAL(cj.Dump(), "[true,\"xyz\",\"12345\",789]");

	j["name"] = "Jack";
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"name\":\"Jack\"}");

	j["gender"] = "male";
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"name\":\"Jack\",\"gender\":\"male\"}");

	j = Json();
	UNIT_ASSERT_EQUAL(cj.Dump(), "null");

	j["1"] = Json();
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"1\":null}");

	j[3] = "test";
	UNIT_ASSERT_EQUAL(cj.Dump(), "[null,null,null,\"test\"]");

	j[0] += 123;
	UNIT_ASSERT_EQUAL(cj.Dump(), "[[123],null,null,\"test\"]");

	j[1] += j;
	UNIT_ASSERT_EQUAL(cj.Dump(), "[[123],[[[123],null,null,\"test\"]],null,\"test\"]");

	j[1] += j[1][0];
	UNIT_ASSERT_EQUAL(cj.Dump(), "[[123],[[[123],null,null,\"test\"],[[123],null,null,\"test\"]],null,\"test\"]");

	j[3] += j[3];
	UNIT_ASSERT_EQUAL(cj.Dump(), "[[123],[[[123],null,null,\"test\"],[[123],null,null,\"test\"]],null,[\"test\"]]");

	j[3] += j[3];
	UNIT_ASSERT_EQUAL(cj.Dump(), "[[123],[[[123],null,null,\"test\"],[[123],null,null,\"test\"]],null,[\"test\",[\"test\"]]]");

	j[5] = "1\"2'3\\4/5\b6\f7\n8\r9\t0";
	UNIT_ASSERT_EQUAL(cj.Dump(), "[[123],[[[123],null,null,\"test\"],[[123],null,null,\"test\"]],null,[\"test\",[\"test\"]],null,\"1\\\"2'3\\\\4\\/5\\b6\\f7\\n8\\r9\\t0\"]");
}

UNIT_TEST(Json, Generation2)
{
	Json j;
	const Json& cj = j;

	j[0] = "abc";
	UNIT_ASSERT_EQUAL(cj.Dump(), "[\"abc\"]");

	j[1];
	UNIT_ASSERT_EQUAL(cj.Dump(), "[\"abc\",null]");

	j[1] = j;
	UNIT_ASSERT_EQUAL(cj.Dump(), "[\"abc\",[\"abc\",null]]");

	Json k;
	const Json& ck = k;

	k["value"] = 1;
	UNIT_ASSERT_EQUAL(ck.Dump(), "{\"value\":1}");

	k["obj"] = j;
	UNIT_ASSERT_EQUAL(ck.Dump(), "{\"value\":1,\"obj\":[\"abc\",[\"abc\",null]]}");
}

UNIT_TEST(Json, EqualTo)
{
	UNIT_ASSERT_EQUAL(J("null"), J("null"));
	UNIT_ASSERT_EQUAL(J("null"), J("  null  "));

	UNIT_ASSERT_EQUAL(J("123"), J("123"));
	UNIT_ASSERT_EQUAL(J("abc"), J("abc"));
	UNIT_ASSERT_EQUAL(J("[]"), J("[]"));
	UNIT_ASSERT_EQUAL(J("{}"), J("{}"));

	UNIT_ASSERT_EQUAL(J("[1,2,3]"), J("[1,2,3]"));
	UNIT_ASSERT_EQUAL(J("{a:1,b:2,c:3}"), J("{a:1,b:2,c:3}"));
}

UNIT_TEST(Json, Sub)
{
	Json j;
	const Json& cj = j;

	UNIT_ASSERT_EQUAL(cj.Sub(0).Type(), Json::TYPE_NULL);
	UNIT_ASSERT_EQUAL(cj.Sub(1).Type(), Json::TYPE_NULL);
	UNIT_ASSERT_EQUAL(cj.Sub(99).Type(), Json::TYPE_NULL);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_NULL);
	UNIT_ASSERT_EQUAL(cj.Size(), 0);

	UNIT_ASSERT_EQUAL(j.Size(), 0);
	UNIT_ASSERT_EQUAL(j.Type(), Json::TYPE_NULL);
	UNIT_ASSERT_EQUAL(j.Sub(0).Type(), Json::TYPE_NULL); // non-const Sub() MAY change value
	UNIT_ASSERT_EQUAL(j.Size(), 1);
	UNIT_ASSERT_EQUAL(j.Type(), Json::TYPE_ARRAY);
	UNIT_ASSERT_EQUAL(j.Sub(1).Type(), Json::TYPE_NULL);
	UNIT_ASSERT_EQUAL(j.Size(), 2);
	UNIT_ASSERT_EQUAL(j.Type(), Json::TYPE_ARRAY);
	UNIT_ASSERT_EQUAL(j.Sub(99).Type(), Json::TYPE_NULL);
	UNIT_ASSERT_EQUAL(j.Size(), 100);
	UNIT_ASSERT_EQUAL(j.Type(), Json::TYPE_ARRAY);

	UNIT_ASSERT_EQUAL(cj.Sub("foo").Type(), Json::TYPE_NULL); // const Sub() DOES NOT change value
	UNIT_ASSERT_EQUAL(cj.Size(), 100);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_ARRAY);

	UNIT_ASSERT_EQUAL(j.Sub("foo").Type(), Json::TYPE_NULL); // non-const Sub() MAY change value
	UNIT_ASSERT_EQUAL(cj.Size(), 1);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_OBJECT);
}

UNIT_TEST(Json, Clone)
{
	Json j;
	Json k;

	{
		const char *s = "null";
		UNIT_ASSERT_EQUAL(j.Parse(s), true);
		k.Clone(j);
		UNIT_ASSERT_EQUAL(k.Dump(), j.Dump());
	}
	{
		const char *s = "123";
		UNIT_ASSERT_EQUAL(j.Parse(s), true);
		k.Clone(j);
		UNIT_ASSERT_EQUAL(k.Dump(), j.Dump());
	}
	{
		const char *s = "\"foo\"";
		UNIT_ASSERT_EQUAL(j.Parse(s), true);
		k.Clone(j);
		UNIT_ASSERT_EQUAL(k.Dump(), j.Dump());
	}
	{
		const char *s = "[1,2,3]";
		UNIT_ASSERT_EQUAL(j.Parse(s), true);
		k.Clone(j);
		UNIT_ASSERT_EQUAL(k.Dump(), j.Dump());
	}
	{
		const char *s = "{\"name\":\"Jack\"}";
		UNIT_ASSERT_EQUAL(j.Parse(s), true);
		k.Clone(j);
		UNIT_ASSERT_EQUAL(k.Dump(), j.Dump());
	}
}

UNIT_TEST(Json, StrictParsing)
{
	Json j;
	const Json& cj = j;

	UNIT_ASSERT_EQUAL(j.Parse("", NULL, true), false);
	UNIT_ASSERT_EQUAL(j.Parse("{", NULL, true), false);
	UNIT_ASSERT_EQUAL(j.Parse("}", NULL, true), false);

	UNIT_ASSERT_EQUAL(j.Parse("null", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "null");

	UNIT_ASSERT_EQUAL(j.Parse("true", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "true");

	UNIT_ASSERT_EQUAL(j.Parse("false", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "false");

	UNIT_ASSERT_EQUAL(j.Parse("123", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "123");

	UNIT_ASSERT_EQUAL(j.Parse("3.14159", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "3.14159");

	UNIT_ASSERT_EQUAL(j.Parse("-2.718281828", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "-2.718281828");

	UNIT_ASSERT_EQUAL(j.Parse("\"xyz\"", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "\"xyz\"");

	UNIT_ASSERT_EQUAL(j.Parse("{}", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "{}");

	UNIT_ASSERT_EQUAL(j.Parse("[]", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "[]");

	UNIT_ASSERT_EQUAL(j.Parse("{null}", NULL, true), false);
	UNIT_ASSERT_EQUAL(j.Parse("{\"abcd\"}", NULL, true), false);

	UNIT_ASSERT_EQUAL(j.Parse("{\"a\":null}", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"a\":null}");

	UNIT_ASSERT_EQUAL(j.Parse("{\"a\":{}}", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"a\":{}}");

	UNIT_ASSERT_EQUAL(j.Parse("{\"a\":[[],{}]}", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"a\":[[],{}]}");

	UNIT_ASSERT_EQUAL(j.Parse("{\"xyz\":1234.5678e+90}", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"xyz\":1234.5678e+90}");

	UNIT_ASSERT_EQUAL(j.Parse("{\"a\":\"b c\"}", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"a\":\"b c\"}");

	UNIT_ASSERT_EQUAL(j.Parse("[\"a\"]", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "[\"a\"]");

	UNIT_ASSERT_EQUAL(j.Parse("[[\"a\"]]", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "[[\"a\"]]");

	UNIT_ASSERT_EQUAL(j.Parse("[[[[\"a\"]]]]", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "[[[[\"a\"]]]]");

	UNIT_ASSERT_EQUAL(j.Parse("true /* This is a comment */", NULL, true), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "true");

	UNIT_ASSERT_EQUAL(j.Parse("[{\"a\":\"b\",\"c\":3}}"), false);
}

UNIT_TEST(Json, NonStrictParsing)
{
	Json j;
	const Json& cj = j;

	UNIT_ASSERT_EQUAL(j.Parse(""), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj.Size(), 1);
	UNIT_ASSERT_EQUAL(cj.AsString(), "");

	UNIT_ASSERT_EQUAL(j.Parse("null"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_NULL);
	UNIT_ASSERT_EQUAL(cj.Size(), 0);
	UNIT_ASSERT_EQUAL(cj.AsBool(), false);
	UNIT_ASSERT_EQUAL(cj.AsUint32(), 0);
	UNIT_ASSERT_EQUAL(cj.AsString(), "");
	UNIT_ASSERT_EQUAL(cj.Dump(), "null");

	UNIT_ASSERT_EQUAL(j.Parse("true"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_BOOL);
	UNIT_ASSERT_EQUAL(cj.Size(), 1);
	UNIT_ASSERT_EQUAL(cj.AsBool(), true);
	UNIT_ASSERT_EQUAL(cj.AsUint32(), 1);
	UNIT_ASSERT_EQUAL(cj.AsString(), "true");
	UNIT_ASSERT_EQUAL(cj.Dump(), "true");

	UNIT_ASSERT_EQUAL(j.Parse("false"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_BOOL);
	UNIT_ASSERT_EQUAL(cj.Size(), 1);
	UNIT_ASSERT_EQUAL(cj.AsBool(), false);
	UNIT_ASSERT_EQUAL(cj.AsUint32(), 0);
	UNIT_ASSERT_EQUAL(cj.AsString(), "false");
	UNIT_ASSERT_EQUAL(cj.Dump(), "false");

	UNIT_ASSERT_EQUAL(j.Parse("1"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj.Size(), 1);
	UNIT_ASSERT_EQUAL(cj.AsUint32(), 1);

	UNIT_ASSERT_EQUAL(j.Parse("abc"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj.Size(), 1);
	UNIT_ASSERT_EQUAL(cj.AsString(), "abc");

	UNIT_ASSERT_EQUAL(j.Parse("[1,2,]", NULL, false), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_ARRAY);
	UNIT_ASSERT_EQUAL(cj.Size(), 2);
	UNIT_ASSERT_EQUAL(cj[0].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[0].AsInt32(), 1);
	UNIT_ASSERT_EQUAL(cj[1].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[1].AsInt32(), 2);

	UNIT_ASSERT_EQUAL(j.Parse("[1,2,\"\"]", NULL, false), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_ARRAY);
	UNIT_ASSERT_EQUAL(cj.Size(), 3);
	UNIT_ASSERT_EQUAL(cj[0].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[0].AsInt32(), 1);
	UNIT_ASSERT_EQUAL(cj[1].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[1].AsInt32(), 2);
	UNIT_ASSERT_EQUAL(cj[2].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj[2].AsString(), "");

	UNIT_ASSERT_EQUAL(j.Parse("[3 4,5]", NULL, false), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_ARRAY);
	UNIT_ASSERT_EQUAL(cj.Size(), 3);
	UNIT_ASSERT_EQUAL(cj[0].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[0].AsInt32(), 3);
	UNIT_ASSERT_EQUAL(cj[1].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[1].AsInt32(), 4);
	UNIT_ASSERT_EQUAL(cj[2].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[2].AsInt32(), 5);

	UNIT_ASSERT_EQUAL(j.Parse("[3 4,5\t\" \"]", NULL, false), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_ARRAY);
	UNIT_ASSERT_EQUAL(cj.Size(), 4);
	UNIT_ASSERT_EQUAL(cj[0].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[0].AsInt32(), 3);
	UNIT_ASSERT_EQUAL(cj[1].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[1].AsInt32(), 4);
	UNIT_ASSERT_EQUAL(cj[2].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[2].AsInt32(), 5);
	UNIT_ASSERT_EQUAL(cj[3].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj[3].AsString(), " ");

	UNIT_ASSERT_EQUAL(j.Parse("[abc x34]", NULL, false), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_ARRAY);
	UNIT_ASSERT_EQUAL(cj.Size(), 2);
	UNIT_ASSERT_EQUAL(cj[0].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj[0].AsString(), "abc");
	UNIT_ASSERT_EQUAL(cj[1].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj[1].AsString(), "x34");

	UNIT_ASSERT_EQUAL(j.Parse("{a:1 b:c}", NULL, false), true);
	UNIT_ASSERT_EQUAL(cj.Size(), 2);
	UNIT_ASSERT_EQUAL(cj.Keys().size(), 2);
	UNIT_ASSERT_EQUAL(cj.Keys()[0], "a");
	UNIT_ASSERT_EQUAL(cj.Keys()[1], "b");
	UNIT_ASSERT_EQUAL(cj["a"].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj["a"].AsInt32(), 1);
	UNIT_ASSERT_EQUAL(cj["b"].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj["b"].AsString(), "c");

	UNIT_ASSERT_EQUAL(j.Parse("{\"a\":1,\"b\":\"c\",}", NULL, false), true);
	UNIT_ASSERT_EQUAL(cj.Size(), 2);
	UNIT_ASSERT_EQUAL(cj.Keys()[0], "a");
	UNIT_ASSERT_EQUAL(cj.Keys()[1], "b");
	UNIT_ASSERT_EQUAL(cj["a"].AsInt32(), 1);
	UNIT_ASSERT_EQUAL(cj["b"].AsString(), "c");

	UNIT_ASSERT_EQUAL(j.Parse("[\n\t1,\n\t2,\n]\n", NULL, false), true);
	UNIT_ASSERT_EQUAL(cj.Size(), 2);
	UNIT_ASSERT_EQUAL(cj[0].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[0].AsInt32(), 1);
	UNIT_ASSERT_EQUAL(cj[1].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[1].AsInt32(), 2);

	UNIT_ASSERT_EQUAL(j.Parse("\t{\n\t\t\"a\":1,\n\t\t\"b\":\"c\",\n\t}\n", NULL, false), true);
	UNIT_ASSERT_EQUAL(cj.Size(), 2);
	UNIT_ASSERT_EQUAL(cj.Keys()[0], "a");
	UNIT_ASSERT_EQUAL(cj.Keys()[1], "b");
	UNIT_ASSERT_EQUAL(cj["a"].AsInt32(), 1);
	UNIT_ASSERT_EQUAL(cj["b"].AsString(), "c");

	UNIT_ASSERT_EQUAL(j.Parse("{a_b0:3,_x1:ok}"), true);
	UNIT_ASSERT_EQUAL(cj.Size(), 2);
	UNIT_ASSERT_EQUAL(cj.Keys()[0], "a_b0");
	UNIT_ASSERT_EQUAL(cj.Keys()[1], "_x1");
	UNIT_ASSERT_EQUAL(cj["a_b0"].AsInt32(), 3);
	UNIT_ASSERT_EQUAL(cj["_x1"].AsString(), "ok");

	UNIT_ASSERT_EQUAL(j.Parse("{a:1,b:-2,x:5.7,y:abc,z:3e9,foo:null,bar:{}}"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_OBJECT);
	UNIT_ASSERT_EQUAL(cj.Size(), 7);
	UNIT_ASSERT_EQUAL(j.Parse("{\"a\":1,\"b\":-2,\"x\":5.7,\"y\":\"abc\",\"z\":3e9,\"foo\":null,\"bar\":{}}"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_OBJECT);
	UNIT_ASSERT_EQUAL(cj.Size(), 7);

	UNIT_ASSERT_EQUAL(j.Parse("{\nfoo: 这是一个测试，你猜猜结果怎样？ bar: 结果谁用谁知道\n}"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_OBJECT);
	UNIT_ASSERT_EQUAL(cj.Size(), 2);

	UNIT_ASSERT_EQUAL(j.Parse("[{a:b,c:3}}"), false);

	UNIT_ASSERT_EQUAL(j.Parse("{ipv4:192.168.0.101,name:foo,time:12.345}"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_OBJECT);
	UNIT_ASSERT_EQUAL(cj.Size(), 3);
	UNIT_ASSERT_EQUAL(cj["ipv4"].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj["ipv4"].AsString(), "192.168.0.101");
	UNIT_ASSERT_EQUAL(cj["name"].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj["name"].AsString(), "foo");
	UNIT_ASSERT_EQUAL(cj["time"].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj["time"].AsFloat(), 12.345);

	UNIT_ASSERT_EQUAL(j.Parse("[null,true,false,+1,0,-1]"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_ARRAY);
	UNIT_ASSERT_EQUAL(cj.Size(), 6);
	UNIT_ASSERT_EQUAL(cj[0].Type(), Json::TYPE_NULL);
	UNIT_ASSERT_EQUAL(cj[1].Type(), Json::TYPE_BOOL);
	UNIT_ASSERT_EQUAL(cj[1].AsBool(), true);
	UNIT_ASSERT_EQUAL(cj[2].Type(), Json::TYPE_BOOL);
	UNIT_ASSERT_EQUAL(cj[2].AsBool(), false);
	UNIT_ASSERT_EQUAL(cj[3].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[3].AsInt32(), 1);
	UNIT_ASSERT_EQUAL(cj[4].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[4].AsInt32(), 0);
	UNIT_ASSERT_EQUAL(cj[5].Type(), Json::TYPE_NUMBER);
	UNIT_ASSERT_EQUAL(cj[5].AsInt32(), -1);

	UNIT_ASSERT_EQUAL(j.Parse("[null1,true2,false3\r+1a 0b\n-1c]"), true);
	UNIT_ASSERT_EQUAL(cj.Type(), Json::TYPE_ARRAY);
	UNIT_ASSERT_EQUAL(cj.Size(), 6);
	UNIT_ASSERT_EQUAL(cj[0].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj[0].AsString(), "null1");
	UNIT_ASSERT_EQUAL(cj[1].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj[1].AsString(), "true2");
	UNIT_ASSERT_EQUAL(cj[2].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj[2].AsString(), "false3");
	UNIT_ASSERT_EQUAL(cj[3].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj[3].AsString(), "+1a");
	UNIT_ASSERT_EQUAL(cj[4].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj[4].AsString(), "0b");
	UNIT_ASSERT_EQUAL(cj[5].Type(), Json::TYPE_STRING);
	UNIT_ASSERT_EQUAL(cj[5].AsString(), "-1c");
}

UNIT_TEST(Json, ValueDump)
{
	Json j;
	const Json& cj = j;

	UNIT_ASSERT_EQUAL(j.Parse("{\"book_list\":{\"book\":[{\"title\":\"title 1\"},{\"title\":\"title 2\"}]}}"), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"book_list\":{\"book\":[{\"title\":\"title 1\"},{\"title\":\"title 2\"}]}}");
	UNIT_ASSERT_EQUAL(cj["book_list"].Dump(), "{\"book\":[{\"title\":\"title 1\"},{\"title\":\"title 2\"}]}");
	UNIT_ASSERT_EQUAL(cj["book_list"]["book"].Dump(), "[{\"title\":\"title 1\"},{\"title\":\"title 2\"}]");
	UNIT_ASSERT_EQUAL(cj["book_list"]["book"][0].Dump(), "{\"title\":\"title 1\"}");
	UNIT_ASSERT_EQUAL(cj["book_list"]["book"][1].Dump(), "{\"title\":\"title 2\"}");
}

UNIT_TEST(Json, Format)
{
	Json j;
	const Json& cj = j;

	UNIT_ASSERT_EQUAL(cj.Format(), "null");

	j = true;
	UNIT_ASSERT_EQUAL(cj.Format(), "true");

	j[0] = "abc";
	UNIT_ASSERT_EQUAL(cj.Format(), "[\n\t\"abc\"\n]");

	j["key"] = "value";
	UNIT_ASSERT_EQUAL(cj.Format(), "{\n\t\"key\":\"value\"\n}");

	j[0]["key"]["foo"] = "bar";
	UNIT_ASSERT_EQUAL(cj.Dump(), "[{\"key\":{\"foo\":\"bar\"}}]");
	UNIT_ASSERT_EQUAL(cj.Format(), "[\n\t{\n\t\t\"key\":{\n\t\t\t\"foo\":\"bar\"\n\t\t}\n\t}\n]");
}

UNIT_TEST(Json, RealExamples)
{
	Json j;
	const Json& cj = j;

	const char *s = ""
		"{\n"
		"	\"firstName\": \"John\",\n"
		"	\"lastName\": \"Smith\",\n"
		"	\"age\": 25,\n"
		"	\"address\": {\n"
		"		\"streetAddress\": \"21 2nd Street\",\n"
		"		\"city\": \"New York\",\n"
		"		\"state\": \"NY\",\n"
		"		\"postalCode\": 10021\n"
		"	},\n"
		"	\"phoneNumbers\": [\n"
		"		{\n"
		"			\"type\": \"home\",\n"
		"			\"number\": \"212 555-1234\"\n"
		"		},\n"
		"		{\n"
		"			\"type\": \"fax\",\n"
		"			\"number\": \"646 555-4567\"\n"
		"		}\n"
		"	]\n"
		"}\n";

	UNIT_ASSERT_EQUAL(j.Parse(s), true);

	std::vector<std::string> keys = j.Keys();
	UNIT_ASSERT_EQUAL(keys.size(), 5);
	UNIT_ASSERT_EQUAL(keys[0], "firstName");
	UNIT_ASSERT_EQUAL(keys[1], "lastName");
	UNIT_ASSERT_EQUAL(keys[2], "age");
	UNIT_ASSERT_EQUAL(keys[3], "address");
	UNIT_ASSERT_EQUAL(keys[4], "phoneNumbers");

	UNIT_ASSERT_EQUAL(cj["firstName"].AsString(), "John");
	UNIT_ASSERT_EQUAL(cj["lastName"].AsString(), "Smith");
	UNIT_ASSERT_EQUAL(cj["age"].AsInt32(), 25);

	keys = cj["address"].Keys();
	UNIT_ASSERT_EQUAL(keys.size(), 4);
	UNIT_ASSERT_EQUAL(cj["address"]["streetAddress"].AsString(), "21 2nd Street");
	UNIT_ASSERT_EQUAL(cj["address"]["city"].AsString(), "New York");
	UNIT_ASSERT_EQUAL(cj["address"]["state"].AsString(), "NY");
	UNIT_ASSERT_EQUAL(cj["address"]["postalCode"].AsString(), "10021");

	UNIT_ASSERT_EQUAL(cj["phoneNumbers"].Size(), 2);
	UNIT_ASSERT_EQUAL(cj["phoneNumbers"][0].Size(), 2);
	UNIT_ASSERT_EQUAL(cj["phoneNumbers"][0]["type"].AsString(), "home");
	UNIT_ASSERT_EQUAL(cj["phoneNumbers"][0]["number"].AsString(), "212 555-1234");
	UNIT_ASSERT_EQUAL(cj["phoneNumbers"][1].Size(), 2);
	UNIT_ASSERT_EQUAL(cj["phoneNumbers"][1]["type"].AsString(), "fax");
	UNIT_ASSERT_EQUAL(cj["phoneNumbers"][1]["number"].AsString(), "646 555-4567");
}

UNIT_TEST(Json, Unicode)
{
	Json j;
	const Json& cj = j;

	UNIT_ASSERT_EQUAL(j.Parse("{\"测试\":\"abc\"}"), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"测试\":\"abc\"}");
	UNIT_ASSERT_EQUAL(cj.DumpU(), "{\"\\u00E6\\u00B5\\u008B\\u00E8\\u00AF\\u0095\":\"abc\"}");

	UNIT_ASSERT_EQUAL(j.Parse("{\"\\u00E6\\u00B5\\u008B\\u00E8\\u00AF\\u0095\":\"abc\"}"), true);
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"测试\":\"abc\"}");
	UNIT_ASSERT_EQUAL(cj.DumpU(), "{\"\\u00E6\\u00B5\\u008B\\u00E8\\u00AF\\u0095\":\"abc\"}");
}

UNIT_TEST(Json, Insert)
{
	Json j;
	const Json& cj = j;

	j[1] = "foo";
	UNIT_ASSERT_EQUAL(cj.Dump(), "[null,\"foo\"]");

	j.Insert(Json("bar"), 1);
	UNIT_ASSERT_EQUAL(cj.Dump(), "[null,\"bar\",\"foo\"]");

	j.Insert(Json("abc"));
	UNIT_ASSERT_EQUAL(cj.Dump(), "[null,\"bar\",\"foo\",\"abc\"]");

	j.Insert(Json(123), 2);
	UNIT_ASSERT_EQUAL(cj.Dump(), "[null,\"bar\",123,\"foo\",\"abc\"]");

	j.Insert(Json(-3.14), 0);
	UNIT_ASSERT_EQUAL(cj.Dump(), "[-3.14,null,\"bar\",123,\"foo\",\"abc\"]");
}

UNIT_TEST(Json, Move)
{
	Json j = J("[0,1,2,3,4]");
	j.Move(3, 1);
	UNIT_ASSERT_EQUAL(j, J("[0,3,1,2,4]"));
	j.Move(0);
	UNIT_ASSERT_EQUAL(j, J("[3,1,2,4,0]"));
	j.Move(1);
	UNIT_ASSERT_EQUAL(j, J("[3,2,4,0,1]"));
	j.Move(2,4);
	UNIT_ASSERT_EQUAL(j, J("[3,2,0,4,1]"));
	j.Move(2,3);
	UNIT_ASSERT_EQUAL(j, J("[3,2,0,4,1]"));
	j.Move(2,2);
	UNIT_ASSERT_EQUAL(j, J("[3,2,0,4,1]"));
}

UNIT_TEST(Json, Erase)
{
	Json j;
	const Json& cj = j;

	j["foo"] = "bar";
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"foo\":\"bar\"}");

	j["abc"] = 123;
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"foo\":\"bar\",\"abc\":123}");

	j["xyz"][2] = Json(Json::TYPE_NUMBER, "-3.14");
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"foo\":\"bar\",\"abc\":123,\"xyz\":[null,null,-3.14]}");

	j.Erase("abc");
	UNIT_ASSERT_EQUAL(cj.Dump(), "{\"foo\":\"bar\",\"xyz\":[null,null,-3.14]}");
}

UNIT_TEST(Json, Iterator)
{
	{
		Json j;
		UNIT_ASSERT(j.Begin() == j.End());
		j.Clear(Json::TYPE_BOOL);
		UNIT_ASSERT(j.Begin() == j.End());
		j.Clear(Json::TYPE_NUMBER);
		UNIT_ASSERT(j.Begin() == j.End());
		j.Clear(Json::TYPE_STRING);
		UNIT_ASSERT(j.Begin() == j.End());
		j.Clear(Json::TYPE_ARRAY);
		UNIT_ASSERT(j.Begin() == j.End());
		j.Clear(Json::TYPE_OBJECT);
		UNIT_ASSERT(j.Begin() == j.End());
	}

	{
		Json j;
		UNIT_ASSERT_EQUAL(j.Parse("{a:1,b:2}"), true);

		Json::Iterator it;
		int count = 0;
		for (it = j.Begin(); it != j.End(); ++it, ++count) {
			if (count == 0) {
				UNIT_ASSERT_EQUAL(it.Name(), "a");
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_NUMBER);
				UNIT_ASSERT_EQUAL(it->AsUint32(), 1);
			} else if (count == 1) {
				UNIT_ASSERT_EQUAL(it.Name(), "b");
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_NUMBER);
				UNIT_ASSERT_EQUAL(it->AsInt32(), 2);
			}
		}
		UNIT_ASSERT_EQUAL(count, 2);

		it = j.Begin();
		*it++ = "jyz";
		UNIT_ASSERT_EQUAL(j.Dump(), "{\"a\":\"jyz\",\"b\":2}");

		*it++ = "{}";
		UNIT_ASSERT_EQUAL(j.Dump(), "{\"a\":\"jyz\",\"b\":\"{}\"}");

		UNIT_ASSERT(it == j.End());
	}

	{
		Json j;
		UNIT_ASSERT_EQUAL(j.Parse("{a:1,b:2}"), true);

		Json::ConstIterator it;
		int count = 0;
		for (it = j.Begin(); it != j.End(); ++it, ++count) {
			if (count == 0) {
				UNIT_ASSERT_EQUAL(it.Name(), "a");
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_NUMBER);
				UNIT_ASSERT_EQUAL(it->AsUint32(), 1);
			} else if (count == 1) {
				UNIT_ASSERT_EQUAL(it.Name(), "b");
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_NUMBER);
				UNIT_ASSERT_EQUAL(it->AsInt32(), 2);
			}
		}
		UNIT_ASSERT_EQUAL(count, 2);
	}

	{
		Json j;
		UNIT_ASSERT_EQUAL(j.Parse("[a,b,3]"), true);

		Json::Iterator it;
		int count = 0;
		for (it = j.Begin(); it != j.End(); ++it, ++count) {
			if (count == 0) {
				UNIT_ASSERT_EQUAL(it.Name().empty(), true);
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_STRING);
				UNIT_ASSERT_EQUAL(it->AsString(), "a");
			} else if (count == 1) {
				UNIT_ASSERT_EQUAL(it.Name().empty(), true);
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_STRING);
				UNIT_ASSERT_EQUAL(it->AsString(), "b");
			} else if (count == 2) {
				UNIT_ASSERT_EQUAL(it.Name().empty(), true);
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_NUMBER);
				UNIT_ASSERT_EQUAL(it->AsUint32(), 3);
			}
		}
		UNIT_ASSERT_EQUAL(count, 3);
	}

	{
		Json j;
		UNIT_ASSERT_EQUAL(j.Parse("[a,b,, 3 \" 4\"]"), true);

		UNIT_ASSERT_EQUAL(j.Type(), Json::TYPE_ARRAY);
		UNIT_ASSERT_EQUAL(j.Size(), 4);

		Json::ConstIterator it;
		int count = 0;
		for (it = j.Begin(); it != j.End(); ++it, ++count) {
			if (count == 0) {
				UNIT_ASSERT_EQUAL(it.Name().empty(), true);
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_STRING);
				UNIT_ASSERT_EQUAL(it->AsString(), "a");
			} else if (count == 1) {
				UNIT_ASSERT_EQUAL(it.Name().empty(), true);
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_STRING);
				UNIT_ASSERT_EQUAL(it->AsString(), "b");
			} else if (count == 2) {
				UNIT_ASSERT_EQUAL(it.Name().empty(), true);
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_NUMBER);
				UNIT_ASSERT_EQUAL(it->AsUint32(), 3);
			} else if (count == 3) {
				UNIT_ASSERT_EQUAL(it.Name().empty(), true);
				UNIT_ASSERT_EQUAL(it->Type(), Json::TYPE_STRING);
				UNIT_ASSERT_EQUAL(it->AsString(), " 4");
			}
		}
		UNIT_ASSERT_EQUAL(count, 4);
	}
}

UNIT_TEST(Json, QueryEmptyArray)
{
	Json j = J("{data:[]}");

	UNIT_ASSERT_EQUAL(j.Query("data"), J("[]"));
	UNIT_ASSERT_EQUAL(j.Query("data/foo"), J("null"));
}

UNIT_TEST(Json, QueryNonEmptyArray)
{
	Json j = J("[{name:Alice,age:20},{name:Bob,age:25}]");

	UNIT_ASSERT_EQUAL(j.Query("*/name"), J("[Alice,Bob]"));
	UNIT_ASSERT_EQUAL(j.Query("*/age"), J("[20,25]"));

	UNIT_ASSERT_EQUAL(j.Query("0/name"), J("Alice"));
	UNIT_ASSERT_EQUAL(j.Query("1/name"), J("Bob"));
	UNIT_ASSERT_EQUAL(j.Query("2/name"), J("null"));
	UNIT_ASSERT_EQUAL(j.Query("0/*"), J("[Alice,20]"));
	UNIT_ASSERT_EQUAL(j.Query("0"), J("{name:Alice,age:20}"));
	UNIT_ASSERT_EQUAL(j.Query("*/*"), J("[Alice,20,Bob,25]"));
}
