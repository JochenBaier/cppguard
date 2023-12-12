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

#include "deadlock_monitor.hpp"

#include <cassert>
#include <cstdint>

#ifdef _WIN32
#include <Windows.h>
#else
#define WINAPI
#include "pthread.h"
#define  LPCRITICAL_SECTION void*

void InitializeCriticalSection(LPCRITICAL_SECTION p_critical_section)
{
  pthread_mutexattr_t Attr;
  pthread_mutexattr_init(&Attr);
  pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init((pthread_mutex_t*)p_critical_section, &Attr);
}

void DeleteCriticalSection(LPCRITICAL_SECTION p_critical_section)
{
  assert(p_critical_section);
  pthread_mutex_destroy((pthread_mutex_t*)p_critical_section);
}

void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
  pthread_mutex_lock((pthread_mutex_t*)lpCriticalSection);
}

void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
  pthread_mutex_unlock((pthread_mutex_t*)lpCriticalSection);
}

bool TryEnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
  return pthread_mutex_trylock((pthread_mutex_t*)lpCriticalSection)==0;
}

#endif

extern deadlock_monitor_t* g_deadlock_monitor;
extern statistics_t g_statistics;

TM_ATTRIBUTE_VISIBILITY extern "C" void WINAPI cppguard_initialize_critical_section(void* lpCriticalSection)
{
  assert(lpCriticalSection);

  InitializeCriticalSection(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
  ++g_statistics.m_initial_critial_section_counter;
}

TM_ATTRIBUTE_VISIBILITY extern "C" void WINAPI cppguard_enter_critical_section(void* lpCriticalSection)
{
  assert(lpCriticalSection);

  void* hold_locks=nullptr;
  assert(g_deadlock_monitor);

  uint64_t id=0;
  g_deadlock_monitor->dlc_deadlock_check_before_lock(lpCriticalSection, &hold_locks, id);
  if(id==0)
  {
    EnterCriticalSection(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
    return;
  }

  EnterCriticalSection(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
  g_deadlock_monitor->dlc_deadlock_check_in_lock(lpCriticalSection, id, hold_locks);
}

TM_ATTRIBUTE_VISIBILITY extern "C" void WINAPI cppguard_leave_critical_section(void* lpCriticalSection)
{
  assert(lpCriticalSection);

  g_deadlock_monitor->dlc_deadlock_check_after_lock(lpCriticalSection);
  LeaveCriticalSection(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
}

TM_ATTRIBUTE_VISIBILITY extern "C" bool WINAPI cppguard_tryenter_critical_section(void* lpCriticalSection)
{
  void* hold_locks=nullptr;
  uint64_t id=0;
  g_deadlock_monitor->dlc_deadlock_check_before_try_lock(lpCriticalSection, &hold_locks, id);
  if(id==0)
  {
    assert(false);
    return TryEnterCriticalSection(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
  }

  const bool ok=TryEnterCriticalSection(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
  if(!ok)
  {
    return ok;
  }

  g_deadlock_monitor->dlc_deadlock_check_in_lock(lpCriticalSection, id, hold_locks);
  return ok;
}

TM_ATTRIBUTE_VISIBILITY extern "C" void WINAPI cppguard_delete_critical_section(void* lpCriticalSection)
{
  assert(lpCriticalSection);
  g_deadlock_monitor->dlc_deadlock_delete_lock(lpCriticalSection);
  DeleteCriticalSection(static_cast<LPCRITICAL_SECTION>(lpCriticalSection));
  ++g_statistics.m_delete_critial_section_counter;
}

