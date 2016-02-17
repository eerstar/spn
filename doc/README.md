# 单元测试库 - libnpunit

本类库主要用于做实现自动化单元测试。

## 基本用法：

新建（一个或多个）cpp文件，基本形式如下：

	#include "UnitTest.h"

	void do_sth_with_except() { throw 1; }
	void do_sth_without_except() { }

	UNIT_TEST(Pkg, Name)
	{
		UNIT_ASSERT(sizeof(short int) == 2);                // 布尔断言
		UNIT_ASSERT_EQUAL(sizeof(short int), 2);            // 相等断言
		UNIT_ASSERT_EXCEPTION(do_sth_with_except());        // 有异常断言
		UNIT_ASSERT_NO_EXCEPTION(do_sth_without_except());  // 无异常断言
	}

编译并链接`libnpunit.a`即可，所生成的可执行文件（假设为`unittest`），直接运行即可：

	$ ./unittest  # 运行所有测试

或者可以跟参数运行：

	$ ./unittest Pkg         # 运行一组测试
	$ ./unittest Pkg::Name   # 运行单个测试
