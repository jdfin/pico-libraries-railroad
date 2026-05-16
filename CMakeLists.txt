
add_library(railroad INTERFACE)

target_sources(railroad INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/lights.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/locos.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/turnout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/desktop_layout.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/desktop_ops.cpp
)

target_include_directories(railroad INTERFACE 
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(railroad INTERFACE
    pico_stdlib
    hardware_clocks
    hardware_gpio
    hardware_pwm
    dcc
    misc
    ws2812
)

add_subdirectory(oneshot)
add_subdirectory(turnouts_cmd)
add_subdirectory(sensors_test)
add_subdirectory(test)
