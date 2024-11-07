# vscode基本知识

1. 假如有源代码文件 helloworld.cpp
2. 编译helloworld.cpp
    这一步将会创建tasks.json文件，告诉VSCode怎么编译程序；
    在菜单栏，依次点击终端→配置默认生成任务，在弹出的选择框中选择g++.exe build active file，将会自动创建.vscode文件夹和tasks.json文件；
    回到helloworld.cpp文件，在菜单栏依次点击 终端→运行生成任务，或者使用快捷键Ctrl+Shift+B；
    编译完成后，在源代码下方弹出的终端窗口中，会显示编译是否成功的信息；如下图所示；
    ![](https://pic2.zhimg.com/80/v2-765112787de4be0b1919dbdbaef6938d_720w.webp)
    在终端窗口右侧点击+按钮，在新的终端窗口中输入dir，可以看到helloworld文件夹中生成了helloworld.exe文件；
    输入.\helloworld.exe执行文件，窗口中显示出程序运行的结果；
    ![](https://pic2.zhimg.com/80/v2-66c2f08fc8733c914f91b1fe6ac8c2a1_720w.webp)
    **配置构建任务**：tasks.json 文件是 Visual Studio Code 中用于配置任务运行的文件。任务可以是编译源代码、运行测试、执行自定义脚本等。通过配置 tasks.json 文件，你可以定义一系列任务，并指定它们的执行命令、参数以及其他相关设置。在tasks.json文件中配置编译器的路径、源文件、输出文件等参数。这里可以指定编译器如何生成.exe文件，以及编译过程中需要包含的源代码文件或库。
    **编译生成exe文件**一旦配置好构建任务，就可以通过执行构建命令来编译项目并生成.exe文件了。
    执行构建任务：在VS Code中，可以通过快捷键Ctrl+Shift+B（或从Terminal的Run Build Task菜单中选择）来执行构建任务。
    检查输出：编译成功后，.exe文件将会生成在指定的输出目录中。这时，可以通过双击.exe文件来运行应用程序，或者将其分发给他人使用。
    **为什么在VS Code中生成exe文件**生成exe文件是为了将编写好的代码转化为可执行文件，方便用户在计算机上直接运行自己的程序。在VS Code中生成exe文件可以提供更方便的开发和部署方式。

3. 调试helloworld.cpp
    这一步将会自动生成launch.json文件，可以配置VSCode启动GDB调试器；
    依次点击菜单栏 运行→添加配置，选择C++（GDB/LLDB）；
    在弹出的下拉选项中，选择g++.exe - 生成和调试活动文件；
    VSCode将会自动生成launch.json文件；
    回到helloworld.cpp文件，这样helloworld.cpp就是活动文件；
    依次点击菜单栏 运行→启动调试，或者使用快捷键F5；
    在下面的终端窗口中，将会出现调试的信息；
    这时程序里没有打上任何断点，可以在源文件某一行代码的前面打上断点，可以单步调试程序；
    可以将launch.json文件中的stopAtEntry修改为true，那么调试程序时，将会在main函数入口打上断点，自动停止；
    在窗口上方，也可以看到调试的按钮，如下图所示；
    在窗口左侧有变量、监视的窗口，在监视一栏中可以添加关注的变量；
    ![](https://pic2.zhimg.com/v2-600d5ac29eb01eaa74a9ff6f96ffbe61_r.jpg)
4. C/C++配置
    使用快捷键Ctrl'+Shift+P，打开命令面板；
    在命令面板中，输入并选中C/C++ 编辑配置(UI)，将会弹出C/C++配置页面；
    在C/C++配置页面，选择配置名称：Win32，编译器路径：D:/Program Files/mingw-w64/x86_64-8.1.0-release-win32-seh-rt_v6-rev0/bin/gcc.exe；
    配置完成后，点击资源管理器，可以看到在.vscode文件夹中自动生成了c_cpp_properties.json文件；
    如果程序中使用的头文件不在工作空间或标准头文件的路径中，可以在Include path中添加搜索的路径；


# 配置调试文件
launch: 配置调试器的行为，以便在开发过程中调试代码。
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug My Project",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/path/to/your/${fileBasenameNoExtension}", // 可执行文件路径
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb", // 或 "lldb" 根据你的编译器
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "build", // 指定前置构建任务
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}

```

+ ${workspaceFolder}：指代当前工作区的根文件夹。
+ ${fileBasenameNoExtension}：会解析为当前打开文件的名称（不包括扩展名）。
+ ${fileDirname} : 代表当前打开文件所在目录的路径。 如果你当前打开的文件是 /home/user/my_project/src/main.cpp，那么 ${fileDirname} 将解析为 /home/user/my_project/src。
+ ${fileBasenameNoExtension}: 代表当前打开文件的基本名称，不包括文件扩展名。如果当前打开的文件是 example.cpp，那么 ${fileBasenameNoExtension} 将解析为 example。
+ ${file} :代表当前打开的文件的完整路径，包括文件名和扩展名。/home/user/my_project/src/example.cpp，那么 ${file} 将解析为这个完整路径。

在 VSCode 中通过 launch.json 文件传递命令行参数给 main(int argc, char **argv) 函数非常简单。你需要在 launch.json 文件中的 args 属性中指定这些参数。下面是如何具体操作的步骤：
` "args": ["--input", "file.txt", "--output", "output.txt"], // 这里是你的命令行参数`
args 数组：数组中的每个元素都是一个字符串，代表一个单独的命令行参数。数组的第一个元素会被视为 argv[1]，第二个元素为 argv[2]，以此类推。argv[0] 通常代表可执行文件的名字，是由调试器自动提供的。
字符串字面量：每个参数都应该是字符串字面量，例如 "--input", "file.txt" 等等。

tasks: 定义和管理构建、清理等任务，可以用来自动化常见的开发流程
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "g++", // 或 "clang++" 根据你的编译器
            "args": [
                "-g", // 生成调试信息
                "${workspaceFolder}/src/*.cpp", // 源文件路径
                "-o",
                "${workspaceFolder}/path/to/your/executable" // 输出路径
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"]
        }
    ]
}
```
"-g"：
  这个选项告诉编译器生成调试信息。这对调试非常重要，因为它使得调试器能够提供源代码级别的调试体验（例如，设置断点、查看变量等）。

${workspaceFolder}/src/*.cpp：
  这是一个路径模式，表示要编译的源文件。${workspaceFolder} 是一个变量，代表当前工作区的根文件夹。这个表达式会匹配 src 文件夹下的所有 .cpp 文件。
  如果你有多个源文件，可以使用通配符 *.cpp 来一次性编译它们。

"-o"：
  这个选项后面跟着的是输出文件的路径，指示编译器将生成的可执行文件保存到指定位置。
  -o 后面的路径可以是相对路径或绝对路径。

${workspaceFolder}/path/to/your/executable：
  这是指定输出可执行文件的完整路径。编译器将在这个位置创建生成的可执行文件。
  你需要将 path/to/your/executable 替换为你实际想要的输出文件名。