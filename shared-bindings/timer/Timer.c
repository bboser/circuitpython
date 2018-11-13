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
#include "shared-bindings/timer/Timer.h"
#include "shared-bindings/util.h"

enum {
    TIMER_MODE_ONESHOT,
    TIMER_MODE_PERIODIC,
};


//| .. currentmodule:: timer
//|
//| :class:`Timer` -- Timed code execution via callbacks
//| ====================================================================================
//|
//| Timer executes a callback either periodically or after a delay. It can
//| also be used to measure elapsed time with micro-second resolution.
//| Uses hardware timers.
//|
//| .. class:: Timer(\*, period=1000000, mode=Timer.MODE_PERIODIC, callback=None, fast=False)
//|
//|   Create timer.
//|
//|   :param ~int period: period (or delay for MODE_ONESHOT) in micro-seconds.
//|   :param ~timer.Timer mode: MODE_PERIODIC (default) or MODE_ONESHOT.
//|   :param callback: Method called when timer expires.
//|   :param ~boolean fast: Decreased latency. WARNING: memory allocation not permitted in callback function.
//|
//|   For example::
//|
//|     from timer import Timer
//|
//|     def cb(timer):
//|         # Called when timer expires. Do whatever but keep it short!
//|         # ATTENTION: no memory allocation if fast==True. Use with caution.
//|         pass
//|
//|     # create timer that calls `cb` every 5000 micro-seconds
//|     t = Timer(period=5000, mode=Timer.MODE_PERIODIC, callback=cb)
//|     t.start()
//|
STATIC mp_obj_t timer_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_period, ARG_mode, ARG_callback, ARG_fast };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_period,   MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int  = 1000000} }, // 1 second
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int  = TIMER_MODE_PERIODIC} },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj  = mp_const_none} },
        { MP_QSTR_fast,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // create timer
    timer_timer_obj_t *self = m_new_obj(timer_timer_obj_t);
    self->base.type = &timer_timer_type;
    self->fast = args[ARG_fast].u_bool;

    // callback
    if (MP_OBJ_IS_FUN(args[ARG_callback].u_obj)) {
        self->callback = args[ARG_callback].u_obj;
    } else if (args[ARG_callback].u_obj == mp_const_none) {
        self->callback = NULL;
    } else {
        mp_raise_ValueError(translate("callback must be a function"));
    }

    common_hal_timer_timer_construct(self, args[ARG_period].u_int,
                                   args[ARG_mode].u_int == TIMER_MODE_ONESHOT);
    return (mp_obj_t)self;
}

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
STATIC mp_obj_t timer_timer_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_timer_timer_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(timer_timer___exit___obj, 4, 4, timer_timer_obj___exit__);

//|   .. method:: deinit()
//|
//|      Deinitializes the Timer and releases any hardware resources for reuse.
//|
STATIC mp_obj_t timer_timer_deinit(mp_obj_t self_in) {
    timer_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->callback = NULL;
    common_hal_timer_timer_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(timer_timer_deinit_obj, timer_timer_deinit);

//|   .. attribute:: elapsed_time
//|
//|     Elapsed time in micro-seconds (int).
//|
STATIC mp_obj_t timer_timer_obj_get_elapsed_time(mp_obj_t self_in) {
    timer_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_timer_timer_deinited(self));
    return MP_OBJ_NEW_SMALL_INT(common_hal_timer_timer_get_elapsed_time(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(timer_timer_get_elapsed_time_obj, timer_timer_obj_get_elapsed_time);

//|   .. method:: start
//|
//|     Start timer. Restart if already running.
//|
STATIC mp_obj_t timer_timer_obj_start(mp_obj_t self_in) {
    timer_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_timer_timer_deinited(self));
    common_hal_timer_timer_start(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(timer_timer_start_obj, timer_timer_obj_start);

//|   .. method:: stop
//|
//|     Stop timer.
//|
STATIC mp_obj_t timer_timer_obj_stop(mp_obj_t self_in) {
    timer_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_timer_timer_deinited(self));
    common_hal_timer_timer_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(timer_timer_stop_obj, timer_timer_obj_stop);

const mp_obj_property_t timer_timer_elapsed_time_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&timer_timer_get_elapsed_time_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t timer_timer_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit),   MP_ROM_PTR(&timer_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&timer_timer___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_start),    MP_ROM_PTR(&timer_timer_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),     MP_ROM_PTR(&timer_timer_stop_obj) },
    // Properties
    { MP_ROM_QSTR(MP_QSTR_elapsed_time), MP_ROM_PTR(&timer_timer_elapsed_time_obj) },
    // Constants
    { MP_ROM_QSTR(MP_QSTR_ONESHOT),  MP_ROM_INT(TIMER_MODE_ONESHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};
STATIC MP_DEFINE_CONST_DICT(timer_timer_locals_dict, timer_timer_locals_dict_table);

const mp_obj_type_t timer_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .make_new = timer_timer_make_new,
    .locals_dict = (mp_obj_dict_t*)&timer_timer_locals_dict,
};
