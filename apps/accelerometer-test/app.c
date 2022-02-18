#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "watch.h"
#include "lis2dw.h"

static void cb_light_pressed(void) {
}

static void cb_mode_pressed(void) {
}

static void cb_alarm_pressed(void) {
}

uint8_t interrupts = 0;
uint8_t last_interrupts = 0;
uint8_t ticks = 0;
char buf[13] = {0};

static void cb_tick(void) {
    watch_clear_indicator(WATCH_INDICATOR_SIGNAL);
    if (!lis2dw_have_new_data()) return;

    watch_set_indicator(WATCH_INDICATOR_SIGNAL);
    lis2dw_fifo_t fifo;
    bool overrun = lis2dw_read_fifo(&fifo);
    printf("FIFO captured %d readings.\n", fifo.count);
    if (overrun) printf("\tThere was an overrun!\n\n");

    printf("%d, %d, %d\n", fifo.readings[0].x, fifo.readings[0].y, fifo.readings[0].z);
    printf("%d, %d, %d\n", fifo.readings[10].x, fifo.readings[10].y, fifo.readings[10].z);
    printf("%d, %d, %d\n", fifo.readings[20].x, fifo.readings[20].y, fifo.readings[20].z);
}

void app_init(void) {
    watch_enable_display();
    watch_display_string("AC  Strean", 0);

    watch_enable_external_interrupts();
    watch_register_interrupt_callback(BTN_MODE, cb_mode_pressed, INTERRUPT_TRIGGER_RISING);
    watch_register_interrupt_callback(BTN_LIGHT, cb_light_pressed, INTERRUPT_TRIGGER_RISING);
    watch_register_interrupt_callback(BTN_ALARM, cb_alarm_pressed, INTERRUPT_TRIGGER_RISING);

    watch_enable_i2c();
    lis2dw_begin();
    lis2dw_set_low_power_mode(LIS2DW_LP_MODE_2); // lowest power 14-bit mode, 25 Hz is 3.5 µA @ 1.8V w/ low noise, 3µA without
    lis2dw_set_low_noise_mode(true); // consumes a little more power
    lis2dw_set_range(LIS2DW_CTRL6_VAL_RANGE_4G);
    lis2dw_set_data_rate(LIS2DW_DATA_RATE_25_HZ); // is this enough for training?

    lis2dw_enable_fifo();

    watch_rtc_register_periodic_callback(cb_tick, 1);
}

void app_wake_from_backup(void) {
}

void app_setup(void) {
}

void app_prepare_for_standby(void) {
}

void app_wake_from_standby(void) {
}

bool app_loop(void) {
    // TODO: interrupt configuration for LIS2DW
    watch_display_string(buf, 0);

    return true;
}