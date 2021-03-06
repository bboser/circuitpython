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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_ATOMIC_FIFO_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_ATOMIC_FIFO_H

#include "common-hal/_iot/AtomicFIFO.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t atomic_fifo_type;

// Initializes the hardware peripheral.
extern void common_hal_atomic_fifo_construct(atomic_fifo_obj_t *self, mp_int_t size);

// Add item to the end of the fifo. Returns false if fifo is full.
extern bool common_hal_atomic_fifo_put(atomic_fifo_obj_t *self, mp_obj_t item);

// Remove oldest entry in fifo. Returns item or raises IndexError if fifo is empty.
extern mp_obj_t common_hal_atomic_fifo_get(atomic_fifo_obj_t *self);

// Returns number of items presently stored in fifo.
extern int32_t common_hal_atomic_fifo_len(atomic_fifo_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_ATOMIC_FIFO_H
