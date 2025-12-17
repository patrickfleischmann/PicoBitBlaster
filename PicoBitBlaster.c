//PicoBitBlaster - a simple SPI and JTAG bitbanger for the Raspberry Pi Pico

#include <stdio.h>
#include "pico/stdlib.h"
#include <pico/bootrom.h>
#include "hardware/gpio.h"


#define MISO_O 16
#define CSN_O 17
#define SCK_O 18
#define MOSI_O 19

#define TDO 0
#define TMS 1
#define TCK 2
#define TDI 3

int main()
{
    stdio_init_all();

    // Initialize the bootsel button
    // The bootsel button is GPIO 21 on Pico (with internal pull-up enabled)
    gpio_init(21);
    gpio_set_dir(21, GPIO_IN);
    gpio_pull_up(21);

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 1); // Turn on the onboard LED
    sleep_ms(500);
    gpio_put(PICO_DEFAULT_LED_PIN, 0); // Turn on the onboard LED
    sleep_ms(500);
    gpio_put(PICO_DEFAULT_LED_PIN, 1); // Turn on the onboard LED

    gpio_init(TDO);
    gpio_set_dir(TDO, GPIO_IN);
    gpio_init(TMS);
    gpio_set_dir(TMS, GPIO_OUT);
    gpio_init(TCK);
    gpio_set_dir(TCK, GPIO_OUT);
    gpio_init(TDI);
    gpio_set_dir(TDI, GPIO_OUT);  
    
    gpio_put(TCK, 0);  //clock init to 0 necessary!
    gpio_put(TMS, 1); 
    gpio_put(TDI, 1);

    gpio_init(MISO_O);
    gpio_set_dir(MISO_O, GPIO_IN);
    gpio_init(CSN_O);
    gpio_set_dir(CSN_O, GPIO_OUT);
    gpio_init(SCK_O);
    gpio_set_dir(SCK_O, GPIO_OUT);
    gpio_init(MOSI_O);
    gpio_set_dir(MOSI_O, GPIO_OUT);
    
    gpio_put(CSN_O, 1);
    gpio_put(SCK_O, 0);
    gpio_put(MOSI_O, 0);

    sleep_ms(10000);
    printf("Pico Ready\n");

    char c = 0;
    char r = 0;

    while (true) {
        c = stdio_getchar();
        
        if(c >> 3 == '0' >> 3) { // '0' to '7'
            c = (c & 0x07); //only keep lower 3 bits
            
            gpio_put(TDI, c & 1);
            gpio_put(TMS, c & 2);  
            r = (gpio_get(TDO)) ? '1' : '0'; //read TDO
            sleep_us(1); //todo may not be necessary
            gpio_put(TCK, 1);
            sleep_us(1);    
            gpio_put(TCK, 0);
            if(c & 4) stdio_putchar(r); //if bit 2 is set, return TDO value

        } else if(c >> 4 == 'a' >> 4) { // 'a' to 'o'
            int pin = (c >> 1) & 0x07;
            //printf("Setting CSN_O=%d SCK_O=%d MOSI_O=%d\n", (pin>>0)&1, (pin>>1)&1, (pin>>2)&1);
            gpio_put(SCK_O, (pin>>1)&1);   //clock first
            gpio_put(CSN_O, (pin>>0)&1);
            gpio_put(MOSI_O, (pin>>2)&1);
            stdio_putchar(gpio_get(MISO_O) ? '1' : '0'); //read MISO_O

        }else if(c == 'q') {
            printf("Entering bootsel mode...\n");
            sleep_ms(100);
            reset_usb_boot(0, 0); //go into bootsel mode

        } else {
            printf("Unknown command: %c\n", c);
        }
    }

    return 0;
}
