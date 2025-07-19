cmake_minimum_required(VERSION 3.29)
project(container_primitive_tools C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(container_primitive_tools
    cgroups.c
    cgroups.h
    main.c
        tests/getCGroupCount.c
        pid_namespace.c
        uts_namespace.c
        uts_namespace.c
        uts_namespace.c
        uts_namespace.c
        ethernet_pairing.c
        ethernet_pairing.c)
