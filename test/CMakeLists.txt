
add_compile_options(-Wall -Wextra -Werror)

# lights_test

add_executable(lights_test
    lights_test.cpp
)

pico_enable_stdio_uart(lights_test 0)
pico_enable_stdio_usb(lights_test 1)

target_link_libraries(lights_test PRIVATE
    pico_stdlib
    pico_stdio_usb
    railroad
    ws2812
)

pico_add_extra_outputs(lights_test)

# display_test

add_executable(display_test
    display_test.cpp
)

pico_enable_stdio_uart(display_test 0)
pico_enable_stdio_usb(display_test 1)

target_link_libraries(display_test PRIVATE
    pico_stdlib
    pico_stdio_usb
    railroad
)

pico_add_extra_outputs(display_test)
