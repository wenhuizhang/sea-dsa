add_definitions(-D__STDC_CONSTANT_MACROS)
add_definitions(-D__STDC_LIMIT_MACROS)
find_program(CLANG-FORMAT NAMES clang-format clang-format-mp-10 clang-format-10 REQUIRED)

set(LLVM_LINK_COMPONENTS
  irreader
  bitwriter
  analysis
  ipo
  scalaropts
  instrumentation
  core
  codegen
  objcarcopts)


add_llvm_executable(seadsa DISABLE_LLVM_LINK_LLVM_DYLIB seadsa.cc)
add_executable(libcSpecGen libcSpecGen.cc)

target_link_libraries (seadsa PRIVATE SeaDsaAnalysis)
add_custom_target(sea-dsa-tool DEPENDS seadsa)


if (TOP_LEVEL OR TARGET sea-dsa-tool)
  install(TARGETS seadsa RUNTIME DESTINATION bin)
endif()

if (SEA_DSA_STATIC_EXE)
  set (CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
  set_target_properties (seadsa PROPERTIES LINK_SEARCH_START_STATIC ON)
  set_target_properties (seadsa PROPERTIES LINK_SEARCH_END_STATIC ON)
endif()

message(${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_C_COMPILER_ID STREQUAL "Clang")

  add_custom_target(GenerateSpecs ALL
    COMMAND libcSpecGen
    COMMAND ${CLANG-FORMAT} -i libc.spec.c
    DEPENDS libcSpecGen
  )

  add_custom_target(LibcSpec-64 ALL
                  COMMAND ${CMAKE_C_COMPILER}
                          -I${CMAKE_CURRENT_SOURCE_DIR}/../include
                          -emit-llvm -c ${CXX_COMPILER_FLAGS}
                          -march=native -O3
                          -o libc-64.spec.bc
                          libc.spec.c
                  DEPENDS GenerateSpecs)

  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/libc-64.spec.bc
            DESTINATION lib)

  option(SEAHORN_BUILD_32_BIT_SPEC "Build 32bit library specs" OFF)
  if(SEAHORN_BUILD_32_BIT_SPEC) 

  add_custom_target(LibcSpec-32 ALL
                  COMMAND ${CMAKE_C_COMPILER}
                          -I${CMAKE_CURRENT_SOURCE_DIR}/../include
                          -emit-llvm -c ${CXX_COMPILER_FLAGS}
                          -march=native -O3
                          -o libc-32.spec.bc
                          libc.spec.c
                  DEPENDS GenerateSpecs)

    install (FILES ${CMAKE_CURRENT_BINARY_DIR}/libc-32.spec.bc
              DESTINATION lib)
  endif()
else()
  message (WARNING "Unable to build specs \n\t Set C compiler to clang with -DCMAKE_C_COMPILER")
endif()