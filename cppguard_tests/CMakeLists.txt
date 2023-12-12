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

set(source_list 
main.cpp  
utils/signal.cpp
utils/signal.hpp
utils/cs.cpp
utils/cs.hpp
utils/random_numbers.cpp
utils/random_numbers.hpp
test_critical_section_lock_order_inversion.cpp
test_critical_section_lock_order_ok_or_disabled.cpp
test_thread_watchdog_bad.cpp
test_thread_watchdog_good.cpp
ctests.cmake
)

if (WIN32)
set(source_list ${source_list}
        test_runtime_options.cpp
        test_std_mutex_good.cpp
        test_std_mutex_bad.cpp
        test_std_recursive_mutex_good.cpp
        test_std_recursive_mutex_bad.cpp
        test_c_file.c
        test_c_file.h
    )
endif()


add_executable(cppguard_tests ${source_list} )
target_include_directories(cppguard_tests PRIVATE ${CMAKE_CURRENT_LIST_DIR} utils)
target_link_libraries(cppguard_tests cppguard::cppguard cppguard::auto_include)


if (WIN32)
target_link_libraries(cppguard_tests cppguard_tests_static_lib)
target_link_libraries(cppguard_tests cppguard_tests_dyn_lib)
endif()


include(ctests.cmake)

#target_compile_options(cppguard_tests PRIVATE -fsanitize=thread)
#target_link_options(cppguard_tests PRIVATE -fsanitize=thread)
