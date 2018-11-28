/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_ATOMIC_FIFO_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_ATOMIC_FIFO_H


#include "nrfx.h"

typedef uint32_t ret_code_t;
#define NRF_LOG_INSTANCE_PTR_DECLARE(_p_name)
#define NRF_LOG_INST_INFO(p_inst,...)
#define NRF_LOG_INST_ERROR(p_inst,...)
#define NRF_LOG_INST_WARNING(p_inst,...)
#define NRF_LOG_INST_DEBUG(p_inst,...)

#include "nrf_atfifo/nrf_error.h"
#include "nrf_atfifo/nrf_atfifo.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    nrf_atfifo_t fifo_instance;
} atomic_fifo_obj_t;

#endif // MICROPY_INCLUDED_NRF_COMMON_HAL_ATOMIC_FIFO_H
