
add_compile_options(-Wall -Wextra -Werror)

add_executable(oneshot
    oneshot.cpp
)

pico_enable_stdio_uart(oneshot 0)
pico_enable_stdio_usb(oneshot 1)

target_include_directories(oneshot PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(oneshot PRIVATE
    pico_stdlib
    pico_stdio_usb
    hardware_clocks
    hardware_gpio
    hardware_pwm
)

pico_add_extra_outputs(oneshot)
