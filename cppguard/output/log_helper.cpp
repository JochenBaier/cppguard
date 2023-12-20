// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License.  

#include "log_helper.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <csignal>
#endif

#include <algorithm>
#include <cassert>

#include "fmt/std.h"
#include <synch_locks_held.hpp>
#include <graphcycles.h>

#include "debugbreak.h"
#include "log.hpp"
#include "stacktrace.h"
#include "run_time_options.hpp"

namespace log_helper
{
void create_options_log_text(const runtime_options_t& p_runtime_options, fmt::memory_buffer& p_out_buffer)
{
  fmt::format_to(std::back_inserter(p_out_buffer), "\n");
  fmt::format_to(std::back_inserter(p_out_buffer), "Used CppGuard options:\n");
  fmt::format_to(std::back_inserter(p_out_buffer), "  halt_on_error: {}\n", p_runtime_options.m_halt_on_error ? "1" : "0");
  fmt::format_to(std::back_inserter(p_out_buffer), "  fail_process_in_case_of_error: {}\n", p_runtime_options.m_override_process_exit_code_on_error ? "1" : "0");
  fmt::format_to(std::back_inserter(p_out_buffer), "  thread_watchdog: {}\n", p_runtime_options.m_thread_watchdog_enabled ? "1" : "0");
  fmt::format_to(std::back_inserter(p_out_buffer), "  thread_watchdog_max_duration_sec: {}\n", p_runtime_options.m_thread_watchdog_max_duration_sec);
  fmt::format_to(std::back_inserter(p_out_buffer), "  exit_code: {}\n", p_runtime_options.m_exit_code);

  std::string log_option;

  switch(static_cast<log_type_t>(p_runtime_options.m_log_type.load()))
  {
  case log_type_t::disabled: log_option="disabled"; break;
  case log_type_t::to_stderr: log_option="stderr"; break;
  case log_type_t::to_stdout: log_option="stdout"; break;
  case log_type_t::to_file: log_option=p_runtime_options.m_log_file; break;
  }

  fmt::format_to(std::back_inserter(p_out_buffer), "  log: {}\n\n", log_option);
}


void create_summary(statistics_t& p_statistics, const bool& p_found_deadlocks, const bool& p_other_errors_found, fmt::memory_buffer& p_summary)
{
  const bool critical_section_error=p_statistics.m_initial_critial_section_counter!=p_statistics.m_delete_critial_section_counter;

  fmt::format_to(std::back_inserter(p_summary), "\n\n");
  fmt::format_to(std::back_inserter(p_summary), "==================\n");
  fmt::format_to(std::back_inserter(p_summary), "CppGuard summary:\n");
  fmt::format_to(std::back_inserter(p_summary), "==================\n");
  fmt::format_to(std::back_inserter(p_summary), "{} Potential deadlocks found:  {}\n", p_found_deadlocks ? "[Error]" : "[Ok]   ", p_found_deadlocks ? "Yes" : "No");
  fmt::format_to(std::back_inserter(p_summary), "{} Errors found:               {}\n", p_other_errors_found ? "[Error]" : "[Ok]   ", p_other_errors_found ? "Yes" : "No");
  fmt::format_to(std::back_inserter(p_summary), "{} CriticalSection statistics: Initialized: {}, Deleted: {}\n", critical_section_error ? "[Error]" : "[Ok]   ",
    p_statistics.m_initial_critial_section_counter, p_statistics.m_delete_critial_section_counter);
}

std::string construct_hold_locks_string(const std::array<lock_t, MAX_LOCKS_PER_THREAD>& hold_locks, const uint8_t& hold_locks_count)
{
  std::string hold_locks_info;
  for(size_t k=0; k<(std::min)(hold_locks.size(), static_cast<size_t>(hold_locks_count)); ++k)
  {
    hold_locks_info+=fmt::format("{}{}", k==0 ? "" : ", ", hold_locks[k].mu);
  }

  return hold_locks_info;
}

void build_deadlock_circle_log_text(const GraphCycles& p_deadlock_graph, void* mu, const uint64_t& mu_id, const uint64_t& other_node_id, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "CppGuard: [Error] Potential deadlock at lock: {}, thread ID: {}\n", mu, std::this_thread::get_id());
  fmt::format_to(std::back_inserter(p_log_string), "Hint: Go through all Call Stacks and check which locks are called in inconsistent order\n\n");
  fmt::format_to(std::back_inserter(p_log_string), "Lock Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 2);

  fmt::format_to(std::back_inserter(p_log_string), "\nA cycle in the historical lock ordering graph has been observed:\n");

  std::array<uint64_t, 10> path{0};

  const size_t path_len=static_cast<size_t>(p_deadlock_graph.FindPath(mu_id, other_node_id, (int)path.size(), path.data()));

  for(size_t j=0; j!=path_len&&j!=path.size(); j++)
  {
    const uint64_t& id=path[j];
    void* path_mu=static_cast<void*>(p_deadlock_graph.Ptr(id));
    if(path_mu==nullptr) continue;

    std::thread::id thread_id;
    uint8_t hold_locks_count=0;
    std::array<lock_t, MAX_LOCKS_PER_THREAD> hold_locks;
    p_deadlock_graph.extra_infos(id, thread_id, hold_locks_count, hold_locks);

    const std::string hold_locks_info=construct_hold_locks_string(hold_locks, hold_locks_count);

    void** stack;
    const int depth=p_deadlock_graph.GetStackTrace(id, &stack);

    fmt::format_to(std::back_inserter(p_log_string), "\n\nThread ID: {}\nHolding lock: {}\nWait for lock: {}\nCall Stack:\n", thread_id, hold_locks_info, path_mu);
    printf_call_stack_symbol_infos(stack, depth, p_log_string);

  }
}

void build_delete_locked_lock_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "CppGuard: [Error] The thread with ID '{}' deletes the lock '{}' which the thread probably still holds.\n", p_thread_id, mu);

  fmt::format_to(std::back_inserter(p_log_string), "Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 3);
}

void build_release_lock_which_thread_does_hold_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "CppGuard: [Error] The thread with ID '{}' releasing the lock '{}' which is not hold by this thread.\n", p_thread_id, mu);

  fmt::format_to(std::back_inserter(p_log_string), "Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 4);
}

void build_release_locked_deleted_by_other_thread_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "CppGuard: [Error] The thread with ID '{}' releasing the lock '{}' which was probably already deleted by an other thread.\n", p_thread_id, mu);

  fmt::format_to(std::back_inserter(p_log_string), "Call Stack:\n");
  save_current_callstack_to_string(p_log_string, 3);
}

void build_watchdog_callstack(GraphCycles& p_deadlock_graph, const lock_t& p_lock, fmt::memory_buffer& p_log_string)
{
#ifdef _WIN32
  fmt::format_to(std::back_inserter(p_log_string), "\n\nCall Stack of lock {}:\n", p_lock.mu);
  void** stack;
  const int depth=p_deadlock_graph.GetStackTrace(p_lock.id, &stack);
  assert(depth>0);
  printf_call_stack_symbol_infos(stack, depth, p_log_string);
#else
  UNUSED(p_deadlock_graph);
  UNUSED(p_lock);
  UNUSED(p_log_string);
#endif
}

void build_watchdog_error(const std::thread::id& p_thread_id, const bool& p_in_lock, const uint64_t& p_hold_duration_sec, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");

  if(p_in_lock)
  {
    fmt::format_to(std::back_inserter(p_log_string), "CppGuard: [Error] The thread with ID '{}' holds a lock since {} seconds.\n", p_thread_id, p_hold_duration_sec);
  } else
  {
    //FIXME show callstack of this lock
    fmt::format_to(std::back_inserter(p_log_string), "CppGuard: [Error] The thread with ID '{}' waits to enter a lock since {} seconds.\n", p_thread_id, p_hold_duration_sec);
  }
}

void build_max_locks_per_thread_reached_text(const int& p_lock_count, const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\n\n==========================================================================================================\n");
  fmt::format_to(std::back_inserter(p_log_string), "CppGuard: [Error] The thread with ID '{}' already holds {} locks. Max number of hold locks per thread reached. Adjust cmake variable 'CPPGUARD_MAX_LOCKS_PER_THREAD. 'The lock '{}' will not be ignored.", p_thread_id, p_lock_count, mu);

  fmt::format_to(std::back_inserter(p_log_string), "\nCallstack:\n");
  save_current_callstack_to_string(p_log_string, 3);
}

void process_error(fmt::memory_buffer& p_log_text, const runtime_options_t& p_runtime_options, const bool p_is_deadlock)
{
  if(p_runtime_options.m_log_type!=static_cast<uint8_t>(log_type_t::disabled))
  {
    logger::log_msg(p_log_text, p_runtime_options);
  }

  if(!p_runtime_options.m_halt_on_error)
  {
    return;
  }

  if(p_runtime_options.m_log_type!=static_cast<uint8_t>(log_type_t::disabled))
  {
    auto out=fmt::memory_buffer();
    fmt::format_to(std::back_inserter(out), "\nCppGuard: Process will be terminated with exit code '{}' due to {}. Use option 'halt_on_error:0' to continue on error\n",
      p_runtime_options.m_exit_code.load(), p_is_deadlock ? "potential deadlock" : "error");
    logger::log_msg(out, p_runtime_options);
  }

#ifdef _WIN32 
  if(IsDebuggerPresent())
#endif
  {
    debug_break();
  }

#ifdef _WIN32
  TerminateProcess(GetCurrentProcess(), p_runtime_options.m_exit_code);
#else
  std::quick_exit(p_runtime_options.m_exit_code);
  //kill(getpid(), SIGKILL);
#endif
}
}
