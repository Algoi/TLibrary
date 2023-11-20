### 1、jsoncpp

Jsoncpp 是个跨平台的 C++ 开源库，提供的类为我们提供了很便捷的操作，而且使用的人也很多。在使用之前我们首先要从 github 仓库下载源码，地址如下：

```sh
https://github.com/open-source-parsers/jsoncpp
```

然后通过cmake构建生成对应的动态库静态库即可使用。



### 2、jsoncpp详解

#### 2.1 说明

jsoncpp库中的类被定义到了一个 Json 命名空间中，所以在使用对应的API时需要加上Json这个命名空间。

```
使用jsoncpp库解析 json 格式的数据，我们只需要掌握三个类：

Value 类：	 将 json 支持的数据类型进行了包装，最终得到一个 Value 类型
FastWriter类： 将 Value 对象中的数据序列化为字符串
Reader类：	 反序列化，将 json 字符串 解析成 Value 类型
```

#### 2.2 Value类

这个类是Json的一个包装类，能够封装Json所支持的所有数据类型，能够提高开发效率。

| 枚举类型     | 说明                      | 翻译                         |
| ------------ | ------------------------- | ---------------------------- |
| nullValue    | "null" value              | 不表示任何数据，空值         |
| intValue     | signed integer value      | 表示有符号的整数             |
| uintValue    | unsigned integer value    | 表示无符号整数               |
| realValue    | double value              | 表示浮点数                   |
| stringValue  | UTF-8 string value        | 表示utf8格式的字符串         |
| booleanValue | bool value                | 表示布尔数                   |
| arrayValue   | array value(ordered list) | 表示数组，即JSON串中的[ ]    |
| objectValue  | object value              | 表示键值对，即JSON串中的 { } |

```c++
enum ValueType {
  nullValue = 0, ///< 'null' value
  intValue,      ///< signed integer value
  uintValue,     ///< unsigned integer value
  realValue,     ///< double value
  stringValue,   ///< UTF-8 string value
  booleanValue,  ///< bool value
  arrayValue,    ///< array value (ordered list)
  objectValue    ///< object value (collection of name/value pairs).
};
```

##### 2.2.1 Value类构造函数

Value 类提供了很多构造函数，通过构造函数来封装Json支持的各种类型的数据，最终得到一个统一的类型。

```c++
// Json::Value实现了各种数据类型的构造函数
Value(ValueType type = nullValue);
Value(Int value);
Value(UInt value);
Value(Int64 value);
Value(UInt64 value);
Value(double value);
Value(const char* value);
Value(const char* begin, const char* end);
Value(bool value);
Value(const Value& other);
Value(Value&& other);
```

##### 2.2.2 检测数据类型

```c++
// 检测Value中包装的数据类型
bool isNull() const;
bool isBool() const;
bool isInt() const;
bool isInt64() const;
bool isUInt() const;
bool isUInt64() const;
bool isIntegral() const;
bool isDouble() const;
bool isNumeric() const;
bool isString() const;
bool isArray() const;
bool isObject() const;
```

##### 2.2.3 Value对象转换为实际类型

```c++
Int asInt() const;
UInt asUInt() const;
Int64 asInt64() const;
UInt64 asUInt64() const;
LargestInt asLargestInt() const;
LargestUInt asLargestUInt() const;
JSONCPP_STRING asString() const;
float asFloat() const;
double asDouble() const;
bool asBool() const;
const char* asCString() const;
```

##### 2.2.4 对json数组的操作

```c++
ArrayIndex size() const;

Value& operator[](ArrayIndex index);
Value& operator[](int index);
const Value& operator[](ArrayIndex index) const;
const Value& operator[](int index) const;

// 根据下标的index返回这个位置的value值
// 如果没找到这个index对应的value, 返回第二个参数defaultValue
Value get(ArrayIndex index, const Value& defaultValue) const;

Value& append(const Value& value);

const_iterator begin() const;
const_iterator end() const;
iterator begin();
iterator end();
```

##### 2.2.5 对json对象的操作

```c++
Value& operator[](const char* key);
const Value& operator[](const char* key) const;
Value& operator[](const JSONCPP_STRING& key);
const Value& operator[](const JSONCPP_STRING& key) const;
Value& operator[](const StaticString& key);

// 通过key, 得到value值
Value get(const char* key, const Value& defaultValue) const;
Value get(const JSONCPP_STRING& key, const Value& defaultValue) const;
Value get(const CppTL::ConstString& key, const Value& defaultValue) const;

// 得到对象中所有的键值
typedef std::vector<std::string> Members;
Members getMemberNames() const;

```

##### 2.2.6 将Value对象数据序列化为string

```c++
//序列化得到的字符串是有样式的，带有换行和缩进等，方便阅读可以选这种方式序列化
//在写配置文件的时候可以使用这个序列化方式
std::string toStyledString() const;
```

#### 2.3 FastWriter类

```c++
//了解一个函数即可，用于序列化，与toStyledString功能类似，但是没有格式。
//进行网络数据传输的时候可以使用这种方式序列化
std::string Json::FastWriter::write(const Value & root);
```

#### 2.4 Reader类

解析来自来自Json串或者文件流中的json格式的字符串，并且返回一个Value对象。

```c++
bool Json::Reader::parse(const std::string& document, Value& root, bool collectComments = true);
    参数:
        - document: json格式字符串
        - root: 传出参数, 存储了json字符串中解析出的数据
        - collectComments: 是否保存json字符串中的注释信息

// 通过begindoc和enddoc指针定位一个json字符串
// 这个字符串可以是完成的json字符串, 也可以是部分json字符串
bool Json::Reader::parse(const char* beginDoc, const char* endDoc, Value& root, bool collectComments = true);
	
// write的文件流  -> ofstream
// read的文件流   -> ifstream
// 假设要解析的json数据在磁盘文件中
// is流对象指向一个磁盘文件, 读操作
bool Json::Reader::parse(std::istream& is, Value& root, bool collectComments = true);

```



### 3、实操

##### 3.1 文件

```c++
#include <iostream>
#include <ostream>
#include <fstream>
using namespace std;

#include "json/json.h"
using namespace Json;

/**
[
    12,
    12.34,
    true,
    "tom",
    ["jack", "ace", "robin"],
    {"sex":"man", "girlfriend":"lucy"}
]
*/

void writeJson()
{
    // 将最外层的数组看做一个Value
    // 最外层的Value对象创建
    Value root;
    // Value有一个参数为int 行的构造函数
    root.append(12); // 参数进行隐式类型转换
    root.append(12.34);
    root.append(true);
    root.append("tom");

    // 创建并初始化一个子数组
    Value subArray;
    subArray.append("jack");
    subArray.append("ace");
    subArray.append("robin");
    root.append(subArray);

    // 创建并初始化子对象
    Value subObj;
    subObj["sex"] = "woman"; // 添加键值对
    subObj["girlfriend"] = "lucy";
    root.append(subObj);

    // 序列化
#if 0
    // 有格式的字符串
    string str = root.toStyledString();
#else
    // 没有格式的字符串
    FastWriter f;
    string str = f.write(root);
#endif
    // 将序列化的字符串写磁盘文件
    std::ofstream ofs("test1.json");
    ofs << str;
    ofs.close();
}

void readJson()
{
    // 1. 将磁盘文件中的json字符串读到磁盘文件
    ifstream ifs("test.json");
    // 2. 反序列化 -> value对象
    Value root;
    Reader r;
    r.parse(ifs, root);
    // 3. 从value对象中将数据依次读出
    if (root.isArray())
    {
        // 数组, 遍历数组
        for (int i = 0; i < root.size(); ++i)
        {
            // 依次取出各个元素, 类型是value类型
            Value item = root[i];
            // 判断item中存储的数据的类型
            if (item.isString())
            {
                cout << item.asString() << ", ";
            }
            else if (item.isInt())
            {
                cout << item.asInt() << ", ";
            }
            else if (item.isBool())
            {
                cout << item.asBool() << ", ";
            }
            else if (item.isDouble())
            {
                cout << item.asFloat() << ", ";
            }
            else if (item.isArray())
            {
                for (int j = 0; j < item.size(); ++j)
                {
                    cout << item[j].asString() << ", ";
                }
            }
            else if (item.isObject())
            {
                // 对象
                // 得到所有的key
                Value::Members keys = item.getMemberNames();
                for (size_t k = 0; k < keys.size(); ++k)
                {
                    cout << keys.at(k) << ":" << item[keys[k]] << ", ";
                }
            }
        }
        cout << endl;
    }
}

int main()
{
    writeJson();	//将json字符串写入文件中
    readJson();		//读取文件中的json字符串转为Value对象读取数据
}
```

