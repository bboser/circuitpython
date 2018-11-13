IoT-Python
====================================

**IoT-Python** adds features not currently supported in  **CircuitPython** (e.g. interrupts) and is otherwise identical. The motivation for the different name is to avoid confusion: code following the CircuitPython API (should) run on IoT-Python, but the opposite is not necessarily true.

`Adafruit's <https://www.adafruit.com>`_ **CircuitPython** is an open source derivative of `MicroPython <https://micropython.org>`_. CircuitPython features unified Python core APIs and a growing list of Adafruit libraries and drivers that work with it. Check Adafruit's `documentation <https://circuitpython.readthedocs.io>`_ and `GitHub repository <https://github.com/adafruit/circuitpython>`_ for information about CircuitPython.

Status
------

This project is experimental. Expect changes, except for CircuitPython APIs which should be stable going forward. Those that change will change on major version numbers such as 3.0.0 and 4.0.0.

Supported Boards
----------------

**nRF52840 Boards**

.. _content:

Content
----------------------

.. toctree::
   :maxdepth: 3
   :caption: API

   ../shared-bindings/index.rst
   ./drivers.rst
   ./library/index.rst
   MicroPython-Lib <https://github.com/micropython/micropython-lib>
   ./troubleshooting.rst
   ../license.rst

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
