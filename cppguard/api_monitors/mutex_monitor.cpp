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

extern deadlock_monitor_t* g_deadlock_monitor;
extern statistics_t g_statistics;

extern "C" int __cdecl cppguard_mtx_lock(_Mtx_t t)
{
  void* hold_locks=nullptr;
  assert(g_deadlock_monitor);

  uint64_t id=0;
  g_deadlock_monitor->dlc_deadlock_check_before_lock(t, &hold_locks, id);
  if(id==0)
  {
    return static_cast<int>(_Mtx_lock(t));
  }

  const int res=static_cast<int>(_Mtx_lock(t));
  g_deadlock_monitor->dlc_deadlock_check_in_lock(t, id, hold_locks);

  return res;
}

extern "C"  int __cdecl cppguard_mtx_unlock(_Mtx_t t)
{
  g_deadlock_monitor->dlc_deadlock_check_after_lock(t);
  return static_cast<int>(_Mtx_unlock(t));
}

extern "C"  int __cdecl cppguard_mtx_trylock(_Mtx_t t)
{
  void* hold_locks=nullptr;
  uint64_t id=0;
  g_deadlock_monitor->dlc_deadlock_check_before_try_lock(t, &hold_locks, id);
  if(id==0)
  {
    assert(false);
    return static_cast<int>(_Mtx_trylock(t));
  }

  const int res=static_cast<int>(_Mtx_trylock(t));
  if(res!=0)
  {
    return res;
  }

  g_deadlock_monitor->dlc_deadlock_check_in_lock(t, id, hold_locks);
  return res;
}

extern "C" void __cdecl cppguard_init_in_situ(_Mtx_t t, int i)
{
  _Mtx_init_in_situ(t, i);
}

extern "C" void __cdecl cppguard_mtx_destroy_in_situ(_Mtx_t t)
{
  g_deadlock_monitor->dlc_deadlock_delete_lock(t);
  _Mtx_destroy_in_situ(t);
}

extern "C" int __cdecl cppguard_cnd_wait(_Cnd_t c, _Mtx_t t)
{
  g_deadlock_monitor->cppguard_cnd_wait(t);
  return static_cast<int>(_Cnd_wait(c, t));
}

#if _MSC_VER>=1938 //VS 2022, v143
extern "C" int __cdecl cppguard_cnd_timedwait(_Cnd_t c, _Mtx_t t, const _timespec64*d)
#else
extern "C" int __cdecl cppguard_cnd_timedwait(_Cnd_t c, _Mtx_t t, const xtime*d)
#endif
{
  g_deadlock_monitor->cppguard_cnd_wait(t);
  return static_cast<int>(_Cnd_timedwait(c, t, d));
}
