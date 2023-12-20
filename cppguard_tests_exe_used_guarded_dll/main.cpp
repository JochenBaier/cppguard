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

#include "argh.h"

#include <thread>

void good_test();
void bad_test();

int main(int, char* argv[])
{
  argh::parser cmdl(argv);

  bool run_good_test=false;
  if(cmdl[{ "-g", "--good" }])
  {
    run_good_test=true;
  }

  bool run_bad_test=false;
  if(cmdl[{ "-b", "--bad" }])
  {
    run_bad_test=true;
  }


  if(run_good_test)
  {
    std::thread t([&]()
      {
        for(size_t i=0; i<1000; i++)
        {
          good_test();
        }
      });

    t.join();
  }

  if (run_bad_test)
  {
    bad_test();
  }

  return 0;
}

