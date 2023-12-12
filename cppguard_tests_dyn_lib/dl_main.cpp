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

#ifdef _WIN32
#include <Windows.h>
#include <vector>

struct loader_t
{
  loader_t()
  {
    if(std::getenv("CPPGUARD_STATIC_TEST_IN_DLL_BEFORE_MAIN")==nullptr)
    {
      return;
    }


    CRITICAL_SECTION mu1;
    CRITICAL_SECTION mu2;

    InitializeCriticalSection(&mu1);
    InitializeCriticalSection(&mu2);

    // mu1 => mu2
    EnterCriticalSection(&mu1);
    EnterCriticalSection(&mu2);
    LeaveCriticalSection(&mu2);
    LeaveCriticalSection(&mu1);

    // mu2 => mu1
    EnterCriticalSection(&mu2);
    EnterCriticalSection(&mu1);  // <<<<<<< OOPS

    //REQUIRE(false);

    LeaveCriticalSection(&mu1);
    LeaveCriticalSection(&mu2);

    DeleteCriticalSection(&mu1);
    DeleteCriticalSection(&mu2);
  }

};

loader_t loader;

struct static_struct_cs_counter_t
{
  static_struct_cs_counter_t()
  {
    critical_sections.resize(1000);

    for(auto& cs:critical_sections)
    {
      InitializeCriticalSection(&cs);
    }
  }

  ~static_struct_cs_counter_t()
  {
    for(auto& cs:critical_sections)
    {
      DeleteCriticalSection(&cs);
    }
  }

  std::vector<CRITICAL_SECTION> critical_sections;
};

static_struct_cs_counter_t static_struct_cs_counter;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  switch(ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  {

  } break;

  case DLL_PROCESS_DETACH:
  {
    return TRUE;

  } break;
  default: break;
  }

  return TRUE;
}
#endif
