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

#define CPPGUARD_ENABLED

#if defined (__cplusplus)
extern "C" { 
#endif

//**public functions**

/**
 * \brief Set options during runtime
 * \param p_options text containing options, key:value pairs seperated by comma 
 * \param p_error_text: error text in case of error
 * \param p_error_text_size: max size of error text
 * \return 
 */
int cppguard_set_options(const char* p_options, char* p_error_text, int p_error_text_size);

/**
* \brief: increase ignore counter for this thread
* \param p_lock_counter: lock counter is increased by this value
* \param p_unlocks_counter: unlock counter is increased by this value
*/
void cppguard_increase_ignore_counter(const unsigned int p_lock_counter, const unsigned int p_unlocks_counter);

//**internal functions**

//CRITICAL_SECTION
#define InitializeCriticalSection(x) cppguard_initialize_critical_section(x)
#define EnterCriticalSection(x) cppguard_enter_critical_section(x)
#define TryEnterCriticalSection(x) cppguard_tryenter_critical_section(x)
#define LeaveCriticalSection(x) cppguard_leave_critical_section(x)
#define DeleteCriticalSection(x) cppguard_delete_critical_section(x)

//std::mutex
#if defined (__cplusplus)
#if (__cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900))
#define _Mtx_init_in_situ(x,y) cppguard_init_in_situ(x,y)
#define _Mtx_destroy_in_situ(x) cppguard_mtx_destroy_in_situ(x)

#define _Mtx_lock(x) cppguard_mtx_lock(x)
#define _Mtx_unlock(x) cppguard_mtx_unlock(x)
#define _Mtx_trylock(x) cppguard_mtx_trylock(x)

#define _Cnd_wait(x, y) cppguard_cnd_wait(x,y) 
#define _Cnd_timedwait(x, y, z) cppguard_cnd_timedwait(x,y,z) 
#endif
#endif


#if defined (__cplusplus)
}
#endif
