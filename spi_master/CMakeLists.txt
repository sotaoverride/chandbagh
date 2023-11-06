add_executable(spi_master_messaging
        spi_master.c
        )

# enable usb output, disable uart output
pico_enable_stdio_usb(spi_master_messaging 1)
pico_enable_stdio_uart(spi_master_messaging 0)
# Pull in basic dependencies
target_link_libraries(spi_master_messaging pico_stdlib hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(spi_master_messaging)

# add url via pico_set_program_url
example_auto_set_url(spi_master_messaging)
