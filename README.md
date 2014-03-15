A Wireless RFID access control system
=====================================

For my ELEC3609 assignment I built a RFID system that used a XBee to wirelessly communicate to
a python server that would allow or deny access.

It was designed to run on an LPC1343 and use an XBee 4214a for transmission

Features:

- Wireless connectivity
- Secure against replay attacks by encoding 4 random bytes that the server must mirror
- Encrypted communications: uses a very basic encryption scheme of C = P XOR PW but this could easily be
  switched out for a real encryption system.

Overview of the code:

    - Initialise
        - Setup GPIO DIR's and assignments.
        - Setup Timers etc
        - Setup XBee Module
            - Enter AT mode
            - Set Network ID
            - Set Source
            - Set Destination
        -Register Interupts

    - While 1:
        - Enter Sleep
        - Wait for UART interupt (RFID MODE)
        - Process RFID
        - Switch modes to XBee
        - Send/Recieve RFID data ( encrypted )
        - Light LED for 10 Seconds.
        - Switch modes to RFID

