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

#include "stacktrace.h"

#ifdef _WIN32
#include <windows.h>
#pragma warning(push)
#pragma warning(disable:4091)
#include <dbghelp.h>
#pragma warning(pop)
#endif

#include <cassert>

#if _WIN32
typedef USHORT NTAPI RtlCaptureStackBackTrace_Function(
  IN ULONG frames_to_skip,
  IN ULONG frames_to_capture,
  OUT PVOID* backtrace,
  OUT PULONG backtrace_hash);

static RtlCaptureStackBackTrace_Function* const RtlCaptureStackBackTrace_fn=(RtlCaptureStackBackTrace_Function*)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlCaptureStackBackTrace");

static HANDLE process=nullptr;
#endif

TM_ATTRIBUTE_NOINLINE int GetStackFrames(void** result, int p_max_depth, int p_skip_count)
{
#if _WIN32
  assert(RtlCaptureStackBackTrace_fn);
  const int size=(RtlCaptureStackBackTrace_fn)(static_cast<ULONG>(p_skip_count)+1, static_cast<ULONG>(p_max_depth), result, nullptr);
  return size;
#else
  return 0;
#endif
}

void InitializeSymbolizer()
{
#if _WIN32

  if(process!=nullptr) {
    return;
  }
  process=GetCurrentProcess();

  // Symbols are not loaded until a reference is made requiring the
  // symbols be loaded. This is the fastest, most efficient way to use
  // the symbol handler.
  SymSetOptions(SYMOPT_DEFERRED_LOADS|SYMOPT_UNDNAME);
  if(!SymInitialize(process, nullptr, true)) {
    // GetLastError() returns a Win32 DWORD, but we assign to
    // unsigned long long to simplify the ABSL_RAW_LOG case below.  The uniform
    // initialization guarantees this is not a narrowing conversion.
    const unsigned long long error{GetLastError()};  // NOLINT(runtime/int)
    printf("CppGuard: Internal error: SymInitialize() failed: %llu", error);
    assert(false);
  }
#endif
}

void printf_symbolinfo(const void* pc, fmt::memory_buffer& p_callstack_string)
{
#if _WIN32
  alignas(SYMBOL_INFO) char buf[sizeof(SYMBOL_INFO)+MAX_SYM_NAME];
  SYMBOL_INFO* symbol=reinterpret_cast<SYMBOL_INFO*>(buf);
  symbol->SizeOfStruct=sizeof(SYMBOL_INFO);
  symbol->MaxNameLen=MAX_SYM_NAME;

  if(!SymFromAddr(process, reinterpret_cast<DWORD64>(pc), nullptr, symbol))
  {
    return;
  }

  const std::string symbol_name=symbol->Name;

  // Get the line number information
  IMAGEHLP_LINE64 line;
  line.SizeOfStruct=sizeof(IMAGEHLP_LINE64);

  DWORD displacement;
  const bool get_line_ok=SymGetLineFromAddr64(process, reinterpret_cast<DWORD64>(pc), &displacement, &line);

  fmt::format_to(std::back_inserter(p_callstack_string), "{}({},0): {}\n", get_line_ok ? line.FileName : "---", get_line_ok ? line.LineNumber : 0, symbol_name);
#endif
}

void printf_call_stack_symbol_infos(void** pcs, int n, fmt::memory_buffer& p_callstack_string)
{
  for(int i=0; i!=n; i++)
  {
    printf_symbolinfo(pcs[i], p_callstack_string);
  }
}

TM_ATTRIBUTE_NOINLINE void save_current_callstack_to_string(fmt::memory_buffer& p_callsstack_string, const int& p_skip_count)
{
#if _WIN32
  void* pcs[40]{};
  const int s=GetStackFrames(pcs, _countof(pcs), p_skip_count);
  printf_call_stack_symbol_infos(pcs, s, p_callsstack_string);
#endif
}
