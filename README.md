**CppGuard is now part of https://github.com/JochenBaier/BugInsight**

### Purpose

**CppGuard** is developer tool for **testing** C++  applications made with Visual Studio **for deadlocks and wrong lock usage.** I can be used manually and within a CI system. CppGuard consists of the DLL *cppguard.dll* and the header *cppguard.h*.

CppGuard uses the [deadlock detection algorithm](https://abseil.io/docs/cpp/guides/synchronization#deadlock-detection) from C++ library Abseil. This algorithm will detect a potential deadlock even if the executed program does not actually deadlock. It maintains an acquired-before graph and checks for cycles in this graph. The underlying graph algorithm is described [here](https://whileydave.com/2020/12/19/dynamic-cycle-detection-for-lock-ordering). There is no relationship between the Abseil project and CppGuard.  

##### Advantages:

- Test applications that cannot be tested on Linux with ThreadSanitizer
- ThreadSanitizer often cannot cover 100% as some program parts are not portable: CppGuard covers this gap
- Test with the Windows operating system on which the program will later run
- Test applications that run too slowly with other Windows OS deadlock test tools
- Test C/C++ DLLs loaded in a Java program ( Java Native Interfaces)
- Test C/C++ DLLs loaded in a .Net program

###### Example deadlock detection:

 If locks are acquired in inconsistent order, it depends on the timing whether a deadlock occur or not. Example with 2 threads and mutex a and b:  

```
std::mutex a;
std::mutex b;

//lock order: a -> b
std::thread t1([&]() {
  const std::lock_guard<std::mutex> lock_a(a);
  const std::lock_guard<std::mutex> lock_b(b);
});

//lock order: b -> a
std::thread t2([&]() {
  const std::lock_guard<std::mutex> lock_b(b);
  const std::lock_guard<std::mutex> lock_a(a);
});

t1.join();
t2.join();
```

If t2 will acquired lock b after t1 has locked a and b no deadlock occur.
if t1 got lock a and t2 got lock b a deadlock occur.


CppGuard has detected the potential deadlock:

```
==========================================================================================================
CppGuard: [Error] Potential deadlock at lock: 0x1e2599ebd0, thread ID: 50092.
Lock Call Stack:
C:\dev\cppguard\cppguard\deadlock_monitor.cpp(152,0): deadlock_monitor_t::dlc_deadlock_check_before_lock
C:\dev\cppguard\cppguard\api_monitors\mutex_monitor.cpp(37,0): cppguard_mtx_lock
...

A cycle in the historical lock ordering graph has been observed:

Thread ID: 50092
Holding lock: 0x1e2599ec40
Wait for lock: 0x1e2599ebd0
Call Stack:
C:\dev\cppguard\cppguard\api_monitors\mutex_monitor.cpp(37,0): cppguard_mtx_lock
C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\include\mutex(56,0): std::_Mutex_base::lock
...

Thread ID: 48624
Holding lock: 0x1e2599ebd0
Wait for lock: 0x1e2599ec40
Call Stack:
C:\dev\cppguard\cppguard\api_monitors\mutex_monitor.cpp(37,0): cppguard_mtx_lock
C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\include\mutex(56,0): std::_Mutex_base::lock
...
```

### Features:

-  Deadlock detection:  A warning or an error is generated if potential deadlock (a cycle) is detected (from Abseil algorithm)

-  Detect wrong lock usage: Detect locks which are released by threads which do no hold them (from Abseil algorithm)
-  Lock watchdog: The time a lock is waiting for a lock or holding a lock is monitored and a log or error is created if a maximum time is exceeded
-  Runs on Windows OS with the native lock APIs of Windows:  Critical Section, std::mutex and std::recursive_mutex
-  Test C/C++ DLLs loaded in a Java or a .Net  programs
-  Overhead depends on usage. Pretty good for IO bound applications (e.g.  TCPI/IP based)
-  Works with Debug, Release and RelWithDebInfo builds
-  For basic usage no source code changes are needed. Minor changes (minimum 2 lines) to the build system (CMake, or Visual Studio Project) are needed 

### Quickstart and building

see [Quickstart](docs/Quickstart.md)

### Technical background:

CppGuard provides:

- cppguard.dll: This DLL and has to be linked to the project to test (import library: cppguard.lib)
- [cppguard.h](cppguard/include/cppguard.h): This header file is automatically included in all source files using Visual Studio option [Forced Include File](https://learn.microsoft.com/en-us/cpp/build/reference/fi-name-forced-include-file?view=msvc-170).
  Macros like *#define EnterCriticalSection(x) cppguard_enter_critical_section(x)* will intercept the lock API call

### Supported Lock APIs:

-  InitializeCriticalSection(), EnterCriticalSection(), TryEnterCriticalSection(), LeaveCriticalSection(), DeleteCriticalSection()

- std::mutex, std::recursive_mutex: via internal functions from mutex header_Mtx_*()
- std::lock_guard, std::unique_lock

### Not supported Lock APIs:

- Slim Reader/Writer (SRW) Locks: no DeleteSRWLock() provided, needed to track delete of lock

### Not yet tested:

- std::scoped_lock, std::shared_mutex, std::timed_mutex, std::recursive_timed_mutex std::shared_timed_mutex

### Options

see [Options](docs/Options.md)

### Used Third-party software

see [Third-party software](docs/Third_party_software.md)

### License

CppGuard is licensed under the terms of the Apache 2.0 license. See LICENSE for more information.

