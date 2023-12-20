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

#include <cstdlib>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#include <vector>

const size_t NUM_CS=10;
CRITICAL_SECTION critical_sections_good[NUM_CS]{};

std::mutex mutex_good;

CRITICAL_SECTION mu1_bad;
CRITICAL_SECTION mu2_bad;

void good_test()
{
  for(size_t i=0; i<NUM_CS; ++i)
  {
    EnterCriticalSection(&critical_sections_good[i]);
  }

  mutex_good.lock();

  for(size_t i=0; i<NUM_CS; ++i)
  {
    LeaveCriticalSection(&critical_sections_good[i]);
  }

  mutex_good.unlock();

}


void bad_test()
{
  // mu1_bad => mu2_bad
  EnterCriticalSection(&mu1_bad);
  EnterCriticalSection(&mu2_bad);
  LeaveCriticalSection(&mu2_bad);
  LeaveCriticalSection(&mu1_bad);

  // mu2_bad => mu1_bad
  EnterCriticalSection(&mu2_bad);
  EnterCriticalSection(&mu1_bad);  // <<<<<<< OOPS

  //REQUIRE(false);

  LeaveCriticalSection(&mu1_bad);
  LeaveCriticalSection(&mu2_bad);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  switch(ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  {
    DWORD id=GetCurrentThreadId();
    int brk=1;

    for(size_t i=0; i<NUM_CS; ++i)
    {
      InitializeCriticalSection(&critical_sections_good[i]);
    }

    InitializeCriticalSection(&mu1_bad);
    InitializeCriticalSection(&mu2_bad);



  } break;

  case DLL_PROCESS_DETACH:
  {
    DWORD id=GetCurrentThreadId();
    int brk=1;

    for(size_t i=0; i<NUM_CS; ++i)
    {
      DeleteCriticalSection(&critical_sections_good[i]);
    }

    DeleteCriticalSection(&mu1_bad);
    DeleteCriticalSection(&mu2_bad);

  } break;
  default: break;
  }

  return TRUE;
}
#endif
