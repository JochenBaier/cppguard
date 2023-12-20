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

#ifdef _WIN32
#include <Windows.h>
#else
#include <csignal>
#endif

#include <cstdio>
#include <deadlock_monitor.hpp>
#include <cassert>
#include "options_parser.hpp"
#include <stacktrace.h>

#include "log.hpp"
#include "log_helper.hpp"

#include "fmt/format.h"
#include "fmt/std.h"
#include "debugbreak.h"

runtime_options_t g_runtime_options;
statistics_t g_statistics;
deadlock_monitor_t* g_deadlock_monitor=nullptr;

void set_options_from_env_variable(const std::string& p_env_var_name)
{
#ifdef _WIN32
  std::array<char, 2048> env_buffer{};
  size_t requiredSize=0;
  const errno_t res=getenv_s(&requiredSize, nullptr, 0, p_env_var_name.c_str());
  if(res!=0||requiredSize==0)
  {
    return;
  }

  if(requiredSize>env_buffer.size())
  {
    assert(false);
    return;
  }

  getenv_s(&requiredSize, env_buffer.data(), requiredSize, p_env_var_name.c_str());

  printf("CppGuard: Env var '%s' variable is: '%s'\n", p_env_var_name.c_str(), env_buffer.data());

  parse_options(env_buffer.data(), g_runtime_options);
#else
  char* env_content=std::getenv(p_env_var_name.c_str());
  if(env_content==nullptr)
  {
    return;
  }
  parse_options(env_content, g_runtime_options);

#endif
}

struct loader_t
{
  loader_t()
  {
    set_options_from_env_variable("CPPGUARD");

    InitializeSymbolizer();

    g_deadlock_monitor=new deadlock_monitor_t(g_runtime_options);

    if(g_runtime_options.m_log_type.load()!=static_cast<uint8_t>(log_type_t::disabled))
    {
      auto out=fmt::memory_buffer();
      fmt::format_to(std::back_inserter(out), "========================================\n");
      fmt::format_to(std::back_inserter(out), "CppGuard loaded [version: '{}.{}.{}.{}']\n", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, VERSION_REVISION);
      fmt::format_to(std::back_inserter(out), "========================================\n");


      log_helper::create_options_log_text(g_runtime_options, out);

      logger::log_msg(out, g_runtime_options);


    }
  }

  ~loader_t()
  {
    assert(g_deadlock_monitor);

    const bool potential_deadlocks_found=g_deadlock_monitor->potential_deadlocks_found();
    const bool other_errros_found=g_deadlock_monitor->other_errors_found();
    delete g_deadlock_monitor;

    if(g_runtime_options.m_log_type.load()!=static_cast<uint8_t>(log_type_t::disabled))
    {
      auto summary=fmt::memory_buffer();
      log_helper::create_summary(g_statistics, potential_deadlocks_found, other_errros_found, summary);
      logger::log_msg(summary, g_runtime_options);
    }

    const bool abort=potential_deadlocks_found||other_errros_found||(g_statistics.m_initial_critial_section_counter!=g_statistics.m_delete_critial_section_counter);

    if(abort&&g_runtime_options.m_override_process_exit_code_on_error)
    {
#ifdef _WIN32
      if(IsDebuggerPresent())
#endif
      {
        debug_break();
      }

      if(g_runtime_options.m_log_type.load()!=static_cast<uint8_t>(log_type_t::disabled))
      {
        auto out=fmt::memory_buffer();
        fmt::format_to(std::back_inserter(out), "CppGuard: Terminate process with exit code {}\n", g_runtime_options.m_exit_code);
        logger::log_msg(out, g_runtime_options);
      }

#ifdef _WIN32
      TerminateProcess(GetCurrentProcess(), g_runtime_options.m_exit_code);
#else
      kill(getpid(), SIGKILL);
#endif
    }

  }
};

loader_t loader;
