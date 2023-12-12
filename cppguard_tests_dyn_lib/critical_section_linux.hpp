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

#pragma  once


#include "pthread.h"

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
// used by C++ source code
#endif


typedef   pthread_mutex_t CRITICAL_SECTION;

void cppguard_enter_critical_section(void* lpCriticalSection);
int cppguard_tryenter_critical_section(void* lpCriticalSection);
void cppguard_leave_critical_section(void* lpCriticalSection);

void InitializeCriticalSection(CRITICAL_SECTION* p_critical_section);
void DeleteCriticalSection(CRITICAL_SECTION* p_critical_section);

#ifdef __cplusplus
}
#endif
