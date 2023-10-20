add_executable(hmac512_test hmac512_test.cpp
)
#target_compile_options(hmac512_test PRIVATE "-Wall" "-Wextra")
target_link_libraries(hmac512_test HMAC512::hmac512 ABY::aby ENCRYPTO_utils::encrypto_utils)

