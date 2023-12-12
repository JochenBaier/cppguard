#CppGuard quickstart_example

cmake_minimum_required(VERSION 3.15)
project(quickstart_example)

add_executable(quickstart_example main.cpp) 

#add this line to all targets you want to test
target_link_libraries(quickstart_example cppguard::cppguard cppguard::auto_include)

#Optimization:
#if the target is static library use:target_link_libraries(quickstart_example cppguard::auto_include)
#if the target is a exe or dll which does not contain any locks use: target_link_libraries(quickstart_example cppguard::cppguard)



