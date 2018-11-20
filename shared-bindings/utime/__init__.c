/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Josef Gajdusek
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include <string.h>

#include "py/obj.h"
#include "py/objnamedtuple.h"
#include "py/runtime.h"
#include "py/smallint.h"
#include "lib/timeutils/timeutils.h"
#include "shared-bindings/rtc/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/shared/translate.h"

extern uint64_t common_hal_time_monotonic(void);

//| :mod:`utime` --- low level timing functions
//| ========================================================
//|
//| .. module:: utime
//|   :synopsis: very specialized functions to address hardware limitations
//|   :platform: nRF52840
//|
//| Dealing with time on 32-bit microcontrollers poses problems. E.g. time with
//| milli-second resolution when truncated to 32-bits rolls over every ~50 days.
//| Single precision floats representing time in seconds have only ~22-bit
//| resolution and retain milli-second resolution for only ~ one hour.
//| Representing time with arbitrary length integers incurs overhead that is
//| not always acceptable.
//|
//| This library, copied from `micropython`, provides ms-resolution time
//| `ticks_ms` represented with small ints that roll over and `ticks_diff`
//| and `ticks_add` to deal with this limitation under some circumstances.
//| It is not intended for general purpose use.

//|
//| .. method:: ticks_ms()
//|
//|   Returns time with ms resolution that wraps around.
//|
//|   :return: time with arbitrary reference, wraps around
//|   :rtype: int
//|
STATIC mp_obj_t utime_ticks_ms(void) {
    return MP_OBJ_NEW_SMALL_INT(
        ((uint32_t)common_hal_time_monotonic())
      & (MICROPY_PY_UTIME_TICKS_PERIOD - 1));
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_utime_ticks_ms_obj, utime_ticks_ms);

//|
//| .. method:: ticks_diff(end, start)
//|
//|   Returns `end-start`, taking into account a (single) wrap around.
//|   `end` and `start` are values returned from `ticks_ms`.
//|
//|   :return: time difference
//|   :rtype: int
//|
STATIC mp_obj_t utime_ticks_diff(mp_obj_t end_in, mp_obj_t start_in) {
    // we assume that the arguments come from ticks_xx so are small ints
    mp_uint_t start = MP_OBJ_SMALL_INT_VALUE(start_in);
    mp_uint_t end = MP_OBJ_SMALL_INT_VALUE(end_in);
    // Optimized formula avoiding if conditions. We adjust difference "forward",
    // wrap it around and adjust back.
    mp_int_t diff =
        ((end - start + MICROPY_PY_UTIME_TICKS_PERIOD / 2)
      & (MICROPY_PY_UTIME_TICKS_PERIOD - 1))
      - MICROPY_PY_UTIME_TICKS_PERIOD / 2;
    return MP_OBJ_NEW_SMALL_INT(diff);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_utime_ticks_diff_obj, utime_ticks_diff);

//|
//| .. method:: ticks_diff(start, delta)
//|
//|   Returns `start+delta` wrapping around if the sum does not fit in an `int`.
//|
//|   :return: time sum
//|   :rtype: int
//|
STATIC mp_obj_t utime_ticks_add(mp_obj_t ticks_in, mp_obj_t delta_in) {
    // we assume that first argument come from ticks_xx so is small int
    mp_uint_t ticks = MP_OBJ_SMALL_INT_VALUE(ticks_in);
    mp_uint_t delta = mp_obj_get_int(delta_in);
    return MP_OBJ_NEW_SMALL_INT(
        (ticks + delta) & (MICROPY_PY_UTIME_TICKS_PERIOD - 1));
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_utime_ticks_add_obj, utime_ticks_add);


STATIC const mp_rom_map_elem_t utime_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_utime) },
    { MP_ROM_QSTR(MP_QSTR_ticks_ms), MP_ROM_PTR(&mp_utime_ticks_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_diff), MP_ROM_PTR(&mp_utime_ticks_diff_obj) },
    { MP_ROM_QSTR(MP_QSTR_ticks_add), MP_ROM_PTR(&mp_utime_ticks_add_obj) },
};

STATIC MP_DEFINE_CONST_DICT(utime_module_globals, utime_module_globals_table);

const mp_obj_module_t utime_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&utime_module_globals,
};
