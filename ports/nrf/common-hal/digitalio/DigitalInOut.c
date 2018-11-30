/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

void common_hal_digitalio_digitalinout_never_reset(
        digitalio_digitalinout_obj_t *self) {
    never_reset_pin_number(self->pin->number);
}

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
        digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {
    claim_pin(pin);
    self->pin = pin;
    nrf_gpio_cfg_input(pin->number, NRF_GPIO_PIN_NOPULL);

    return DIGITALINOUT_OK;
}

// REMOVE THIS
// STATIC mp_obj_t pin_handler;

// The reason that the saved pointer needs to go into MP_STATE_PORT is so that
// the garbage collector won't collect the callback function.
// https://forum.micropython.org/viewtopic.php?t=4379

STATIC void common_hal_fast_irq_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    mp_obj_t pin_handler = MP_STATE_PORT(pin_irq_handlers)[pin];
    bool value = (nrf_gpio_pin_dir_get(pin) == NRF_GPIO_PIN_DIR_INPUT)
        ? nrf_gpio_pin_read(pin)
        : nrf_gpio_pin_out_read(pin);
    mp_call_function_1(pin_handler, mp_obj_new_bool(value));
}

STATIC void common_hal_port_irq_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    mp_obj_t pin_handler = MP_STATE_PORT(pin_irq_handlers)[pin];
    bool value = (nrf_gpio_pin_dir_get(pin) == NRF_GPIO_PIN_DIR_INPUT)
        ? nrf_gpio_pin_read(pin)
        : nrf_gpio_pin_out_read(pin);
#if MICROPY_ENABLE_SCHEDULER
    if (!mp_sched_schedule(pin_handler, mp_obj_new_bool(value))) {
        mp_raise_msg(&mp_type_RuntimeError, translate("schedule stack full"));
    }
#else
    mp_raise_ValueError(translate("scheduler not enabled, use fast interrupt"));
#endif
}

// BUG: fast=False crashes VM
// https://devzone.nordicsemi.com/f/nordic-q-a/18052/gpiote-handler-module-or-gpiote-driver-for-port-interrupt/69597#69597
void common_hal_digitalio_digitalinout_irq(digitalio_digitalinout_obj_t *self, mp_obj_t handler, int trigger, bool fast) {
    nrfx_gpiote_pin_t pin = self->pin->number;

    if (nrf_gpio_pin_dir_get(pin) == NRF_GPIO_PIN_DIR_OUTPUT) {
        mp_raise_AttributeError(translate("Cannot attach handler to output pin"));
    }

    // initialize gpiote
    if (!nrfx_gpiote_is_init()) nrfx_gpiote_init();

    // configure pin, defalt is
    nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);  // fast);

    // trigger polarity
    if (trigger == 1) config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
    if (trigger == 2) config.sense = NRF_GPIOTE_POLARITY_HITOLO;

    // match pull to current setting
    // Changes pin to be a relative pin number in port.
    uint32_t pin_ = pin;
    NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin_);
    config.pull = ((reg->PIN_CNF[pin_] & GPIO_PIN_CNF_PULL_Msk) >> GPIO_PIN_CNF_PULL_Pos);

    // set interrupt handler
    nrfx_gpiote_evt_handler_t irq_handler = fast ? common_hal_fast_irq_handler : common_hal_port_irq_handler;
    nrfx_err_t err_code = nrfx_gpiote_in_init(pin, &config, irq_handler);
    if (err_code == NRFX_ERROR_INVALID_STATE) {
        nrfx_gpiote_in_uninit(pin);
        err_code = nrfx_gpiote_in_init(pin, &config, irq_handler);
    }
    if (err_code == NRFX_ERROR_NO_MEM) mp_raise_RuntimeError(translate("all irq channels in use"));
    if (err_code != NRFX_SUCCESS) mp_raise_RuntimeError(translate("failed to init GPIOTE for pin"));
    // enable interrupts
    nrfx_gpiote_in_event_enable(pin, true);

    // success, assign handler
    MP_STATE_PORT(pin_irq_handlers)[pin] = handler;
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self) {
    return self->pin == mp_const_none;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }
    int pn = self->pin->number;
    nrfx_gpiote_in_uninit(pn);
    nrf_gpio_cfg_default(pn);
    reset_pin_number(pn);
    MP_STATE_PORT(pin_irq_handlers)[pn] = NULL;

    self->pin = mp_const_none;
}

void common_hal_digitalio_digitalinout_switch_to_input(
        digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    nrf_gpio_cfg_input(self->pin->number, NRF_GPIO_PIN_NOPULL);
    common_hal_digitalio_digitalinout_set_pull(self, pull);
}

void common_hal_digitalio_digitalinout_switch_to_output(
        digitalio_digitalinout_obj_t *self, bool value,
        digitalio_drive_mode_t drive_mode) {

    common_hal_digitalio_digitalinout_set_drive_mode(self, drive_mode);
    common_hal_digitalio_digitalinout_set_value(self, value);
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
        digitalio_digitalinout_obj_t *self) {

    return (nrf_gpio_pin_dir_get(self->pin->number) == NRF_GPIO_PIN_DIR_INPUT)
        ? DIRECTION_INPUT : DIRECTION_OUTPUT;
}

void common_hal_digitalio_digitalinout_set_value(
        digitalio_digitalinout_obj_t *self, bool value) {
    nrf_gpio_pin_write(self->pin->number, value);
}

bool common_hal_digitalio_digitalinout_get_value(
        digitalio_digitalinout_obj_t *self) {
    return (nrf_gpio_pin_dir_get(self->pin->number) == NRF_GPIO_PIN_DIR_INPUT)
        ? nrf_gpio_pin_read(self->pin->number)
        : nrf_gpio_pin_out_read(self->pin->number);
}

void common_hal_digitalio_digitalinout_set_drive_mode(
        digitalio_digitalinout_obj_t *self,
        digitalio_drive_mode_t drive_mode) {
    nrf_gpio_cfg(self->pin->number,
                 NRF_GPIO_PIN_DIR_OUTPUT,
                 NRF_GPIO_PIN_INPUT_DISCONNECT,
                 NRF_GPIO_PIN_NOPULL,
                 drive_mode == DRIVE_MODE_OPEN_DRAIN ? NRF_GPIO_PIN_H0D1 : NRF_GPIO_PIN_H0H1,
                 NRF_GPIO_PIN_NOSENSE);
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
        digitalio_digitalinout_obj_t *self) {
    uint32_t pin = self->pin->number;
    // Changes pin to be a relative pin number in port.
    NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);

    switch ((reg->PIN_CNF[pin] & GPIO_PIN_CNF_DRIVE_Msk) >> GPIO_PIN_CNF_DRIVE_Pos) {
    case NRF_GPIO_PIN_S0D1:
    case NRF_GPIO_PIN_H0D1:
        return DRIVE_MODE_OPEN_DRAIN;
    default:
        return DRIVE_MODE_PUSH_PULL;
    }
}

void common_hal_digitalio_digitalinout_set_pull(
        digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    nrf_gpio_pin_pull_t hal_pull = NRF_GPIO_PIN_NOPULL;

    switch (pull) {
        case PULL_UP:
            hal_pull = NRF_GPIO_PIN_PULLUP;
            break;
        case PULL_DOWN:
            hal_pull = NRF_GPIO_PIN_PULLDOWN;
            break;
        case PULL_NONE:
        default:
            break;
    }

    nrf_gpio_cfg_input(self->pin->number, hal_pull);
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
        digitalio_digitalinout_obj_t *self) {
    uint32_t pin = self->pin->number;
    // Changes pin to be a relative pin number in port.
    NRF_GPIO_Type *reg = nrf_gpio_pin_port_decode(&pin);

    if (nrf_gpio_pin_dir_get(self->pin->number) == NRF_GPIO_PIN_DIR_OUTPUT) {
        mp_raise_AttributeError(translate("Cannot get pull while in output mode"));
    }

    switch ((reg->PIN_CNF[pin] & GPIO_PIN_CNF_PULL_Msk) >> GPIO_PIN_CNF_PULL_Pos) {
        case NRF_GPIO_PIN_PULLUP:
            return PULL_UP;
        case NRF_GPIO_PIN_PULLDOWN:
            return PULL_DOWN;
        default:
            return PULL_NONE;
    }
}
