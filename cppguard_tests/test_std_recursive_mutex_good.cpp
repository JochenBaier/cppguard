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

#include "doctest.h"

#include <mutex>
#include "utils/signal.hpp"

//FIXME some variables could be optimized away in release?

TEST_CASE("test_std_recursive_mutex_t1_a_exit_0")
{
  std::recursive_mutex ma;
  ma.lock();
  ma.unlock();
}

TEST_CASE("test_std_recursive_mutex_t1_a_b_exit_0")
{
  std::recursive_mutex ma;
  std::recursive_mutex mb;

  ma.lock();
  mb.lock();

  mb.unlock();
  ma.unlock();
}

TEST_CASE("test_std_mutex_t1_a_lock_recurse_good_exit_0")
{
  std::recursive_mutex ma;
  ma.lock();
  ma.lock();
  ma.unlock();
  ma.unlock();
}

TEST_CASE("test_lock_guard_std_recursive_mutex_t1_a_exit_0")
{
  std::recursive_mutex ma;

  {
    const std::lock_guard <std::recursive_mutex> lock(ma);

    {
      const bool got_lock=ma.try_lock();
      REQUIRE(got_lock);
    }

  }

  ma.unlock();
}

TEST_CASE("test_std_lock_guard_std_recursive_mutex_t1_a_b_exit_0")
{
  std::recursive_mutex ma;
  std::recursive_mutex mb;

  {
    const std::lock_guard < std::recursive_mutex> lock_a(ma);

    {
      const bool got_lock=ma.try_lock();
      REQUIRE(got_lock);
    }

    const std::lock_guard < std::recursive_mutex> lock_b(mb);

    {
      const bool got_lock=mb.try_lock();
      REQUIRE(got_lock);
    }
  }

  ma.unlock();
  mb.unlock();

}

TEST_CASE("test_lock_guard_std_mutex_t1_a_lock_recurse_exit_0")
{
  std::recursive_mutex ma;
  {
    const std::lock_guard < std::recursive_mutex> lock_a(ma);
    const std::lock_guard < std::recursive_mutex> lock_b(ma);
  }

}

TEST_CASE("test_std_recursive_mutex_delete_without_use_exit_0")
{
  {
    std::recursive_mutex m;
  }
}

TEST_CASE("test_std_recursive_mutex_delete_with_use_exit_0")
{
  {
    std::recursive_mutex m;
    m.lock();
    m.unlock();
  }
}

TEST_CASE("test_std_recursive_mutex_mass_delete_exit_0")
{
  for(size_t i=0; i<10000; ++i)
  {
    auto array_of_locks=std::make_unique<std::recursive_mutex[]>(1000);

    for(size_t j=0; j<1000; j++)
    {
      array_of_locks[j].lock();
      array_of_locks[j].unlock();
    }

  }
}

