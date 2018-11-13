/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/runtime0.h"
#include "shared-bindings/timer/Chronometer.h"
#include "shared-bindings/util.h"

extern uint64_t common_hal_time_monotonic(void);

//| .. currentmodule:: timer
//|
//| :class:`Chronometer` -- Measure elapsed time
//| ====================================================================================
//|
//| Chronometer measures the time since creation or reset.
//|
//| .. class:: Chronometer()
//|
//|   Create a Chronometer and set the initial elapsed time to zero.
//|   Implemented in software. Heap allocation from constructor only.
//|   Milli-second resolution.
//|
//|   Compared to solutions based on `time.monotonic()`, the time resolution
//|   of `Chronometer` is relative to the period measured, not the time
//|   when the microcontroller was started (i.e. it does not suffer from
//|   decreasing accuracy for long power-on times.).
//|
//|   For example::
//|
//|     from timer import Chronometer
//|     from time import sleep
//|
//|     chrono = Chronometer()
//|     # do some work, e.g.
//|     sleep(0.53)
//|     print("Elapsed time: {} seconds".format(chrono.elapsed_time)
//|
STATIC mp_obj_t timer_chronometer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, true);

    timer_chronometer_obj_t *self = m_new_obj(timer_chronometer_obj_t);
    self->base.type = &timer_chronometer_type;
    self->start_time = common_hal_time_monotonic();

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: deinit()
//|
//|      Deinitializes the Chronometer and releases any hardware resources for reuse.
//|
STATIC mp_obj_t timer_chronometer_deinit(mp_obj_t self_in) {
    timer_chronometer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->start_time = 0;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(timer_chronometer_deinit_obj, timer_chronometer_deinit);

//|   .. method:: __enter__()
//|
//|      No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: __exit__()
//|
//|      Automatically deinitializes the hardware when exiting a context. See
//|      :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t timer_chronometer_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return timer_chronometer_deinit(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(timer_chronometer___exit___obj, 4, 4, timer_chronometer_obj___exit__);


//|   .. attribute:: elapsed_time
//|
//|     Elapsed time since construction or reset in seconds (float).
//|
STATIC mp_obj_t timer_chronometer_obj_get_elapsed_time(mp_obj_t self_in) {
    timer_chronometer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(self->start_time == 0);
    float td = common_hal_time_monotonic() - self->start_time;
    return mp_obj_new_float(td/1000.0);
}
MP_DEFINE_CONST_FUN_OBJ_1(timer_chronometer_get_elapsed_time_obj, timer_chronometer_obj_get_elapsed_time);

//|   .. method:: reset
//|
//|     Reset chronometer to restart measuring now.
//|
STATIC mp_obj_t timer_chronometer_obj_reset(mp_obj_t self_in) {
    timer_chronometer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(self->start_time == 0);
    self->start_time = common_hal_time_monotonic();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(timer_chronometer_reset_obj, timer_chronometer_obj_reset);

const mp_obj_property_t timer_chronometer_elapsed_time_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&timer_chronometer_get_elapsed_time_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t timer_chronometer_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&timer_chronometer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&timer_chronometer___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&timer_chronometer_reset_obj) },
    // Properties
    { MP_ROM_QSTR(MP_QSTR_elapsed_time), MP_ROM_PTR(&timer_chronometer_elapsed_time_obj) },
};
STATIC MP_DEFINE_CONST_DICT(timer_chronometer_locals_dict, timer_chronometer_locals_dict_table);

const mp_obj_type_t timer_chronometer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Chronometer,
    .make_new = timer_chronometer_make_new,
    .locals_dict = (mp_obj_dict_t*)&timer_chronometer_locals_dict,
};
