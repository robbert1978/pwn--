project(Process CXX)
message(STATUS "Configuring module '${PROJECT_NAME}'")

enable_language(C ASM_MASM)

set(PROJECT_DIR ${PWNLIB_MODULE_DIR}/${PROJECT_NAME})
set(INTERFACE_DIR ${PROJECT_DIR}/Include)
set(SOURCE_DIR ${PROJECT_DIR}/Source)
set(HEADER_DIR ${SOURCE_DIR}/Include)
set(TEST_DIR ${PROJECT_DIR}/Tests)

list(APPEND SOURCE_FILES)

if(WIN32)
    list(APPEND SOURCE_FILES
        ${SOURCE_DIR}/Win32/Process.cpp
        ${SOURCE_DIR}/Win32/Thread.cpp
        ${SOURCE_DIR}/Win32/Job.cpp
        ${SOURCE_DIR}/Win32/Memory.cpp
        ${SOURCE_DIR}/Win32/ThreadGroup.cpp

        $<$<BOOL:0>:${CMAKE_GENERATOR_PLATFORM}>
        $<$<STREQUAL:${CMAKE_GENERATOR_PLATFORM},x64>:${SOURCE_DIR}/Win32/asm/x64/get_teb.asm>
        $<$<STREQUAL:${CMAKE_GENERATOR_PLATFORM},win32>:${SOURCE_DIR}/Win32/asm/x86/get_teb.asm>

        # $<$<STREQUAL:${CMAKE_GENERATOR_PLATFORM},arm64>:${SOURCE_DIR}/asm/arm64/get_teb.asm>
    )
else()
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
add_dependencies(${PROJECT_NAME} PWN::Common PWN::Security PWN::Symbols PWN::System)
add_library(PWN::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${INTERFACE_DIR} PRIVATE ${HEADER_DIR})
target_compile_definitions(${PROJECT_NAME} PUBLIC)
target_link_libraries(${PROJECT_NAME} PUBLIC PWN::Common PWN::Security PWN::Symbols PWN::System Userenv.lib)

install(DIRECTORY ${INTERFACE_DIR} DESTINATION ${CMAKE_PROJECT_NAME})

if(PWN_BUILD_TESTING)
    add_subdirectory(Tests)
endif(PWN_BUILD_TESTING)
