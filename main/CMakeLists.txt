file(GLOB_RECURSE C_SOURCES *.c)
file(GLOB_RECURSE CPP_SOURCES *.cpp)

idf_component_register(SRCS ${C_SOURCES}  ${CPP_SOURCES} 
                   INCLUDE_DIRS "."  "driver/nextion_async" 
                                    "driver/dial_big_async"
                                     "driver/dial_short_async"
                                     "driver/hx711_async" 
                    "app" "app/calibration"
                     "app/gui" "app/sensor" 
                     "app/testing" "app/usb"
                     "app/configuration" "app/testing/current_test"
                     "app/testing/new_test" "app/testing/previous_test"
                   )
