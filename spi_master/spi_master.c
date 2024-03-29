//=================================================================================//

#include <stdio.h>
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "../spi.h"
#include "../printbuf.h"
//=================================================================================//

#define BUF_LEN 3

//=================================================================================//

int main() {
  // Enable USB serial so we can print
  stdio_init_all();

  sleep_ms (2 * 1000);
  printf ("SPI Central Example\n");

  // Enable SPI0 at 1 KHz
  spi_init (spi_default, 1 * 1000);

  // Assign SPI functions to the default SPI pins
  gpio_set_function (PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
  gpio_set_function (PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function (PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
  gpio_set_function (PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);

  gpio_init(22);			// use GPIO 22 ( pin 29 )
  gpio_set_dir(22,true);	// output
  // We need two buffers, one for the data to send, and one for the data to receive.
  u_int8_t out_buf [BUF_LEN]= {FWRequest, 0xAD, 0xFC}, in_buf [BUF_LEN]={0x4E, 0xC5, 0xB9};

  for (uint8_t i = 0; ; ++i) {
    printf ("Sending data %d to SPI Peripheral\n", i);
    // Write the output buffer to COPI, and at the same time read from CIPO to the input buffer.
    gpio_put(22,false); 	// low, select slave
    sleep_ms (1 * 1000);
    spi_write_read_blocking (spi_default, out_buf, in_buf, 1);
    sleep_ms (10);
    spi_write_read_blocking (spi_default, out_buf+1, in_buf+1, 2);
    gpio_put(22,true);
    // Sleep for some seconds so you get a chance to read the output.
    printbuf(in_buf, BUF_LEN);
  }
}

//=================================================================================//

