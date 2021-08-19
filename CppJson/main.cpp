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
