
add_compile_options(-Wall -Wextra -Werror)

add_executable(turnouts_cmd
    turnouts_cmd.cpp
)

pico_enable_stdio_uart(turnouts_cmd 0)
pico_enable_stdio_usb(turnouts_cmd 1)

target_include_directories(turnouts_cmd PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(turnouts_cmd PRIVATE
    pico_stdlib
    pico_stdio_usb
    misc
    railroad
)

pico_add_extra_outputs(turnouts_cmd)
