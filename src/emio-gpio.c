#include <gpiod.h>   // libgpiod
#include <stdio.h>
#include <unistd.h> // might not need this lib
#include <time.h>   // might not need this lib, nanosleep()

/* 
Remember to reload FPGA image before running emio-gpio.c (can potentially automate this later):
    cd fpga/out/bitstream
    fpga-load
    cd ../../..
    cd src
    gcc emio-gpio.c -o ../build/emio-gpio.out -lgpiod
    ../build/emio-gpio.out
*/

// ------------------------------------ Constants ------------------------------------
#define NUM_LEDS 8
#define HALF_LEDS (NUM_LEDS / 2)
#define IO1 78
#define IO2 79
#define IO3 80
#define IO4 81
#define IO5 82
#define IO6 83
#define IO7 84
#define IO8 85

// ------------------------------------ Globals ------------------------------------
static char *chip_name         = "gpiochip1";
static struct gpiod_chip *chip = NULL;        // Note: If permission is denied, bypass (for now): 'sudo chmod 666 /dev/gpiochip*'
static struct gpiod_line *lines[NUM_LEDS];

// ------------------------------------ Functions ------------------------------------

static int gpio_init(void) {
    // Chip access
    chip = gpiod_chip_open_by_name(chip_name);
    if (!chip) { 
        perror("open chip"); 
        return 1; 
    }
    printf("Successfully opened chip %s.\n", chip_name);
    return 0;
}

static int get_lines(void) {
    // Line access, switches 78-85 (EMIO bit 0 = PMOD2 IO1-I08)
    for (int i = 0; i < NUM_LEDS; i++) {
        lines[i] = gpiod_chip_get_line(chip, IO1 + i);
        if (!lines[i]) {
            perror("access lines"); 
            gpiod_chip_close(chip); // Close chip on error
            break;                  // Stop trying to fetch lines if the first one fails
        }
    }

    printf("Successfully accessed lines: [");
    for (int i = 0; i < NUM_LEDS; i++) {
        printf("%d", gpiod_line_offset(lines[i])); // gpiod_line_offset() will pull our lines for us
        if (i <= NUM_LEDS - 1) { printf(", "); }   // Print a comma and space for all items except the last one
    }
    printf("].\n");

    for (int i = 0; i < NUM_LEDS; i++) {
        if (gpiod_line_request_output(lines[i], "emio-leds", 0) < 0) {
            perror("request line");
            gpiod_chip_close(chip); // Close chip on error
            break;
        }
    }
    return 0;
}

static int gpio_close(void) {
    for (int i = 0; i < NUM_LEDS; i++) {
        if (lines[i]) { gpiod_line_release(lines[i]); }
    }
    gpiod_chip_close(chip);
    return 0;
}

// ------------------------------------ Dances ------------------------------------

static void dance_1 () {
    for (int i = 0; i < HALF_LEDS; i++) {
        gpiod_line_set_value(lines[i], 1);
        sleep(1);
        gpiod_line_set_value(lines[i], 0);
    }
    for (int i = NUM_LEDS - 1; i >= HALF_LEDS; i--) {
        gpiod_line_set_value(lines[i], 1);
        sleep(1);
        gpiod_line_set_value(lines[i], 0);
    }
}

static void dance_2 () {
    for (int i = 0; i < HALF_LEDS; i++) {
        gpiod_line_set_value(lines[i], 1);
        gpiod_line_set_value(lines[i + HALF_LEDS], 1);
        sleep(1);
        gpiod_line_set_value(lines[i], 0);
        gpiod_line_set_value(lines[i + HALF_LEDS], 0);
    }
}

static void dance_3 () {
    for (int i = 0; i < HALF_LEDS; i++) {
        gpiod_line_set_value(lines[i], 1);
        gpiod_line_set_value(lines[(NUM_LEDS - 1) - i], 1);
        sleep(1);
        gpiod_line_set_value(lines[i], 0);
        gpiod_line_set_value(lines[(NUM_LEDS - 1) - i], 0);
    }
}

// ------------------------------------ Main ------------------------------------

int main (void) {

    // Init
    gpio_init();
    get_lines();

    // Dance
    dance_1();
    dance_2();
    dance_3();

    // Return lines and close chip
    gpio_close();

    return 0;
}