/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <stdint.h>
#include <string.h>

#include "lib/utils/context_manager_helpers.h"

#include "py/nlr.h"
#include "py/objtype.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

//| .. currentmodule:: digitalio
//|
//| :class:`Polarity` -- defines the irq trigger polarity of a digital pin
//| =============================================================
//|
//| .. class:: digitalio.DigitalInOut.Polarity
//|
//|     Enum-like class to define which Polarity triggering the irq handler of a pin.
//|
//|     .. data:: HI2LO
//|     .. data:: LO2HI
//|     .. data:: BOTH
//|
const mp_obj_type_t digitalio_polarity_type;

const digitalio_polarity_obj_t digitalio_polarity_hi2lo_obj = {
    { &digitalio_polarity_type },
};

const digitalio_polarity_obj_t digitalio_polarity_lo2hi_obj = {
    { &digitalio_polarity_type },
};

const digitalio_polarity_obj_t digitalio_polarity_both_obj = {
    { &digitalio_polarity_type },
};

STATIC const mp_rom_map_elem_t digitalio_polarity_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_FALLING_EDGE),               MP_ROM_PTR(&digitalio_polarity_hi2lo_obj) },
    { MP_ROM_QSTR(MP_QSTR_RISING_EDGE),                MP_ROM_PTR(&digitalio_polarity_lo2hi_obj) },
    { MP_ROM_QSTR(MP_QSTR_RISING_OR_FALLING_EDGE),     MP_ROM_PTR(&digitalio_polarity_both_obj) },
};
STATIC MP_DEFINE_CONST_DICT(digitalio_polarity_locals_dict, digitalio_polarity_locals_dict_table);

STATIC void digitalio_polarity_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr polarity = MP_QSTR_BOTH;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&digitalio_polarity_hi2lo_obj)) {
        polarity = MP_QSTR_HI2LO;
    }
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&digitalio_polarity_lo2hi_obj)) {
        polarity = MP_QSTR_LO2HI;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_digitalio, MP_QSTR_Polarity, polarity);
}

const mp_obj_type_t digitalio_polarity_type = {
    { &mp_type_type },
    .name = MP_QSTR_Polarity,
    .print = digitalio_polarity_print,
    .locals_dict = (mp_obj_t)&digitalio_polarity_locals_dict,
};
