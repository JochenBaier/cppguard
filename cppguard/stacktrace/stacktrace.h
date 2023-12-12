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

#include "attributes.hpp"
#include "fmt/format.h"

TM_ATTRIBUTE_NOINLINE int GetStackFrames(void** result, int p_max_depth, int p_skip_count);
void InitializeSymbolizer();
void printf_call_stack_symbol_infos(void** pcs, int n, fmt::memory_buffer& p_callstack_string);
TM_ATTRIBUTE_NOINLINE void save_current_callstack_to_string(fmt::memory_buffer& p_callstack_string, const int& p_skip_count);
