/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Bernhard Boser
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

#include "shared-bindings/timer/Timer.h"
#include "peripherals/nrf/timers.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

#include "nrfx_timer.h"

STATIC void timer_event_handler(nrf_timer_event_t event_type, void *p_context) {
    if (event_type != NRF_TIMER_EVENT_COMPARE0) return;
    timer_timer_obj_t *timer_obj = (timer_timer_obj_t*)p_context;
    mp_obj_t function = timer_obj->function;
    if (function != NULL) {
        if (timer_obj->fast) {
            mp_call_function_1(function, timer_obj);
        } else {
#if MICROPY_ENABLE_SCHEDULER
            if (!mp_sched_schedule(function, timer_obj)) {
                mp_raise_msg(&mp_type_RuntimeError, translate("schedule stack full"));
            }
#else
            mp_raise_ValueError(translate("scheduler not enabled, use fast interrupt"));
#endif
        }
    }
}

void common_hal_timer_timer_construct(timer_timer_obj_t *self,
                                           uint32_t interval, bool one_shot) {
    // Find a free timer instance.
    self->timer_instance = nrf_peripherals_allocate_timer();
    if (self->timer_instance == NULL) {
        mp_raise_RuntimeError(translate("All timers are in use"));
    }

    // Timer peripheral usage:
    // Every timer instance has a number of capture/compare (CC) registers.
    // These can store either the value to compare against (to trigger an
    // interrupt or a shortcut) or store a value returned from a
    // capture/compare event.
    // We use channel 0 for comparing (to trigger the function and clear
    // shortcut) and channel 1 for capturing the elapsed time.

    const nrfx_timer_config_t config = {
        .frequency = NRF_TIMER_FREQ_1MHz,
        .mode = NRF_TIMER_MODE_TIMER,
        .bit_width = NRF_TIMER_BIT_WIDTH_32,
        #ifdef NRF51
        .interrupt_priority = 3,
        #else
        .interrupt_priority = 6,
        #endif
        .p_context = self,
    };

    // Initialize the driver.
    // When it is already initialized, this is a no-op.
    nrfx_timer_init(self->timer_instance, &config, timer_event_handler);

    // Configure channel 0.
    nrf_timer_short_mask_t short_mask = NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK |
        (one_shot ? NRF_TIMER_SHORT_COMPARE0_STOP_MASK : 0);
    bool enable_interrupts = true;
    nrfx_timer_extended_compare(
            self->timer_instance,
            NRF_TIMER_CC_CHANNEL0,
            interval,
            short_mask,
            enable_interrupts);
}

void common_hal_timer_timer_deinit(timer_timer_obj_t *self) {
    common_hal_timer_timer_cancel(self);
    nrf_peripherals_free_timer(self->timer_instance);
    self->timer_instance = NULL;
}

bool common_hal_timer_timer_deinited(timer_timer_obj_t *self) {
    return self->timer_instance == NULL;
}

uint32_t common_hal_timer_timer_get_elapsed_time(timer_timer_obj_t *self) {
    uint32_t interval = nrfx_timer_capture(self->timer_instance, NRF_TIMER_CC_CHANNEL1);
    return interval;
}

void common_hal_timer_timer_start(timer_timer_obj_t *self) {
    nrfx_timer_enable(self->timer_instance);
}

void common_hal_timer_timer_cancel(timer_timer_obj_t *self) {
    nrfx_timer_disable(self->timer_instance);
}
