# CppGuard Quickstart

##### Usage/**TL;DR**

1. Add CppGuard to an existing CMake project:

```
add_subdirectory(path_to/cppguard/cppguard) or
add_subdirectory(path_to/cppguard/cppguard cppguard) #with binary_dir 'cppguard'
```

2. Add 'cppguard' as a dependency to all targets you want to test: 

```
target_link_libraries(target_to_test cppguard::cppguard cppguard::auto_include)
```

3.  Copy cppguard.dll after build to output directory

------

##### Prerequisites

- The target to test: Executable or DLL with access to source code created with Visual Studio 2019 or 2022 in C or C++, all C++ versions supported.
  
- C-projects: C++ support has to be enabled:  project(project_name C **CXX**)
  
- To compile CppGuard: 
  - Visual Studio 2019 or 2022 with C++14 support 
  - CMake >=3.15

- Use same compiler and platform toolset for CppGuard and the test target
- git to download sources (optional)

##### CppGuard consists of

- cppguard.dll: needs to be linked to test target (link cppguard.lib)
- cppguard.hpp: needs to be included in all sources which contains locks, this is automated with '[/FI](https://learn.microsoft.com/en-us/cpp/build/reference/fi-name-forced-include-file?view=msvc-170)' compile option 

##### Getting CppGuard

Clone source with git:

```
git clone https://github.com/JochenBaier/cppguard.git
```

or download as zip from GitHub repo

##### Build and run example

- Open the main folder 'cppguard' in Visual Studio via "Open a local folder"
- select 'quickstart_example' as startup item
- run 'quickstart_example'

Alternatively generate a VS solution with CMake:

```
cd cppguard
mkdir build
cd build
cmake ..
open *.sln from build folder with Visual Studio
```

##### Deadlock detection

The example 'quickstart_example' executes the dining philosophers problem. CppGuard has detected the deadlock. The code will still deadlock. The program has to be stopped manually.

```
=================
CppGuard loaded
=================
Philosopher 0 picked up left fork
Philosopher 3 picked up left fork
Philosopher 4 picked up left fork
Philosopher 1 picked up left fork
Philosopher 2 picked up left fork

==========================================================================================================
CppGuard: [Error] Potential deadlock at lock: 0x7ff75d0824c0, thread ID: 19784.
Lock Call Stack:
C:\dev\cppguard\cppguard\deadlock_monitor.cpp(148,0): deadlock_monitor_t::dlc_deadlock_check_before_lock
C:\dev\cppguard\cppguard\api_monitors\critical_section_monitor.cpp(96,0): cppguard_enter_critical_section
C:\dev\cppguard\quickstart_example\main.cpp(47,0): philosopher
...

A cycle in the historical lock ordering graph has been observed:

Thread ID: 19784
Holding lock: 0x7ff75d082498
Wait for lock: 0x7ff75d0824c0
Call Stack:
C:\dev\cppguard\cppguard\api_monitors\critical_section_monitor.cpp(96,0): cppguard_enter_critical_section
C:\dev\cppguard\quickstart_example\main.cpp(47,0): philosopher
...

Thread ID: 10272
Holding lock: 0x7ff75d0824c0
Wait for lock: 0x7ff75d0824e8
Call Stack:
C:\dev\cppguard\cppguard\api_monitors\critical_section_monitor.cpp(96,0): cppguard_enter_critical_section
C:\dev\cppguard\quickstart_example\main.cpp(47,0): philosopher
...
```

*Hint: the Call Stack is also printed to the Visual Studio output window: double click on a line to go to the source location*

To configure CppGuard to halt on case of deadlock add this to main.cpp of the example:

```c++
char error_text[1024]{};
int res=cppguard_set_options("halt_on_error:1", error_text, _countof(error_text));
assert(res == 1);
```

Alternatively set the environment variable 'CPPGUARD' with value 'halt_on_error:1' and start quickstart_example.exe on command line:

```
C:\dev\cppguard\out\build\x64-Debug>set CPPGUARD=halt_on_error:1 & quickstart_example.exe
```

Now the example will terminate before the deadlock occur or pause on a breakpoint if run inside a debugger:

```
CppGuard: [Error] Potential deadlock at lock: 0x7ff75d0824c0, thread ID: 19784.
...
CppGuard: Process will be terminated with exit code '66' due to potential deadlock. Use option 'halt_on_error:0' to continue on error
```

##### CMakeLists.txt of a target to test:

The CMakeLists.txt of the example:

```cmake
add_executable(quickstart_example main.cpp) 

#add this line to all targets you want to test
target_link_libraries(quickstart_example cppguard::cppguard cppguard::auto_include)
```

**cppguard::cppguard**: will link cppguard and adds 'cppguard/include' folder to VS 'Additional included directories'.
Use this for Exe or DLL targets. If the exe or DLL target does not contain any locks you can omit "cppguard::auto_include"

**cppguard::auto_include**: sets  '[/FI](https://learn.microsoft.com/en-us/cpp/build/reference/fi-name-forced-include-file?view=msvc-170)' compile option with value 'cppguard.hpp'. This option includes 'cppguard.hpp'
in all source files of the target. Use this option for all targets which contains locks. This header enables CppGuard to monitor the usage of Locks.



##### Usage without add_subdirectory(cppguard)/External build

Build cppguard with CMake outside of your project:

```
cd cppguard/cppguard
mkdir build
cd build
cmake ..
cmake --build .  //debug
```

Set '[/FI](https://learn.microsoft.com/en-us/cpp/build/reference/fi-name-forced-include-file?view=msvc-170)'  compile option for all targets to test:

```
target_compile_options(test_target PRIVATE "/FIpath_to/cppguard/cppguard/include/cppguard.hpp" )
```

Link cppguard:

```
add_library(cppguard STATIC IMPORTED GLOBAL)
set_target_properties(cppguard PROPERTIES IMPORTED_LOCATION_DEBUG "path_to/cppguard/cppguard/build/Debug/cppguard.lib" )
set_target_properties(cppguard PROPERTIES IMPORTED_LOCATION_RELEASE "path_to/cppguard/cppguard/build/Release/cppguard.lib")
set_target_properties(cppguard PROPERTIES IMPORTED_LOCATION_RELWITHDEBINFO  "path_to/cppguard/cppguard/build/RelWithDebInfo/cppguard.lib")

target_link_libraries(test_target cppguard)
```
