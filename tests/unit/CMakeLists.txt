FetchContent_Declare(unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.7.0
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(unity)

add_executable(test_ring_buffer test_ring_buffer.c)
target_link_libraries(test_ring_buffer PRIVATE portable unity project_warnings)
add_test(NAME ring_buffer COMMAND test_ring_buffer)
