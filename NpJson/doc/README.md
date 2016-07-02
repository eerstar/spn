# JSON类库 - libnpjson

## 概述

用于处理json字符串的解析和构造.

## 快速上手

示例：

	/* JSON示例：
       {
           "a": 123,
           "b": "xyz",
           "c": [ 1, 2.5 ]
       }
	*/
    Json x;
    x.Parse("{\"a\":123,\"b\":\"xyz\",\"c\":[1,2.5]}"); // 通过解析来构造json对象

    std::cout << x << std::endl;           // (以紧凑格式)输出整个json字符串
    std::cout << x.Format() << std::endl;  // 格式化地输出json字符串（对于太长的文本将会自动省略部分内容）

    std::string s = x.Dump();              // 将json字符串存入一个字符串变量
    std::string t = x.Format();            // 将格式化的json字符串存入一个字符串变量

    std::cout << x["a"] << std::endl; // 输出子节点，结果：123
    std::cout << x["b"] << std::endl; // 输出子节点，结果：xyz
    std::cout << x["c"] << std::endl; // 输出子节点，结果："c":[1,2]

    int32_t a = x["a"].AsInt32();             // 提取子节点的数字
    const std::string& b = x["b"].AsString(); // 提取子节点的字符串
    const Json& c = x["c"];                  // 提取子节点
    int32_t c1 = c[0].AsInt32();              // 提取数组元素
    double c2 = c[1].AsDouble();              // 提取数组元素

    x["a"] = "foo";      // 修改子节点
    x["d"] = "bar";      // 新增子节点
    x.Erase("b");        // 删除子节点
    x["c"][3] = "test";  // 添加数组元素，能根据需要自动扩充数组长度
    std::cout << x << std::endl;  // 结果：{"a":"foo","c":[1,2.5,null,"test"],"d":"bar"}

    for (Json::ConstIterator it = x.Begin(); it != x.End(); ++it) {
        const Json& sub = *it;
        std::cout << it->Name() << " => " << sub << std::endl;  // 遍历并打印每个子节点的名称和内容
    }

## 用法

只有一个Json类，代表一个json节点（及其下属所有子节点）.

### 构造

1. 节点类型定义：

        enum Json::JsonType;

    取值包括：

        Json::TYPE_NULL    // 空（null）
        Json::TYPE_BOOL    // 真/假
        Json::TYPE_NUMBER  // 数值
        Json::TYPE_STRING  // 字符串
        Json::TYPE_ARRAY   // 数组（支持异构元素）
        Json::TYPE_OBJECT  // 对象（类似C/C++中的struct）

2. 构造函数：

        Json x;  // 缺省为null

        Json x(true);  // 用bool类型构造
        Json x(false);

        Json x(123);      // 用数值构造，可以使用的类型包括：
        Json x(-45);      //   int, float, double,
        Json x(3.14159);  //   int8_t, int16_t, int32_t, int64_t,
        Json x(5.2e-9);   //   uint8_t, uint16_t, uint32_t, uint64_t

        Json x("foo");               // 用C字符串（原生字符指针）构造
        Json x(std::string("bar"));  // 用C++字符串构造

        Json x(Json::TYPE_NUMBER, "12.34");  // 指定类型，用字符串构造

    对于“指定类型，用字符串构造”的方式，第二个参数并不是总是有效，而有可能会被强制使用某具体值：

    | 类型                 | 最终取值                                              |
    |----------------------|-------------------------------------------------------|
    | `Json::TYPE_NULL`   | "null"                                                |
    | `Json::TYPE_BOOL`   | "true" 或 "false" （若使用其它值将被强制设为"false"） |
    | `Json::TYPE_NUMBER` | text （保留参数的字符串，但转换为数字使用时可能失败） |
    | `Json::TYPE_STRING` | text （保留参数的字符串）                             |
    | `Json::TYPE_ARRAY`  | "" （清空）                                           |
    | `Json::TYPE_OBJECT` | "" （清空）                                           |

3. 赋值：

        Json x;

        x = true;  // 用bool类型赋值
        x = false;

        x = 123;  // 用数值赋值，类型支持同构造函数
        x = -45;
        x = 3.14159;
        x = 5.2e-9;

        x = "foo";               // 用C字符串（原生字符指针）赋值
        x = std::string("bar");  // 用C++字符串赋值

        x[2] = 1;      // 用数字作下标，构造数组类型的json
        x["key"] = 2;  // 用字符串做下标，构造对象类型的json

        Json y;
        y["foo"][3] = "bar";  // 从头直接构造：{"foo":[null,null,"bar"]}

4. 清空：

    作用类似“带类型，用字符串构造”的构造函数.

5. 插入子节点（数组或对象）：

        Json x;
        x.Insert(Json("foo"));       // 向数组插入元素（若非数组则先初始化为数组）
        x.Insert("key", Json(123));  // 向对象插入元素（若非对象则先初始化为对象）

6. 删除子节点（数组或对象）：

        Json x;
        x.Erase(2);      // 删除数组中下标为2的那个元素（即第3个元素）
        x.Erase("key");  // 删除对象中名为"key"的那个元素

### 取值

1. 节点取值

        Json x;

        x.AsBool();  // 按bool类型取值

        x.AsInt8();  // 按数值类型取值
        x.AsInt16();
        x.AsInt32();
        x.AsInt64();
        x.AsUint8();
        x.AsUint16();
        x.AsUint32();
        x.AsUint64();
        x.AsFloat();
        x.AsDouble();

        x.AsString();  // 按字符串类型取值

    对于`AsBool()`，根据原对象的不同类型，会有不同的转换方法：

    | 类型                | AsBool的取值方式     |
    |---------------------|----------------------|
    | `Json::TYPE_NULL`   | false                |
    | `Json::TYPE_BOOL`   | 根据实际存储内容取值 |
    | `Json::TYPE_NUMBER` | 是否非0              |
    | `Json::TYPE_STRING` | 是否非空字符串       |
    | `Json::TYPE_ARRAY`  | 是否元素个数非0      |
    | `Json::TYPE_OBJECT` | 是否元素个数非0      |

2. 取子节点

    Json x;
    x[2];      // 取数组的下标为2的元素（即第3个元素）
    x["key"];  // 取对象中名为"key"的子节点

### 解析

执行Json的成员函数Parse()，将待解析的字符串传入该函数即可。

    Json x;
    x.Parse("[1,2,\"foo\",{\"a\":\"b\"}]");

Json::Parse() 有三个参数，第一个参数为需要解析的字符串，后两个都可选。第二个参数为指针引用，用来返回解析完成后的字符串末尾位置（可用于判断json末尾是否有多余字符）。第三个参数可指定是否采用严格解析（详见后文），缺省为非严格。

### 输出

    Json x;
    x[0]["key"]["foo"] = "bar";  // 初始化一个例子

    std::cout << x << std::endl;  // 可以直接输出Json对象

    std::cout << x.Format() << std::endl;  // 也可以将其格式化好后再输出

上述例子中，输出分别为：

    [{"key":{"foo":"bar"}}]

和

    [
        {
            "key":{
                "foo":"bar"
            }
        }
    ]

### 遍历

1. 对于数组，可以直接使用数字下标：

        Json x;
        x.Parse("[a,b,1,99]");
        for (size_t i = 0; i < x.Size(); ++i) {
            std::cout << x[i] << std::endl;
        }

2. 对于对象，可以使用字符串下标：

        Json x;
        x.Parse("{a:1,b:2,c:3.4,d:xyz}");
        const std::vector<std::string> keys = x.Keys();
        for (size_t i = 0; i < keys.size(); ++i) {
            std::cout << x[keys[i]] << std::endl;
        }

3. 使用迭代器遍历数组：

        Json x;
        x.Parse("[a,b,1,99]");
        for (Json::ConstIterator it = x.Begin(); it != x.End(); ++it) {
            std::cout << *it << std::endl;
        }

4. 使用迭代器遍历对象：

        Json x;
        x.Parse("{a:1,b:2,c:3.4,d:xyz}");
        for (Json::ConstIterator it = x.Begin(); it != x.End(); ++it) {
            std::cout << it.Name() << " => " << *it << std::endl;
        }

### 迭代器

1. 迭代器的类型

    Json的迭代器分为两种：Json::Iterator 和 Json::ConstIterator，分别用于操作非const对象和const对象。

2. 迭代器的操作

    对于数组和对象，操作方法都相同，见前面“遍历”的示例。对于其它类型的Json，则 x.Begin() 和 x.End() 将返回相同值（即不会进入迭代器遍历的循环）。


## 非严格json

为方便使用，作为扩展，Json的解析函数缺省会忽略某些错误，而实现非严格的解析：

1. 数组或对象的每个元素末尾，可以带“,”，也可以不带“,”，而仅使用空格隔开.

        [1,2,]   // 最后一个元素后面仍然可以带上“,”
        [3 4,5]  // 元素之间可以只有空格隔开，本例为三个元素的数组

2. 当字符串只由字母数字构成时，可以省略双引号：

        [abc x34]  // 两个字符串元素组成的数组
        {a:1 b:c}  // 两个元素组成的对象，前一个元素为数字，后一个为字符串

若要执行严格解析，则应指定`Parse()`的第三个参数为true：

    Json j;
    j.Parse("[1,2,]", NULL, true);  // Parse()将失败并返回false

## 常见陷阱

1. 对于`operator []`运算符：

    const变量的行为与非const变量的行为是稍有差别的.

    对于非const变量，若类型非json数组/json对象，则会先初始化为该类型，再做元素提取.

    对于const变量，则不会有任何类型或值的改变.

2. 对于如下操作：

        Json x;
        x["foo"] = "bar";  // {"foo":"bar"}
        x["self"] = x;     // {"foo":"bar","self":{"foo":"bar","self":null}}

    第二个赋值操作，其实是在执行`operator []`之后，也就是创建了`self`子节点之后。
