#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "../spi.h"
#include "../printbuf.h"
#define BUF_LEN        3 

int main() {
    // Enable UART so we can print
    stdio_init_all();
#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_RX_PIN) || !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi/spi_slave example requires a board with SPI pins
    printf("Default SPI pins were not defined");
#else
    sleep_ms (2 * 1000);

    printf("SPI slave example\n");

    // Enable SPI 0 at 1 MHz and connect to GPIOs
    spi_init(spi_default, 1000 * 1000);
    spi_set_slave(spi_default, true);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);
    gpio_init(22);			// use GPIO 22 ( pin 29 )
    gpio_set_dir(22,false);	// input
    // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI));
    struct FirmwareData fwData = {1,1};

    //uint8_t out_buf[BUF_LEN] = {FWResponse, fwData.major, fwData.minor}, in_buf[BUF_LEN]={0xff};
    uint8_t out_buf[BUF_LEN] = {0xAB, 0xCA, 0xAF}, in_buf[BUF_LEN]={0xfA,0x4D,0x5C};


    printf("SPI slave says: When reading from MOSI, the following buffer will be written to MISO:\n");
    printbuf(out_buf, BUF_LEN);
    //forego one transfer to assure devices are in sync
    while(!gpio_get(22)){}
    while(gpio_get(22)){} 
    for (size_t i = 0; ; ++i) {
	spi_write_read_blocking (spi_default, out_buf, in_buf, 3);
        // Write to stdio whatever came in on the MOSI line.
        printf("SPI slave says: read page %d from the MOSI line:\n", i);
        printbuf(in_buf, BUF_LEN);
    }
#endif
}
