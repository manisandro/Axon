
**************************************************
* Axon readme                                    *
**************************************************

* INSTALLATION

- Base packages:
   avr-gcc avr-libc minicom

- Compile the bootloader
   Download the bootloader [1]
   cd <bootloader_dir>
   make

- /dev/ttyUSB0 permissions
  To not need root permissions for programming, run
    usermod -a -G dialout <username>

* WORKING WITH THE CONTROLLER

- Plug the controller in a USB port, recent kernels already contain the driver.
- Check if the BAUD rate of the serial port is correct [2]:
   stty -F /dev/ttyUSB0   (use /dev/ttyUSB<n> according to your particular system)
  The BAUD should be 115200, else set it:
   stty -F /dev/ttyUSB0 115200
- Connect to ttyUSB0 via minicom [3]:
   - run
      minicom -s
     choose Serial port setup and select the correct serial device node (here /dev/ttyUSB0)
   - Save configuration as def[ault] (optional)
   - run minicom, you should now be connected
   - To quit minicom, CTRL+A, then Z and finally Q

* PROGRAMMING

- Download Axon source files
- Edit the source files as you need
- Compile:
   make
- Assure your Axon is turned off, exit all minicom instances.
- Upload the program to the device:
   make program
- Turn on your Axon (i.e. connect power), programming should start (make sure there is no minicom instance running as it will lock the device!)
- Once programming is complete, your done: start minicom, reboot device.

* REFERENCES

[1] http://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_type=project&item_id=1927
[2] http://embedded.seattle.intel-research.net/wiki/index.php?title=Using_other_UARTs_in_Linux
[3] http://www.cyberciti.biz/tips/connect-soekris-single-board-computer-using-minicom.html
