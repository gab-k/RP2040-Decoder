Debugging and Logging
==================================

For logging and debugging you have to build the software yourself. This is explained here: :ref:`building_the_software`

.. _debugging_via_swd:

Debugging via SWD
----------------------------------
Please refer to :ref:`flashing_the_software_swd`.


.. _logging:

Logging
----------------------------------

Logging can be enabled in ``CMakeLists.txt``, and is available with two interfaces:

    - Logging via UART RX/TX pins (GPIO0/GPIO1 by default)
    - Logging via USB CDC/ACM

Additionally you can make the decoder wait for input via serial port by setting LOG_WAIT to 1. For more details please see comments inside ``CMakeLists.txt``.
