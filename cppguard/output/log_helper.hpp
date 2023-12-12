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

#include <thread>

#include "attributes.hpp"
#include "fmt/format.h"

class GraphCycles;
struct SynchLocksHeld;

struct lock_t;
struct statistics_t;
struct runtime_options_t;

namespace log_helper
{
TM_ATTRIBUTE_NOINLINE void build_deadlock_circle_log_text(const GraphCycles& p_deadlock_graph, void* mu, const uint64_t& mu_id, const uint64_t& other_node_id, fmt::memory_buffer& p_log_string);
void create_summary(statistics_t& p_statistics, const bool& p_found_deadlocks, const bool& p_other_errors_found, fmt::memory_buffer& p_summary);
TM_ATTRIBUTE_NOINLINE void build_delete_locked_lock_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string);
TM_ATTRIBUTE_NOINLINE void build_release_locked_deleted_by_other_thread_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string);
TM_ATTRIBUTE_NOINLINE void build_release_lock_which_thread_does_hold_log_text(const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string);
void build_watchdog_error(const std::thread::id& p_thread_id, const bool& p_in_lock, const uint64_t& p_hold_duration_sec, fmt::memory_buffer& p_log_string);
void build_watchdog_callstack(GraphCycles& p_deadlock_graph, const lock_t& p_lock, fmt::memory_buffer& p_log_string);
void build_max_locks_per_thread_reached_text(const int& p_lock_count, const std::thread::id& p_thread_id, const void* mu, fmt::memory_buffer& p_log_string);
void process_error(fmt::memory_buffer& p_log_text, const runtime_options_t& p_runtime_options, bool p_is_deadlock);
}