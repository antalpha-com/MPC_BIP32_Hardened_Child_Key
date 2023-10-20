add_library(hmac512
    B2A/B2Aparty.cpp
    B2A/B2Asharing.cpp
    BigNum/BigNum/BigNum.c
    BigNum/BigNum/BigNum_mod.c
    BigNum/Common/Common.c
    hmac512_circuit/hmac512_circuit.cpp
)
add_library(HMAC512::hmac512 ALIAS hmac512)

target_compile_features(hmac512 PUBLIC cxx_std_17)
#target_compile_options(hmac512 PRIVATE "-Wall" "-Wextra" "-Weffc++")
target_compile_options(hmac512 PRIVATE "-Wall" "-Wextra")

target_include_directories(hmac512
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

# This assumes that libstdc++ is used and should not be required for e.g.
# libc++.  Linking to libstdc++fs is currently required when using the
# std::filesystem library.
# cf. https://gitlab.kitware.com/cmake/cmake/issues/17834
target_link_libraries(hmac512
	PRIVATE stdc++fs
)

target_link_libraries(hmac512
    PUBLIC ABY::aby
    #PUBLIC OTExtension::otextension
    PUBLIC ENCRYPTO_utils::encrypto_utils
    #PUBLIC GMP::GMP
    #PUBLIC Threads::Threads
)


install(TARGETS hmac512
        EXPORT "${PROJECT_NAME}Targets"
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        INCLUDES DESTINATION lib
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
)

export(TARGETS hmac512 NAMESPACE "${PROJECT_NAME}::" FILE "${PROJECT_NAME}Targets.cmake")
install(EXPORT "${PROJECT_NAME}Targets"
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION "lib/cmake/${PROJECT_NAME}"
)

include(CMakePackageConfigHelpers)

configure_package_config_file("${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    DESTINATION "lib/cmake/${PROJECT_NAME}"
)

