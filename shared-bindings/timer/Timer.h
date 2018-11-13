/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_TIMER_TIMER_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_TIMER_TIMER_H

#include "common-hal/timer/Timer.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t timer_timer_type;

// Initializes the hardware peripheral.
extern void common_hal_timer_timer_construct(timer_timer_obj_t *self,
                                               uint32_t period, bool one_shot);

extern void common_hal_timer_timer_deinit(timer_timer_obj_t *self);
extern bool common_hal_timer_timer_deinited(timer_timer_obj_t *self);

// Return elapsed time since timer last started (or expired, if periodic) [us]
extern uint32_t common_hal_timer_timer_get_elapsed_time(timer_timer_obj_t *self);

extern void common_hal_timer_timer_start(timer_timer_obj_t *self);
extern void common_hal_timer_timer_stop(timer_timer_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_TIMER_TIMER_H
