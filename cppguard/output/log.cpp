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

#include "log.hpp"

#include <mutex>
#include <iostream>
#include <array>
#include <fstream>
#include <ios>

#include "run_time_options.hpp"
#include <cs_mutex.hpp>
#include <spinlock.hpp>

namespace logger
{
void log_msg(fmt::memory_buffer& p_log_buffer, const runtime_options_t& p_runtime_options)
{
  constexpr std::array<char, 1> null_byte{};
  p_log_buffer.append(null_byte);

  static spinlock_t log_mutex;
  std::lock_guard<spinlock_t> lock(log_mutex);

  bool is_first_call=false;
  static bool first_call=true;
  if(first_call)
  {
    first_call=false;
    is_first_call=true;
  }

#ifdef _WIN32
  if(p_runtime_options.m_use_outputdebugstring)
  {
    OutputDebugString(p_log_buffer.data());
  }
#endif

  switch(static_cast<log_type_t>(p_runtime_options.m_log_type.load()))
  {
  case log_type_t::disabled: break;
  case log_type_t::to_stdout: std::cout<<p_log_buffer.data()<<std::flush; break;
  case log_type_t::to_stderr: std::cerr<<p_log_buffer.data()<<std::flush; break;
  case log_type_t::to_file:
  {
    std::ofstream file;
    if(is_first_call)
    {
      file.open(p_runtime_options.m_log_file);
    } else
    {
      file.open(p_runtime_options.m_log_file, std::ios_base::app);
    }

    file.write(p_log_buffer.data(), static_cast<std::streamsize>(p_log_buffer.size()));

    file.close();

  } break;

  }

}
}
