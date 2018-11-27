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

#define MP_OBJ_IS_METH(o) (MP_OBJ_IS_OBJ(o) && (((mp_obj_base_t*)MP_OBJ_TO_PTR(o))->type->name == MP_QSTR_bound_method))

enum {
    TIMER_MODE_ONESHOT,
    TIMER_MODE_PERIODIC,
};


//| .. currentmodule:: timer
//|
//| :class:`Timer` -- Timed code execution via functions
//| ====================================================================================
//|
//| Timer executes a function function after a delay or periodically. It can
//| also be used to measure elapsed time with micro-second resolution.
//| Uses hardware timers. Loosely follows `threading.Timer` API.
//|
//| .. class:: Timer(\*, interval=1.0, function=None, mode=Timer.ONESHOT, fast=False)
//|
//|   Create timer.
//|
//|   :param ~float interval: period (or delay for ONESHOT) in seconds. Maximum: 3600.
//|   :param function: A function that is called when timer expires.
//|   :param ~timer.Timer mode: ONESHOT (default) or PERIODIC.
//|   :param ~boolean fast: Decreased latency. WARNING: memory allocation not permitted in function function.
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
//|     # create timer that calls `cb` every 0.5 seconds
//|     t = Timer(interval=0.5, function=cb, mode=Timer.PERIODIC)
//|     t.start()
//|
STATIC mp_obj_t timer_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_interval, ARG_function, ARG_mode, ARG_fast };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj  = mp_const_none} },
        { MP_QSTR_function, MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj  = mp_const_none} },
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int  = TIMER_MODE_ONESHOT} },
        { MP_QSTR_fast,     MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // create timer
    timer_timer_obj_t *self = m_new_obj(timer_timer_obj_t);
    self->base.type = &timer_timer_type;
    self->fast = args[ARG_fast].u_bool;

    // callback function
    if (MP_OBJ_IS_FUN(args[ARG_function].u_obj) || MP_OBJ_IS_METH(args[ARG_function].u_obj)) {
        self->function = args[ARG_function].u_obj;
    } else if (args[ARG_function].u_obj == mp_const_none) {
        self->function = NULL;
    } else {
        mp_raise_ValueError(translate("function must be a function"));
    }

    // convert interval to 32-bits
    float interval = 1.0;
    mp_obj_t interval_obj = args[ARG_interval].u_obj;
    if (interval_obj != mp_const_none) interval = mp_obj_get_float(interval_obj);
    if (interval <= 0) mp_raise_ValueError(translate("interval must be positive"));
    if (interval > 4294.967) mp_raise_ValueError(translate("interval must be <= 3600"));

    common_hal_timer_timer_construct(self, (uint32_t)(interval*1000000),
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
    self->function = NULL;
    common_hal_timer_timer_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(timer_timer_deinit_obj, timer_timer_deinit);

//|   .. attribute:: elapsed_time
//|
//|     Elapsed time in seconds (float). Micro-second resolution.
//|
STATIC mp_obj_t timer_timer_obj_get_elapsed_time(mp_obj_t self_in) {
    timer_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_timer_timer_deinited(self));
    return mp_obj_new_float(common_hal_timer_timer_get_elapsed_time(self)/1000000.0);
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

//|   .. method:: cancel
//|
//|     Cancel timer.
//|
STATIC mp_obj_t timer_timer_obj_cancel(mp_obj_t self_in) {
    timer_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    raise_error_if_deinited(common_hal_timer_timer_deinited(self));
    common_hal_timer_timer_cancel(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(timer_timer_cancel_obj, timer_timer_obj_cancel);

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
    { MP_ROM_QSTR(MP_QSTR_cancel),     MP_ROM_PTR(&timer_timer_cancel_obj) },
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
