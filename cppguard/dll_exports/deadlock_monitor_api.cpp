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

#include <deadlock_monitor.hpp>

#include <cassert>
#include <cstring>
#include "attributes.hpp"
#include "cppguard.h"
#include "options_parser.hpp"
#include "fmt/format.h"
#include "fmt/std.h"

extern runtime_options_t g_runtime_options;
extern deadlock_monitor_t* g_deadlock_monitor;

TM_ATTRIBUTE_VISIBILITY int cppguard_set_options(const char* p_options, char* p_error_text, int p_error_text_size)
{
  if(p_error_text==nullptr||p_error_text_size<50)
  {
    assert(false);
    return 0;
  }

  try
  {
    parse_options(p_options, g_runtime_options);
    return 1;
  } catch(std::invalid_argument& p_ex)
  {
    memset(p_error_text, 0, p_error_text_size);
    fmt::format_to_n(p_error_text, p_error_text_size-1, "Invalid argument: '{}'", p_ex.what());
  }

  return 0;
}

TM_ATTRIBUTE_VISIBILITY void cppguard_increase_ignore_counter(const unsigned int p_lock_counter, const unsigned int p_unlocks_counter)
{
  deadlock_monitor_t::increase_ignore_counter(p_lock_counter, p_unlocks_counter);
}

