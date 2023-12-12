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

#include <stdexcept>

#include "doctest.h"

TEST_CASE("test_runtime_options_exit_0")
{
  SUBCASE("test_runtime_options_null_ptr_bad")
  {
    char error_text[1024]{};
    const bool ok=cppguard_set_options(nullptr, error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'p_options is null or empty'")==0);
  }

  SUBCASE("test_runtime_options_empty_string_bad")
  {
    const std::string  options;
    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(strcmp(error_text, "Invalid argument: 'p_options is null or empty'")==0);
  }

  SUBCASE("test_runtime_options_blanc_string_bad")
  {
    const std::string  options="     ";
    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'No valid key-value pair: '     ''")==0);
  }

  SUBCASE("test_runtime_options_only_one_comma_bad")
  {
    const std::string  options=",";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'No valid key-value pair: '''")==0);
  }

  SUBCASE("test_runtime_options_only_two_comma_bad")
  {
    const std::string  options=" , ";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'No valid key-value pair: ' ''")==0);
  }

  SUBCASE("test_runtime_options_only_a_equal_b_bad")
  {
    const std::string  options="a:b";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'a' is not known'")==0);
  }

  SUBCASE("test_runtime_options_blanc_string_bad")
  {
    const std::string  options="     ";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'No valid key-value pair: '     ''")==0);
  }

  SUBCASE("test_runtime_options_unknown_key_bad")
  {
    const std::string  options="cola:pepsi";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'cola' is not known'")==0);
  }

  SUBCASE("test_runtime_options_log_simple_good")
  {
    const std::string  options="log:C:/tmp/log.txt";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_log_blancs_good")
  {
    const std::string  options="log:C:/tmp folder with blancs/log.txt";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_log_empty_value_bad")
  {
    const std::string  options="log:";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Value from key-value pair 'log:' is empty'")==0);
  }

  SUBCASE("test_runtime_options_log_empty_key_bad")
  {
    const std::string  options=":C:/tmp/log.txt";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option is empty for key-value pair: :c:/tmp/log.txt'")==0);
  }

  SUBCASE("test_runtime_options_log_no_equal_sign_bad")
  {
    const std::string  options="log value";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'No valid key-value pair: 'log value''")==0);
  }

  SUBCASE("test_runtime_options_log_wrong_equal_sign")
  {
    const std::string  options="log=C:/tmp/log.txt";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'log=c' is not known'")==0);
  }

  SUBCASE("test_runtime_options_log_trailing_comma_bad")
  {
    const std::string  options="log:C:/tmp/log.txt,";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'No valid key-value pair: '''")==0);
  }

  SUBCASE("test_runtime_options_halt_on_error_0_good")
  {
    const std::string  options="halt_on_error:0";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_halt_on_error_1_good")
  {
    const std::string  options="halt_on_error:1";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_halt_on_error_wrong_value_good")
  {
    const std::string  options="halt_on_error:wrongvalue";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'halt_on_error' requires value 1/0 but got 'wrongvalue''")==0);
  }

  SUBCASE("test_runtime_options_halt_on_procces_exit_0_good")
  {
    const std::string  options="fail_process_in_case_of_error:0";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_halt_on_procces_exit_1_good")
  {
    const std::string  options="fail_process_in_case_of_error:1";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_halt_on_procces_exit_wrong_value_good")
  {
    const std::string  options="fail_process_in_case_of_error:wrongvalue";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'fail_process_in_case_of_error' requires value 1/0 but got 'wrongvalue''")==0);
  }

  SUBCASE("test_runtime_options_outputdebugstring_0_good")
  {
    const std::string  options="outputdebugstring:0";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_outputdebugstring_1_good")
  {
    const std::string  options="outputdebugstring:1";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_outputdebugstring_wrong_value_good")
  {
    const std::string  options="outputdebugstring:wrongvalue";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'outputdebugstring' requires value 1/0 but got 'wrongvalue''")==0);
  }

  SUBCASE("test_runtime_options_thread_watchdog_0_good")
  {
    const std::string  options="thread_watchdog:0";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_thread_watchdog_1_good")
  {
    const std::string  options="thread_watchdog:1";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_thread_watchdog_wrong_value_good")
  {
    const std::string  options="thread_watchdog:wrongvalue";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'thread_watchdog' requires value 1/0 but got 'wrongvalue''")==0);
  }

  SUBCASE("test_runtime_options_thread_watchdog_max_duration_sec_value_100_good")
  {
    const std::string  options="thread_watchdog_max_duration_sec:100";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_thread_watchdog_max_duration_sec_value_5_good")
  {
    const std::string  options="thread_watchdog_max_duration_sec:5";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_thread_watchdog_max_duration_sec_value_0_bad")
  {
    const std::string  options="thread_watchdog_max_duration_sec:0";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'thread_watchdog_max_duration_sec' requires value 2..86400 but got '0''")==0);
  }

  SUBCASE("test_runtime_options_thread_watchdog_max_duration_sec_value_max_good")
  {
    const std::string  options="thread_watchdog_max_duration_sec:86400";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_thread_watchdog_max_duration_sec_value_max_plus_one_bad")
  {
    const std::string  options="thread_watchdog_max_duration_sec:86401";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'thread_watchdog_max_duration_sec' requires value 2..86400 but got '86401''")==0);
  }

  SUBCASE("test_runtime_options_thread_watchdog_max_duration_no_number_bad")
  {
    const std::string  options="thread_watchdog_max_duration_sec:nonumber";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Value 'nonumber' for option 'thread_watchdog_max_duration_sec' is not number'")==0);
  }

  SUBCASE("test_runtime_options_exit_code_value_0_good")
  {
    const std::string  options="exit_code:0";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_exit_code_value_126_good")
  {
    const std::string  options="exit_code:126";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_exit_code_value_127_bad")
  {
    const std::string  options="exit_code:127";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'exit_code' requires value 1..126 but got '127''")==0);
  }

  SUBCASE("test_runtime_options_exit_code_value_666_bad")
  {
    const std::string  options="exit_code:666";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'exit_code' requires value 1..126 but got '666''")==0);
  }

  SUBCASE("test_runtime_options_exit_code_value_minus_one_bad")
  {
    const std::string  options="exit_code:-1";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Value '-1' for option 'exit_code' is not number'")==0);
  }

  SUBCASE("test_runtime_options_exit_code_value_no_number_bad")
  {
    const std::string  options="exit_code:nonumber";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Value 'nonumber' for option 'exit_code' is not number'")==0);
  }

  SUBCASE("test_runtime_options_all_options")
  {
    std::string  options="log:C:/tmp/log.txt,halt_on_error:1,fail_process_in_case_of_error:1,outputdebugstring:0,";
    options+="thread_watchdog:1,thread_watchdog_max_duration_sec:100,exit_code:42";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_all_options_blancs_good")
  {
    std::string  options="     log:C:/tmp/log.txt  ,   halt_on_error  :  1,   fail_process_in_case_of_error  :  1,   outputdebugstring   :  0 , ";
    options+="thread_watchdog:1 , thread_watchdog_max_duration_sec   :100  ,exit_code:42";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE(ok);
  }

  SUBCASE("test_runtime_options_all_options_one_with_error_bad")
  {
    std::string  options="     log:C:/tmp/log.txt  ,   halt_on_error  :  1,   fail_process_in_case_of_error  :  1,   outputdebugstring   :  0 , ";
    options+="thread_watchdog:1 , thread_watchdog_max_duration_sec   :1  ,exit_code:42";

    char error_text[1024]{};
    const bool ok=cppguard_set_options(options.c_str(), error_text, sizeof(error_text));
    REQUIRE_FALSE(ok);
    REQUIRE(strcmp(error_text, "Invalid argument: 'Option 'thread_watchdog_max_duration_sec' requires value 2..86400 but got '1''")==0);
  }
}

