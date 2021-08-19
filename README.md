# JsonParser

- 不同类型的Json继承自基类JsonValue
- Json类中包含指向JsonValue的智能指针，通过多态机制统一管理不同类型的JsonValue
- array类型的JsonArray使用std::vector\<Json\>管理
- object类型的JsonObject使用std::map\<std::string,Json\>管理
- 采用递归下降解析方式
- 使用智能指针管理资源
- 使用右值引用减少对象拷贝开销

## 使用方法

```C++
    std::string str="your json";//建议使用C++11原始字面量
    JsonParser json(str);//解析器

    std::string res;//format结果储存位置
    json.parse_json().format(res);

    std::cout<<res<<std::endl;
```

## 示例

```C++
//main.cpp
#include "json.hpp"

int main()
{
    std::string str=R"(
{ 
  "version": "0.2.0",
  "configurations": [
    {
      "name": "g++.exe build and debug active file",
      "type": "cppdbg",
      "request": "launch",
      "program": "${fileDirname}\\${fileBasenameNoExtension}",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}",
      "environment": [],
      "externalConsole": true,
      "MIMode": "gdb",
      "miDebuggerPath": "D:\\mingw-w64\\bin\\gdb.exe",
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": false
        }
      ],
      "preLaunchTask": "g++.exe build active file"
    }
  ]
})";
    JsonParser json(str);

    std::string res;
    json.parse_json().format(res);

    std::cout<<res<<std::endl;
    return 0;
}
```
## 运行结果
```
//stdout
{configurations:[{IMode:"gdb", args:[], environment:[], externalConsole:true, miDebuggerPath:"D:\\mingw-w64\\bin\\gdb.exe", name:"g++.exe build and debug active file", preLaunchTask:"g++.exe build active file", program:"${fileDirname}\\${fileBasenameNoExtension}", request:"launch", setupCommands:[{description:"Enable pretty-printing for gdb", ignoreFailures:false, text:"-enable-pretty-printing"}], stopAtEntry:false, type:"cppdbg", wd:"${workspaceFolder}"}], version:"0.2.0"}
```
## tips

同时支持自行构造json类型，例如

```C++
    Json myJson=Json::object{
    { "key1", "value1" },
    { "key2", false },
    { "key3", Json::array { 1, 2, 3 } },
};
```
更多细节详见 json.hpp
