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

#include "options_parser.hpp"

#include <algorithm>

#include "pystring.h"
#include "fmt/format.h"
#include "fmt/std.h"

void parse_options(const char* p_options_ptr, runtime_options_t& p_runtime_options)
{
  const std::vector<std::string> g_know_keys{"log", "outputdebugstring", "halt_on_error", "fail_process_in_case_of_error", "thread_watchdog", "thread_watchdog_max_duration_sec", "exit_code"};
  const std::vector<std::string> g_true_false{"1", "0"};

  if(p_options_ptr==nullptr||p_options_ptr[0]=='\0')
  {
    throw std::invalid_argument(fmt::format("p_options is null or empty"));
  }

  const std::string p_options=p_options_ptr;

  const std::vector<std::string> key_value_strings_sub_parts=pystring::split(pystring::lower(p_options), ",");
  if(key_value_strings_sub_parts.empty())
  {
    throw std::invalid_argument(fmt::format("No key=value pairs found"));
  }

  // Split each substring by equals
  for(const auto& substring:key_value_strings_sub_parts)
  {
    const auto key_value_pair=pystring::partition(substring, ":");
    if(key_value_pair[1]!=":")
    {
      throw std::invalid_argument(fmt::format("No valid key-value pair: '{}'", substring));
    }

    const std::string key=pystring::strip(key_value_pair[0]);
    if(key.empty())
    {
      throw std::invalid_argument(fmt::format("Option is empty for key-value pair: {}", substring));
    }

    if(std::find(g_know_keys.begin(), g_know_keys.end(), key)==g_know_keys.end())
    {
      throw std::invalid_argument(fmt::format("Option '{}' is not known", key));
    }

    const std::string value=pystring::strip(key_value_pair[2]);
    if(value.empty())
    {
      throw std::invalid_argument(fmt::format("Value from key-value pair '{}' is empty", substring));
    }

    if(key=="log")
    {
      if(value=="disable")
      {
        p_runtime_options.m_log_type=static_cast<uint8_t>(log_type_t::disabled);

      } else if(value=="stdout")
      {
        p_runtime_options.m_log_type=static_cast<uint8_t>(log_type_t::to_stdout);

      } else if(value=="stderr")
      {
        p_runtime_options.m_log_type=static_cast<uint8_t>(log_type_t::to_stderr);
      } else
      {
        p_runtime_options.m_log_type=static_cast<uint8_t>(log_type_t::to_file);
        p_runtime_options.m_log_file=value;
      }

    } else if(key=="halt_on_error")
    {
      if(std::find(g_true_false.begin(), g_true_false.end(), value)==g_true_false.end())
      {
        throw std::invalid_argument(fmt::format("Option '{}' requires value 1/0 but got '{}'", key, value));
      }
      p_runtime_options.m_halt_on_error=(value=="1");
    } else if(key=="fail_process_in_case_of_error")
    {
      if(std::find(g_true_false.begin(), g_true_false.end(), value)==g_true_false.end())
      {
        throw std::invalid_argument(fmt::format("Option '{}' requires value 1/0 but got '{}'", key, value));
      }
      p_runtime_options.m_override_process_exit_code_on_error=(value=="1");
    } else if(key=="outputdebugstring")
    {
      if(std::find(g_true_false.begin(), g_true_false.end(), value)==g_true_false.end())
      {
        throw std::invalid_argument(fmt::format("Option '{}' requires value 1/0 but got '{}'", key, value));
      }
      p_runtime_options.m_use_outputdebugstring=(value=="1");
    } else if(key=="thread_watchdog")
    {
      if(std::find(g_true_false.begin(), g_true_false.end(), value)==g_true_false.end())
      {
        throw std::invalid_argument(fmt::format("Option '{}' requires value 1/0 but got '{}'", key, value));
      }
      p_runtime_options.m_thread_watchdog_enabled=(value=="1");
    } else if(key=="thread_watchdog_max_duration_sec")
    {
      if(!pystring::isdigit(value))
      {
        throw std::invalid_argument(fmt::format("Value '{}' for option '{}' is not number", value, key));
      }
      const int as_int_sec=std::stoi(value);
      const bool in_range=as_int_sec>1&&as_int_sec<=24*60*60;
      if(!in_range)
      {
        throw std::invalid_argument(fmt::format("Option '{}' requires value 2..{} but got '{}'", key, 24*60*60, value));
      }

      p_runtime_options.m_thread_watchdog_max_duration_sec=as_int_sec;
    } else if(key=="exit_code")
    {
      if(!pystring::isdigit(value))
      {
        throw std::invalid_argument(fmt::format("Value '{}' for option '{}' is not number", value, key));
      }
      const int as_int=std::stoi(value);
      const bool in_range=as_int>=0&&as_int<127;
      if(!in_range)
      {
        throw std::invalid_argument(fmt::format("Option '{}' requires value 1..{} but got '{}'", key, 126, value));
      }

      p_runtime_options.m_exit_code=as_int;
    }

  }

}

