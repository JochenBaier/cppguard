// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//*************************************************************
//Quickstart example: open parent folder of this file in Visual Studio via 'Open Local Folder'
//and select 'Quickstart_example' as startup item  and run it
//*************************************************************

#include <iostream>
#include <thread>
#include <Windows.h>

const int NUM_PHILOSOPHERS=5;
CRITICAL_SECTION forks[NUM_PHILOSOPHERS];

void philosopher_thread_function(int id, CRITICAL_SECTION& left_fork, CRITICAL_SECTION& right_fork)
{
  while(true)
  {
    EnterCriticalSection(&left_fork);
    std::cout<<"Philosopher "<<id<<" picked up left fork"<<std::endl;

    // Introduce a deliberate delay to increase the chances of deadlock
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EnterCriticalSection(&right_fork);
    std::cout<<"Philosopher "<<id<<" picked up right fork"<<std::endl;

    // Eat or think (not implemented in this example)

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::cout<<"Philosopher "<<id<<" released both forks"<<std::endl;
    LeaveCriticalSection(&left_fork);
    LeaveCriticalSection(&right_fork);
  }
}

int main()
{
  //comment out to enable halt on error
  //char error_text[1024]{};
  //int res = cppguard_set_options("halt_on_error:1", error_text, _countof(error_text));
  //assert(res == 1);

  // Initialize critical sections
  for(auto& fork:forks)
  {
    InitializeCriticalSection(&fork);
  }

  std::thread philosophers[NUM_PHILOSOPHERS];

  // Start philosophers with their respective left and right forks
  for(int i=0; i<NUM_PHILOSOPHERS; ++i)
  {
    philosophers[i]=std::thread(philosopher_thread_function, i, std::ref(forks[i]), std::ref(forks[(i+1)%NUM_PHILOSOPHERS]));
  }

  // Wait for the philosophers to finish (will not happen in this case due to deadlock)
  for(auto& philosopher:philosophers)
  {
    philosopher.join();
  }

  // Clean up critical sections
  for(auto& fork:forks)
  {
    DeleteCriticalSection(&fork);
  }

  return 0;
}

