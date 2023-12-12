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

#pragma once

#include <atomic>
#include <string>

enum class log_type_t
{
  disabled,
  to_stdout,
  to_stderr,
  to_file
};

struct runtime_options_t
{
  std::string m_log_file;
  std::atomic_uint8_t m_log_type{static_cast<uint8_t>(log_type_t::to_stdout)};
  std::atomic_bool m_use_outputdebugstring{true};

  std::atomic_bool m_halt_on_error{false};
  std::atomic_bool m_override_process_exit_code_on_error{false};
  std::atomic_bool m_thread_watchdog_enabled{true};
  std::atomic_uint32_t m_thread_watchdog_max_duration_sec{30};
  std::atomic_int m_exit_code={66};
};

struct statistics_t
{
  std::atomic_uint64_t m_initial_critial_section_counter{0};
  std::atomic_uint64_t m_delete_critial_section_counter{0};
};
