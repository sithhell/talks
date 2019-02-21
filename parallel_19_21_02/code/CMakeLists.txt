# Copyright (c) 2018 Thomas Heller

cmake_minimum_required(VERSION 3.5)

project(parallel19 CXX)

find_package(HPX REQUIRED)

add_executable(execution_resource execution_resource.cpp)
hpx_setup_target(execution_resource)
add_executable(executor executor.cpp)
hpx_setup_target(executor)
add_executable(saxpy saxpy.cpp)
hpx_setup_target(saxpy)
add_executable(saxpy_numa saxpy_numa.cpp)
hpx_setup_target(saxpy_numa)
add_executable(tasks tasks.cpp)
hpx_setup_target(tasks)
add_executable(future future.cpp)
hpx_setup_target(future)
