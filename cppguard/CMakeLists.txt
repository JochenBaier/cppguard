#Licensed to the Apache Software Foundation (ASF) under one
#or more contributor license agreements.  See the NOTICE file
#distributed with this work for additional information
#regarding copyright ownership.  The ASF licenses this file
#to you under the Apache License, Version 2.0 (the
#"License"); you may not use this file except in compliance
#with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing,
#software distributed under the License is distributed on an
#"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#KIND, either express or implied.  See the License for the
#specific language governing permissions and limitations
#under the License.  

cmake_minimum_required(VERSION 3.15)
project(cppguard)
set(CMAKE_CXX_STANDARD 14)

set(source_list 
cppguard.rc
dl_main.cpp
graphcycles/graphcycles.cc
graphcycles/graphcycles.h
deadlock_monitor.cpp
deadlock_monitor.hpp
helper/low_level_alloc.cc
helper/low_level_alloc.h
dll_exports/deadlock_monitor_api.cpp
stacktrace/stacktrace.cc
stacktrace/stacktrace.h
fmt/format.cc
thread_watchdog/thread_watchdog.cpp
thread_watchdog/thread_watchdog.hpp
helper/cs_mutex.cpp
helper/pystring.cpp
helper/pystring.h
helper/options_parser.cpp
helper/options_parser.hpp
helper/run_time_options.hpp
helper/cs_mutex.cpp
helper/cs_mutex.hpp

helper/timer.hpp
output/log.cpp
output/log.hpp
output/log_helper.cpp
output/log_helper.hpp
api_monitors/critical_section_monitor.cpp
thread_history/thread_history_helper.cpp
thread_history/thread_history_helper.hpp
helper/attributes.hpp
helper/one_shot_timer.cpp
helper/one_shot_timer.hpp
)

if (WIN32)
  set(source_list ${source_list}
          dll_exports/cppguard_exports.def
          api_monitors/mutex_monitor.cpp
  )
endif()

add_library(cppguard SHARED ${source_list})
set_property(TARGET cppguard PROPERTY CXX_STANDARD 14)
set_property(TARGET cppguard PROPERTY CXX_STANDARD_REQUIRED on)

if(CPPGUARD_MAX_LOCKS_PER_THREAD)
  target_compile_definitions(cppguard PRIVATE MAX_LOCKS_PER_THREAD=${CPPGUARD_MAX_LOCKS_PER_THREAD})
  else()
  target_compile_definitions(cppguard PRIVATE MAX_LOCKS_PER_THREAD=20)
endif()

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

target_include_directories(cppguard
 PRIVATE
   ${CMAKE_CURRENT_LIST_DIR}
    thread_history
    stacktrace/
    thread_watchdog/
    graphcycles/
    output/
    helper/
    pystring/
    fmt
  PUBLIC
     ${CMAKE_CURRENT_LIST_DIR}/include
)


add_library(cppguard_forced_include INTERFACE)
if (WIN32)
target_compile_options(cppguard_forced_include INTERFACE "/FI${CMAKE_CURRENT_LIST_DIR}/include/cppguard.hpp" )
endif()
add_library(cppguard::auto_include ALIAS cppguard_forced_include)

add_library(cppguard::cppguard ALIAS cppguard)


if (WIN32)
  #flags for standard conformance
  target_compile_options(cppguard PRIVATE "/permissive-")
  target_compile_options(cppguard PRIVATE "/volatile:iso")
  target_compile_options(cppguard PRIVATE "/Zc:inline")
  target_compile_options(cppguard PRIVATE "/Zc:preprocessor")
  target_compile_options(cppguard PRIVATE "/Zc:lambda")
  target_compile_options(cppguard PRIVATE "/Zc:__cplusplus")
  target_compile_options(cppguard PRIVATE "/Zc:externConstexpr")
  target_compile_options(cppguard PRIVATE "/Zc:throwingNew")
  target_compile_options(cppguard PRIVATE "/EHsc")
   
  #this warnings as errors
  target_compile_options(cppguard PRIVATE "/we4062") # enumerator in switch not handled
  target_compile_options(cppguard PRIVATE "/we4296") # '>=': expression is always true
  target_compile_options(cppguard PRIVATE "/we4715") # not all control paths return a value
  target_compile_options(cppguard PRIVATE "/we4553") # '==': operator has no effect; did you intend '='?
  target_compile_options(cppguard PRIVATE "/we4552") # '>=': operator has no effect; expected operator with side-effect
  target_compile_options(cppguard PRIVATE "/we4456") # declaration of 'xxx' hides previous local declaration
  target_compile_options(cppguard PRIVATE "/we4239") # nonstandard extension used
  target_compile_options(cppguard PRIVATE "/we4706") # assignment within conditional expression
  target_compile_options(cppguard PRIVATE "/we4129") # unrecognized character escape sequence

  set_property(TARGET cppguard PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  target_compile_options(cppguard PRIVATE "/W4" )
  target_compile_options(cppguard PRIVATE "$<$<CONFIG:RELEASE>:/GL>" )
  target_link_options(cppguard PRIVATE "$<$<CONFIG:RELEASE>:/LTCG>" )
  target_link_libraries(cppguard PRIVATE dbghelp.lib)

else()
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(cppguard PRIVATE Threads::Threads)
endif()


#target_compile_options(cppguard PUBLIC -fsanitize=address)
#target_link_options(cppguard PUBLIC -fsanitize=address)

#target_compile_options(cppguard PUBLIC -fsanitize=thread)
#target_link_options(cppguard PUBLIC -fsanitize=thread)

