#ifndef __UNIT_TEST_H__
#define __UNIT_TEST_H__

#include <sstream>

class UnitTest
{
public:
	typedef void (*Proc)();
	UnitTest(const char* pkg, const char* name, Proc proc);
	static void Test(bool res, const char* file, int line,
			const char* func, const char* expr, const char* msg = "");
	template <typename T, typename V>
	static void Test(T value, V expected, const char* file,
			int line, const char *func, const char *expr);
};

template <typename T, typename V>
void UnitTest::Test(T value, V expected, const char* file,
		int line, const char *func, const char *expr)
{
	std::ostringstream ss;
	bool res = (value == static_cast<T>(expected));
	if (!res) {
		ss << "\n"
			"expected = " << expected << "\n"
			"   value = " << value << std::endl;
	}
	Test(res, file, line, func, expr, ss.str().c_str());
}

#ifndef CONCAT
#define CONCAT_(x, y) x##y
#define CONCAT(x, y) CONCAT_(x, y)
#endif

#define UNIT_TEST(Pkg, Name) \
	void UnitTest_##Pkg##_##Name(); \
	static UnitTest CONCAT(unitTest, __LINE__)(#Pkg, #Name, UnitTest_##Pkg##_##Name); \
	void UnitTest_##Pkg##_##Name()

#define UNIT_ASSERT(exp) \
	UnitTest::Test(exp, __FILE__, __LINE__, __FUNCTION__, #exp)

#define UNIT_ASSERT_EQUAL(value, expected) \
	UnitTest::Test(value, expected, __FILE__, __LINE__, __FUNCTION__, #value " == " #expected)

#define UNIT_ASSERT_EXCEPTION(expression, exceptionType) \
	do { \
		bool caught = false; \
		try { (expression); } catch (exceptionType) { caught = true; }; \
		UnitTest::Test(caught, __FILE__, __LINE__, __FUNCTION__, #expression, \
				" Exception '" #exceptionType "' was expected."); \
	} while(0)

#define UNIT_ASSERT_ANY_EXCEPTION(expression) \
	UNIT_ASSERT_EXCEPTION(expression, ...)

#define UNIT_ASSERT_NO_EXCEPTION(expression) \
	do { \
		bool caught = false; \
		std::string err; \
		try { (expression); } \
		catch (const std::exception& e) { err = e.what(); caught = true; } \
		catch (...) { caught = true; }; \
		UnitTest::Test(!caught, __FILE__, __LINE__, __FUNCTION__, #expression, \
				(err.empty() ? " No any exception was expected." : \
				(" Unexpected exception: " + err).c_str())); \
	} while(0)

#endif
