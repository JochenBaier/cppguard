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

cmake_minimum_required(VERSION 3.10)

set(source_list
dl_main.cpp
test_in_cpp_file_dyn_lib.hpp
tests_dyn_lib_api.c
test_in_c_file_dyn_lib.c
test_in_c_file_dyn_lib.h
test_in_cpp_file_dyn_lib.cpp
)

if (WIN32)
set(source_list ${source_list}
tests_dyn_lib.def
)
endif()

if (UNIX)
set(source_list ${source_list}
critical_section_linux.hpp
critical_section_linux.cpp
)
endif()

add_library(cppguard_tests_dyn_lib SHARED ${source_list})


if (WIN32)
target_link_libraries(cppguard_tests_dyn_lib cppguard::cppguard cppguard::auto_include)
endif()

#target_compile_options(tests_dyn_lib PRIVATE -fsanitize=thread)
#target_link_options(tests_dyn_lib PRIVATE -fsanitize=thread)






