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

#include "cppguard.h"
#include <cassert>
#include <iostream>

#include <Windows.h>
#include "options_parser.hpp"
#include <deadlock_monitor.hpp>


extern runtime_options_t g_runtime_options;

extern deadlock_monitor_t* g_deadlock_monitor;

int dlm_deadlock_monitor_set_options(const char* const p_options)
{
  if (p_options==nullptr)
  {
    return false;
  }
  
  return parse_options(p_options, g_runtime_options);
}

void dlm_deadlock_monitor_increase_ignore_counter(const unsigned int p_lock_counter, const unsigned int p_unlocks_counter)
{
  assert(g_deadlock_monitor);
  g_deadlock_monitor->increase_ignore_counter(p_lock_counter, p_unlocks_counter);
}

