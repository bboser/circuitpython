IoT-Python
====================================

**IoT-Python** is an experimental branch of `Adafruit's <https://www.adafruit.com>`_ **CircuitPython** and shares its `MicroPython <https://micropython.org>`_ ancestry.

Goal is to explore some features (e.g. low power) of the nRF52840 without concern of compatibility with other platforms.

The motivation for the different name is to avoid confusion: code following the CircuitPython API (should) run on IoT-Python, but the opposite is not necessarily true.

Status
------

This project is experimental. Expect changes; use CircuitPython for a more stable and mature environment.

Supported Boards
----------------

nRF52840

Change Log
----------

- ports/nrf/boards/particle_mesh
- timer module (including interrupts)
- digital io interrupts
- enable scheduler & emergency exception buffer
- these are used by eventio:

    - microcontroller.deepsleep(timeout)
    - _iot module
    - utime module

Known Bugs
----------

- Errors in interrupt handler crash VM (wish I knew why)
- Digitalinout.deinit does not "deinit" gpiote (only relevant for irq). Calling gpiote deinit crashes the VM.
