## CMake学习使用

### 1、cmake安装和入门使用

#### 1.1 安装

```sh
sudo apt install cmake	# 即可安装

cmake -version  # 查看安装的cmake版本
```

![01_cmake](images\01_cmake.png)

#### 1.2 简单程序使用cmake

在指定的目录中作为项目目录，里面只有一个Apply.cpp文件。此外为了使用cmake，需要有一个CMakeLists.txt文件。内容如下：

![02_cmake](images\02_cmake.png)

在项目目录中创建一个build文件夹，然后进入该目录进行命令的操作。如下：

![03_cmake](images\03_cmake.png)

此时，有了Makefile文件，然后可以使用make命令来生成可执行程序app，如下：

![04_cmake](images\04_cmake.png)

最后可以使用app来执行程序。如果需要删除app可执行文件，可以使用make clean然后app就会被清理掉，再使用make又可以重新生成。

#### 1.3 总结三个cmake命令

```sh
# 1、设置对cmake的最小版本，这里设置了要求的最低版本是3.0
cmake_minimum_required(VERSION 3.0)

# 2、设置工程名为apply，此外还可以设置工程的版本，以及对工程的描述。
project(apply VERSION 1.0 DESCRIPTION "这是一个简单的工程")

# 3、生成可执行文件
add_executable(app Apply.cpp)
# add_executable(exename source1 source2 ... sourceN)
# 第一个参数为可执行程序名，后面的是n多个cpp文件
```



### 2、项目级的组织结构

#### 2.1 项目结构安排

![05_cmake](images\05_cmake.png)

4.0.1目录是项目目录，其中include是项目的头文件，所有头文件都放在这里面；src是cpp源文件的目录；build是cmake命令生成的相关的文件的目录（Makefile）；bin目录是make命令之后生成的目标对象的存放目录（可执行文件、静态库、动态库）。CMakeLists.txt文件内容如下：

```sh
# cmake配置

# 1、设置对cmake的最小版本
cmake_minimum_required(VERSION 3.0)

# 2、设置工程名
project(apply VERSION 12.0 DESCRIPTION "这是一个项目级别的工程")

# 3、指定需要的头文件目录
include_directories(include) # 相对于CMakeLists.txt所在的目录

# 4、获取指定目录下的所有cpp文件，用SRC_LIST变量代替
# 如果src目录下的cpp文件很多，在生成可执行文件时一个个写出来十分不便，所以可以使用这个命令方便使用
# aux_source_directory(src SRC_LIST)

# 4.1
# aux_source_directory虽然能够很方便的获取所有cpp，但是某些情况下有部分cpp不需要，此时可以使用set命令解决
set(
    SRC_LIST 
    ./src/Computer.cpp
    ./src/Log.cpp
    ./src/main.cpp
)

# 5、生成可执行文件，这里的所有cpp文件用变量替代
add_executable(app ${SRC_LIST})

# 6、设置目标对象（可执行文件）的输出目录。即app可执行文件就会输出到bin目录中
# EXECUTABLE_OUTPUT_PATH是cmake中已定义的变量，表示目标二进制可执行文件的存放位置
# PROJECT_SOURCE_DIR 是工程根目录的变量
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

```

#### 2.2 命令总结

```sh
# 1、设置变量set(varname value1 value2 ...)
# 设置cpp文件集合的变量，即通过SRC_LIST替代所有cpp文件
set(SRC_LIST main.cpp foo.cpp bar.cpp)
# 增加编译选项，CMAKE_CXX_FLAGS是cmake系统的变量
set(CMAKE_CXX_FLAGS "-O2 -Wall")

# 2、向工程添加多个特定的头文件搜索路径 --->相当于指定g++编译器的-I参数
# 语法： include_directories(dir1 dir2 ...)
include_directories(include1 include2 ...)

# 3、aux_source_directory获取指定目录下的所有cpp资源文件
aux_source_directory(src SRC_LIST)   # 获取src目录下的cpp资源文件，将其用变量SRC_LIST替代

# 4、添加编译选项，相当于-std=c++11 -g -Wall等
add_compile_options(-Wall -g -std=c++11)
```



### 3、静态库和动态库的生成

#### 3.1 项目结构

![06_cmake](images\06_cmake.png)

```sh
# cmake配置

# 1、设置对cmake的最小版本
cmake_minimum_required(VERSION 3.0)

# 2、设置工程名
project(apply VERSION 12.0 DESCRIPTION "这是一个关于库的工程")

# 3、指定需要的头文件目录
include_directories(include)

# 4、获取指定目录下的所有cpp文件，用SRC_LIST变量代替
aux_source_directory(src SRC_LIST)

# 5、生成静态库文件和动态库文件，这里的所有cpp文件用变量替代
add_library(kiki_so SHARED ${SRC_LIST})
add_library(kiki_a STATIC ${SRC_LIST})

# 设置输出对象的名称
set_target_properties (kiki_so PROPERTIES OUTPUT_NAME "kiki")
set_target_properties (kiki_a PROPERTIES OUTPUT_NAME "kiki")

# 6、设置库的输出目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
```

输出结果如下：

![07_cmake](images\07_cmake.png)

#### 3.2 总结命令

```sh
# 1、生成库文件（包括动态库和静态库）
add_library(libname [SHARED|STATIC] source1 source2 ... sourceN) 

# 2、设置库的输出目录
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# EXECUTABLE_OUTPUT_PATH 变量是可执行文件的输出目录变量
# LIBRARY_OUTPUT_PATH 变量是动态库静态库的输出目录变量

# 3、用于设置一个目标的属性。这个命令通常用于设置生成的可执行文件、静态库或动态库的一些属性。
set_target_properties(
    <target> 
    PROPERTIES 
    <prop1> <value1> 
    <prop2> <value2>
    ...
) 
# 所以，第一个target通常是add_libary和add_executable所生成的对象。OUTPUT_NAME属性表示输出的文件名
# 这个命令通常设置的属性有：
* OUTPUT_NAME：指定生成的目标的名称。默认情况下，生成的目标的名称与目标的名称相同，但是可以通过 OUTPUT_NAME 属性来自定义名称。
* LIBRARY_OUTPUT_DIRECTORY：指定生成的动态库的输出目录。默认情况下，生成的动态库将被放置在构建目录下，但是可以通过 LIBRARY_OUTPUT_DIRECTORY 属性来自定义输出目录。
设置这个属性类似于set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)的效果，但是这个会将所有目标都输出到指定的目录中；如果通过这个属性设置，可以设置指定的目标到指定的目录中。如上面的两个库，可以将其中一个动态库对象输出到其他目录中。不可以输出改变静态库的输出目录。
* ARCHIVE_OUTPUT_DIRECTORY：指定生成的静态库的输出目录。同LIBRARY_OUTPUT_DIRECTORY属性的功能。
* RUNTIME_OUTPUT_DIRECTORY：指定生成的可执行文件的输出目录。set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)功能类似。
```



### 4、使用静态库和动态库

#### 4.1 项目结构

![08_cmake](images\08_cmake.png)

```sh
# 使用动态库静态库

# 设置项目对cmake的最低要求
cmake_minimum_required(VERSION 3.25)

# 设置工程名以及版本
project(AppSO VERSION 2.1)

# 设置头文件变量，其包含所有需要的头文件。
# 第一个目录是本项目的，第二个是动态库的头文件目录，第三个是静态库头文件目录
set(headers include ../3.29.3/include ../3.29.5/include)

# 设置头文件变量到项目中
include_directories(${headers})

# 设置编译选项
add_compile_options(-Wall -std=c++11 -g)

# 设置资源变量
set(cpps src/program.cpp src/main.cpp)

# 生成指定的可执行程序
add_executable(apply ${cpps})

# 情况一：libtps.so放在/usr/lib目录中
# 找到动态库，这里是已经把libtps.so放到了默认索的路径中
# find_library(TPS_LIB_VAR tps) 
# 添加动态库连接相当于g++ -l
# target_link_libraries(app ${TPS_LIB_VAR})

# 情况二：是不将动态libtps.so放到指定默认的搜索目录中
# add_library命令创建一个名为tps的目标，表示一个动态库。IMPORTED选项告诉CMake这个目标不是通过源代码编译出来的，而是外部已经编译好的目标。
# 然后使用set_target_properties命令设置tps目标的属性，其中IMPORTED_LOCATION属性指定了动态库文件的路径。
# 最后，target_link_libraries命令将动态库链接到app可执行文件中。命令如下：
# add_library(tps_obj SHARED IMPORTED)
# set_target_properties(tps_obj PROPERTIES IMPORTED_LOCATION /home/yoyo/workspaces/cmakes/3.29.3/build/libtps.so)
# target_link_libraries(app tps_obj)

# 情况三：与情况二相同，只是操作方式不同
find_library(TPS_LIB_VAR_ tps HINTS /home/yoyo/workspaces/cmakes/3.29.3/build/)
# 第二个目录是静态库文件（静态库比较简单，直接写就行）
set(libs ${TPS_LIB_VAR_} /home/yoyo/workspaces/cmakes/3.29.5/build/libstps.a)

# 连接所有的库
target_link_libraries(apply ${libs})

# 设置可执行程序的输出目录
set_target_properties(apply PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "../bin"
)
```

#### 4.2 总结命令

```sh
# 1、find_library: 在指定目录下查找指定库，并把库的绝对路径存放到变量里，其第一个参数是变量名称，第二个参数是库名称，第三个参数是HINTS，第4个参数是路径。表示在指定的目录下找对应库名称的库，用第一个参数变量名称保存。如果不写第三个和第四个参数就默认在系统默认的目录中搜索库。

# 2、target_link_libraries：链接库，第一个参数是可执行程序目标对象，第二个参数是库。
```



## VSCode调试运行

### VSCode调试配置

![01_vscode](images\01_vscode.png)

```json
#### launch.json文件配置
{
    // 使用 IntelliSense 了解相关属性。 
    // 悬停以查看现有属性的描述。
    // 欲了解更多信息，请访问: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "g++ - 生成和调试活动文件",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/bin/apply",  ### 配置可执行文件
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "/usr/bin/gdb",
            "preLaunchTask": "Build",       ### 还需要配置任务json文件，配置这个可以避免修改源码之后需要手动make重新生成。这个Build是task.json文件中一个label
        }
    ]
}
```

```sh
#### tasks.json文件配置
{
    "version": "2.0.0",
    "options": {
        "cwd": "${workspaceFolder}/build"
    },
    "tasks": [
        {
            "type": "shell",
            "label": "cmake",
            "command": "cmake",
            "args": [
                ".."
            ]
        },
        {
            "label": "make",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "command": "make",
            "args": []
        },
        {
            "label": "Build",   ### launch.json中选择了这个
            "dependsOrder": "sequence",
            "dependsOn": [
                "cmake",
                "make"
            ]
        }
    ]
}
```

![02_vscode](images\02_vscode.png)

![03_vscode](images\03_vscode.png)