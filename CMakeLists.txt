# vim: et ts=4 sw=4
#===============================================================================
# Copyright 2016-2017 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#===============================================================================

cmake_minimum_required(VERSION 2.8)
if(CMAKE_TOOLCHAIN_FILE)
    # 3.8 makes it easier to construct Platform/SX-* files
    cmake_minimum_required(VERSION 3.8)
endif()

# Use old-style version handling
if(POLICY CMP0048)
    cmake_policy(SET CMP0048 OLD)
    cmake_policy(SET CMP0054 OLD)
endif()

# Enable RPATH on MacOS/OSX
if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

# Do not export symbols from executables
if(POLICY CMP0065)
    cmake_policy(SET CMP0065 NEW)
endif()

# Pass all flags to try_compile
if(POLICY CMP0056)
    cmake_policy(SET CMP0056 NEW)
endif()
if(POLICY CMP0066)
    cmake_policy(SET CMP0066 NEW)
endif()

set(PROJECT_NAME "GEN-DNN generic MKL-DNN)")
set(PROJECT_FULL_NAME "Generic (Intel(R) MKL-DNN)")
set(PROJECT_VERSION "0.11")

set(LIB_NAME mkldnn)

project(${PROJECT_NAME} C CXX) # This runs any CMAKE_TOOLCHAIN_FILE (twice)
# If enabled, generate a compile_commands.json file of the exact compiler
# commands (for Makefile and Ninja generators)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=1)

# Want -lcblas *after* -lmkldnn ... How?
#set(LINK_LIBRARIES "${LINK_LIBRARIES} ${SX_LINK_LIBRARIES}" CACHE STRING "will this go after -lmkldnn?")

message(STATUS "CMAKE_TOOLCHAIN_PREFIX = ${CMAKE_TOOLCHAIN_PREFIX}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    message(STATUS "CMAKE_BUILD_TYPE is unset, defaulting to RelWithDebInfo")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

########################## moved (mostly?) to cmake/platform.cmake
#if(NECSX)
#    #show_cmake_stuff("NECSX, after project(... C CXX)")
#    #cmake_policy(SET CMP0066 NEW) # try_compile in cmake 3.6+ will honor CMAKE_EXE_LINKER_FLAGS in the test project
#    #cmake_policy(SET CMP0056 NEW) # try_compile in cmake 3.6+ will honor CMAKE_EXE_LINKER_FLAGS in the test project
#    message(STATUS "Other NEC SX options here ...")
#    # OHOH. this appear *before* -lmkldnn, and we want it to appear *after*
#    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SX_LINK_LIBRARIES}")
#    #set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} ${SX_LINK_LIBRARIES}")
#    #
#    # -----------------------------------------------------------------------
#    # These changes may eventually percolate into dev-cmake-sx and Platform/
#    #
#    #   override to better run SX dbx debugger ... (was with -Cvopt)
#    #   Original: -g -ftrace -Cvopt
#    #       (should be -gv -ftrace -CVopt ???)
#    #
#    # Note: do not use -Cdbeug or -Cnoopt (semantics of language changes?)
#    set(CMAKE_C_FLAGS_DEBUG "-g -traceback -Nipa -Nv -pi,noinline -math,inline,scalar -dir,noopt,nopar,novec -pvctl,nomsg")
#    set(CMAKE_CXX_FLAGS_DEBUG "-g -traceback -Nipa -Nv -pi,noinline -math,inline,scalar -dir,noopt,nopar,novec -pvctl,nomsg")
#    #set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-g") # should NOT conflict (but did!)
#    # -----------------------------------------------------------------------
#    show_cmake_stuff("NECSX, after project(... C CXX)")
#else()
#    project(${PROJECT_NAME} C CXX)
#    include("cmake/MKL.cmake")
#    set(CMAKE_C_FLAGS_DEBUG "-g -O0")
#    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
#endif()
#if(NECSX)
#    cmake_minimum_required(VERSION 3.0)
#else()
#    message(STATUS "CMAKE_SYSTEM_NAME is ${CMAKE_SYSTEM_NAME}")
#    message(STATUS "CMAKE_C_FLAGS    are ${CMAKE_C_FLAGS}")
#    cmake_minimum_required(VERSION 2.8)
#endif()

####################################
# [ejk] support a TARGET_VANILLA **alternative** to src/cpu/ 
option(TARGET_VANILLA "Use only pure C/C++ source_code, no JIT assembler" OFF)
set(TARGET_JIT OFF)
if(TARGET_VANILLA)
    set(TARGET_DEFS "-DTARGET_VANILLA")
    set(TARGET_JIT OFF)
else()
    set(TARGET_DEFS "-DTARGET_XXJIT") # I removed this because too many tests involve the JIT formats
endif()
MESSAGE(STATUS "src/ TARGET_VANILLA: " ${TARGET_VANILLA})
if(TARGET_VANILLA)
    set(CPU_DIR vanilla)
    #set(CPU_INCLUDE_SRC_SUBDIRS)
else()
    set(CPU_DIR cpu)
    set(CPU_INCLUDE_SRC_SUBDIRS cpu/xbyak) # this can be a list of subdirectories of src/
endif()
####################################

#include("cmake/Doxygen.cmake")

#[ejk] fooling around ...
#if(CYGWIN)
#    find_package(Threads) # helps cygwin a bit, but headers are too different.
#endif()
#if(FALSE AND NOT HAVE_MKL) # test code -- not working
#    #set(BLA_VENDOR OpenBLAS)
#    #find_package(BLAS)
#    message(STATUS "BLAS_FOUND        ${BLAS_FOUND}")
#    if(BLAS_FOUND)
#        add_definitions(-DUSE_MKL -DUSE_CBLAS)
#        #include_directories(AFTER ${})
#        list(APPEND mkldnn_LINKER_LIBS ${BLAS_LIBRARIES})
#        message(STATUS "Intel(R) MKL found: include ${MKLINC}, lib ${MKLLIB}")
#        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${BLAS_LINKER_FLAGS}")
#        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BLAS_LINKER_FLAGS}")
#        message(STATUS "BLAS_LIBRARIES    ${BLAS_LIBRARIES}")
#        message(STATUS "BLAS_LINKER_FLAGS ${BLAS_LINKER_FLAGS}")
#        message(STATUS "BLA95_FOUND       ${BLA95_FOUND}")
#        message(STATUS "BLA95_LIBRARIES   ${BLA95_LIBRARIES}")
#    endif()
#    add_definitions(-DUSE_CBLAS)
#    include_directories(AFTER /usr/include/openblas)
#    #set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${BLAS_LINKER_FLAGS}")
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lblas")
#    add_definitions(-DUSE_CBLAS)
#endif()

## sdl options ################# moved into cmake/SDL.cmake
#if(NECSX)
#    message(STATUS "more SX stuff can go here")
#    message(STATUS "testing : CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")
#    #set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -fopenmp")
#    #set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -f90lib")
#    #
#    # disable to force examples through ref_convolution.cpp code...
#    ###set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -DUSE_CBLAS")
#    #
#    #set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -Wl,-M") # or maybe as linker flags?
#    set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -wfatal=47")     # macro redefinition
#    set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -woff=1")        # last line of input file doesn't end with newline
#    #
#    # sxc++ with -Cnoopt or -Cdebug CHANGES SEMANTICS OF snprintf
#    # Soln:
#    #  do not use -Cdebug or -Cnoopt
#    #set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -DBAD_SNPRINTF") # return value of snprintf does not follow c99/c++11 conventions
#
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CCXX_FLAGS} ${SX_CFLAGS_C99}")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CCXX_FLAGS} ${SX_CXXFLAGS_C11}")
#    # MathKeisan libs: additional link libs set in src/CMakeLists.txt
#    list(APPEND mkldnn_LINKER_LIBS ${SX_LINK_LIBRARIES})    # ex. -llapack -lcblas -lblas -lm
#    # cmake propagates the library dependencies to any targets
#    # that depend on the build mkldnn target
#
#    message(STATUS "testing : CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")
#elseif(UNIX OR APPLE)
#    error_if_SX()
#    if(NOT CYGWIN)
#        set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -fPIC")
#    endif()
#    set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -Wformat -Wformat-security")
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_FORTIFY_SOURCE=2")
#    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -D_FORTIFY_SOURCE=2")
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")
#    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#        if(NOT CYGWIN)
#            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -z noexecstack -z relro -z now")
#            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now")
#        else()
#            # good enough for compile, but library still not found :(
#            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath,/home/kruus/gen-dnn/build/src")
#            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,/home/kruus/gen-dnn/build/src")
#        endif()
#        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
#            set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -fstack-protector-all")
#        else()
#            set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -fstack-protector-strong")
#        endif()
#    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#        set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -fstack-protector-all")
#        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now")
#        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now")
#    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
#        # workaround for Intel Compiler 16.0 that produces error caused by pragma omp simd collapse(..)
#        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17.0")
#            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -diag-disable:13379")
#        endif()
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector")
#        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -z noexecstack -z relro -z now")
#        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now")
#    endif()
#
#    #set(CCXX_EXTRADEBUG_FLAGS "-g") # TODO separate Release-mode library debuginfo, someday.
#    # SIMPLER alternative:           cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
#    #set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} ${OPENMP_FLAGS} ${CCXX_EXTRADEBUG_FLAGS} ${CCXX_WARN_FLAGS} ${TARGET_DEFS} -DMKLDNN_DLL -DMKLDNN_DLL_EXPORTS -fvisibility=internal")
#endif()

##### in cmake/platform.cmake
# UNIT8_MAX like macros are part of C99 standard and not C++ (see C99 standard 7.18.2 and 7.18.4)
#add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)

####################################
# [ejk] (might be needed for -DUSE_CBLAS compiles, CHECKME) ok for gcc, others ???
#if(HAVE_MKL)
#    get_filename_component(MKLLIBPATH "${MKLLIB}" PATH) # this one was just not exported by MKL.cmake
#    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath=${MKLLIBPATH}")
#    message(STATUS "   MKL: shared linker flags: ${CMAKE_SHARED_LINKER_FLAGS}")
#endif()
####################################

####### also moved to cmake/platform.cmake
#message(STATUS "initial CMAKE_CCXX_FLAGS: ${CMAKE_CCXX_FLAGS}")
#message(STATUS "initial CMAKE_CCXX_FLAGS: ${CMAKE_CXX_FLAGS}")
#if(NECSX)
#    show_cmake_stuff("NECSX, CMAKE settings before subdirectories...")
#elseif(WIN32)
#    set(USERCONFIG_PLATFORM "x64")
#    add_definitions(-DMKLDNN_DLL -DMKLDNN_DLL_EXPORTS)
#    add_definitions(/MP)
#elseif(UNIX OR APPLE)
#    set(CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} ${CCXX_WARN_FLAGS} -DMKLDNN_DLL -DMKLDNN_DLL_EXPORTS -fvisibility=internal")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_CCXX_FLAGS} -std=c99")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CCXX_FLAGS} -std=c++11 -fvisibility-inlines-hidden")
#    set(CCXX_WARN_FLAGS "-Wall -Werror -Wno-unknown-pragmas")
#    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
#        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -xHOST")
#    endif()
#endif()

######### moved to cmake/OpenMP.cmake
#if(WIN32 AND ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
#        add_definitions(/Qpar)
#else()
#    find_package(OpenMP)
#    #newer version for findOpenMP (>= v. 3.9)
#    if(CMAKE_VERSION VERSION_LESS "3.9")
#        if(OPENMP_FOUND)
#            if(${CMAKE_MAJOR_VERSION} VERSION_LESS "3" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
#                # Override FindOpenMP flags for Intel Compiler (otherwise deprecated)
#                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
#                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
#            else()
#                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#                set(OPENMP_FLAGS "${OpenMP_CXX_FLAGS}")
#            endif()
#        endif()
#    else()
#        if(OpenMP_C_FOUND)
#            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#        endif()
#        if(OpenMP_CXX_FOUND)
#            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#            set(OPENMP_FLAGS "${OpenMP_CXX_FLAGS}")
#        endif()
#    endif()
#endif()
#
#if(HAVE_MKL)
#    set(CMAKE_C_CREATE_SHARED_LIBRARY_FORBIDDEN_FLAGS ${OPENMP_FLAGS})
#    set(CMAKE_CXX_CREATE_SHARED_LIBRARY_FORBIDDEN_FLAGS ${OPENMP_FLAGS})
#    if (NOT WIN32)
#        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -liomp5 -Wl,--as-needed")
#        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -liomp5 -Wl,--as-needed")
#    endif()
#endif()

############# moved into cmake/platform.cmake
#if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -xHOST")
#elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -ffast-math")
#    # maybe also -ffinite-math or -funsafe-math-optimizations ?
#endif()

include("cmake/platform.cmake")
include("cmake/OpenMP.cmake")
include("cmake/SDL.cmake")
include("cmake/MKL.cmake")
include("cmake/Doxygen.cmake")

enable_testing()

include_directories(include)

message(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
add_subdirectory(src)
#message(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
add_subdirectory(examples)
add_subdirectory(tests)

install(FILES LICENSE DESTINATION share/doc/${LIB_NAME} COMPONENT doc)
message(STATUS "CMakeLists.txt DONE!")
