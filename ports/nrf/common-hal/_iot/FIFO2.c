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

#include "shared-bindings/atomic/FIFO2.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/objlist.h"
#include "supervisor/shared/translate.h"

void common_hal_atomic_fifo2_construct(atomic_fifo2_obj_t *self, mp_int_t size) {
    uint32_t item_size = 2*sizeof(mp_obj_t);
    uint32_t buf_size = (size+1) * item_size;
    void *buf = m_malloc(buf_size, true);
    ret_code_t err_code = nrf_atfifo_init(&(self->fifo2_instance), buf, buf_size, item_size);
    if (err_code != NRF_SUCCESS) mp_raise_ValueError(translate("init failed"));
}

void common_hal_atomic_fifo2_put(atomic_fifo2_obj_t *self, mp_obj_t item1, mp_obj_t item2) {
    nrf_atfifo_item_put_t context;
    mp_obj_t *dst;
    if ((dst = nrf_atfifo_item_alloc(&(self->fifo2_instance), &context)) != NULL) {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS
        *dst     = item1;
        *(dst+1) = item2;
        nrf_atfifo_item_put(&(self->fifo2_instance), &context);
    } else {
        mp_raise_IndexError(translate("fifo full"));
    }
}

void common_hal_atomic_fifo2_get(atomic_fifo2_obj_t *self, mp_obj_list_t *ret) {
    ret_code_t err_code = nrf_atfifo_get_free(&(self->fifo2_instance), ret->items, 2*sizeof(mp_obj_t), NULL);
    if (err_code != NRF_SUCCESS) mp_raise_IndexError(translate("fifo empty"));
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS   CHECK THIS
}
