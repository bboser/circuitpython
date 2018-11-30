IoT-Python
====================================

**IoT-Python** is derived from `Adafruit's <https://www.adafruit.com>`_ **CircuitPython** and shares its `MicroPython <https://micropython.org>`_ ancestry, but follows a different design philosophy: while CircuitPython strives to provide the same API on many different ports, IoT-Python provides unique features of the underlying hardware (the nRF52840 is the only target for now), at the risk of code portability.

The motivation for the different name is to avoid confusion: code following the CircuitPython API (should) run on IoT-Python, but the opposite is not necessarily true.

Status
------

This project is experimental. Expect changes; use CircuitPython for a more stable and mature environment.

Supported Boards
----------------

nRF52840

Change Log
----------

- add boards/particle_mesh
- add timer module (including interrupts)
- enable scheduler & emergency exception buffer
- add microcontroller.deepsleep()
- add _iot module

Known Bugs
----------

- Errors in interrupt handler crash VM (wish I knew why)
