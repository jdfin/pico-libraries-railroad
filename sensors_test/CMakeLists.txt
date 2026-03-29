
add_compile_options(-Wall -Wextra -Werror)

# sensor_test

add_executable(sensor_test
    sensor_test.cpp
)

pico_enable_stdio_uart(sensor_test 0)
pico_enable_stdio_usb(sensor_test 1)

target_include_directories(sensor_test PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(sensor_test PRIVATE
    pico_stdlib
    pico_stdio_usb
    misc
    railroad
)

pico_add_extra_outputs(sensor_test)

# sensor2_test

add_executable(sensor2_test
    sensor2_test.cpp
)

pico_enable_stdio_uart(sensor2_test 0)
pico_enable_stdio_usb(sensor2_test 1)

target_include_directories(sensor2_test PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(sensor2_test PRIVATE
    pico_stdlib
    pico_stdio_usb
    misc
    railroad
)

pico_add_extra_outputs(sensor2_test)
