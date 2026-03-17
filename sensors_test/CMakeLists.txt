
add_compile_options(-Wall -Wextra -Werror)

add_executable(sensors_test
    sensors_test.cpp
)

pico_enable_stdio_uart(sensors_test 0)
pico_enable_stdio_usb(sensors_test 1)

target_include_directories(sensors_test PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(sensors_test PRIVATE
    pico_stdlib
    pico_stdio_usb
    misc
    railroad
)

pico_add_extra_outputs(sensors_test)
