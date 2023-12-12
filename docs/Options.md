### Options

The options of CppGuard can be set via environment variable **CPPGUARD** or via function *cppguard_set_options*():

```
set via environment variables:
C:\dev\cppguard\out\build\x64-Debug>set CPPGUARD=halt_on_error:1,outputdebugstring:0 & test_target.exe

set via function:
std::string  options = "log:C:/tmp/log.txt,halt_on_error:1,fail_process_in_case_of_error:1,outputdebugstring:0,";
options += "thread_watchdog:1,thread_watchdog_max_duration_sec:100,exit_code:42";

char error_text[1024]{};
const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
assert(ok);
```



##### Option list:

| Option                           | Value range                                              | Comment                                                      |
| -------------------------------- | -------------------------------------------------------- | ------------------------------------------------------------ |
| halt_on_error                    | 0\|1, default: 0                                         | 0: continue<br />1: Terminate process if potential deadlock has been found. Stop on breakpoint if run inside debugger. |
| fail_process_in_case_of_error    | 0\|1,  default: 0                                        | 0: normal exit with exit code of main()<br />1: if error occurred during runtime: fail process with exit 66 |
| thread_watchdog                  | 0\|1,  default: 1                                        | 0: disabled<br />1: observe time of locks spend before acquired and inside a lock<br /> |
| thread_watchdog_max_duration_sec | default: 30 sec, if a debugger is attached: min 120 sec  | max duration a lock can spend waiting before a lock plus time spend inside a lock |
| exit_code                        | 1..126, default: 66                                      | Process exit code in case of an error. Only used if halt_on_error:1 or fail_process_in_case_of_error:1 |
| outputdebugstring                | 0\|1,  default: 1                                        | 0: disabled<br />1: log text will be added to the Visual Studio output windows |
| log                              | disable\|stdout\|stderr\|pathtofile.txt, default: stdout | disable: no log<br />stdout: log to stdout<br />stderr: log to stderr<br />pathtofile.txt: Log to file. Do not quote path even if it contains spaces |



### Public APIs:

```
  /**
   * \brief Set options during runtime
   * \param p_options text containing options, key:value pairs seperated by comma 
   * \param p_error_text: error text in case of error
   * \param p_error_text_size: max size of error text
   * \return 
  */
  int cppguard_set_options(const char* p_options, char* p_error_text, int p_error_text_size);
  
  /**
  * \brief: increase ignore counter for this thread
  * \param p_lock_counter: lock counter is increased by this value
  * \param p_unlocks_counter: unlock counter is increased by this value
  */
  void cppguard_increase_ignore_counter(const unsigned int p_lock_counter, const unsigned int p_unlocks_counter);
  
  Examples A:
  
  #ifdef CPPGUARD_ENABLED
  cppguard_increase_ignore_counter(2, 2);
  #endif
  
  EnterCriticalSection(CriticalSectionB);
  EnterCriticalSection(CriticalSectionA);
  ...
  LeaveCriticalSection(CriticalSectionA);
  LeaveCriticalSection(CriticalSectionB);
   
  Examples B: 
   
  #ifdef CPPGUARD_ENABLED
  cppguard_increase_ignore_counter(1, 1);
  #endif
  std::mutex m_lock;
  const std::lock_guard < std::mutex> lock(m_lock);

```



### Compile time options:

| Define                        | Value range | Comment                                                      |
| ----------------------------- | ----------- | ------------------------------------------------------------ |
| CPPGUARD_MAX_LOCKS_PER_THREAD | default: 20 | Max number of locks a thread can acquire at the same time. Higher values will increase memory usage. |
