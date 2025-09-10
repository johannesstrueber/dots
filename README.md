## Overview

DOTS is a compact, 6-channel Eurorack module designed for generating triggers and gates. This open hardware project allows you to build the module yourself, with instructions and resources provided. Please visit the [website](http://www.d-o-t-s.net) for more info
<p align="left">
  <img src="/gh-images/frontside.jpg" alt="frontside" width="210"/>
  <img src="/gh-images/backside.jpg" alt="backside" width="210"/>
  <img src="/gh-images/pcb.jpg" alt="pcb" width="210"/>
</p>

## Features

- **6-Channel Trigger/Gate**: Generate triggers and gates for your modular system.
- **Compact Size**: Only 6hp wide, making it a handy addition to your setup.
- **Sync Capabilities**: Includes a clock and reset input, allowing it to function as a master or slave.
- **Visual Feedback**: Equipped with a screen and an UI.
- **Multiple Programs**: The firmware features multiple programs for different trigger/gate patterns and functionalities.

## Instructions

Check the [website](http://www.d-o-t-s.net) for detailed instructions on how to order, build and flash the module.

## Resources

- **[Website](http://www.d-o-t-s.net)**: Main website for the project.
- **[Order Guide](http://www.d-o-t-s.net/order)**: Order your DOTS PCB and front panel.
- **[Build Guide](http://www.d-o-t-s.net/build)**: Detailed instructions and an interactive Bill of Materials.
- **[Installation Guide](http://www.d-o-t-s.net/installation)**: Instructions for flashing the firmware to your DOTS module.
- **[Manual](http://www.d-o-t-s.net/manual)**: Comprehensive manual for using the module.
- **[Firmware on GitHub](https://www.github.com/releases)**: Download the latest firmware for the module.
- **[Modulargrid.net](https://modulargrid.net/e/other-unknown-dots)**: Module page on Modulargrid.net.

## Structure

- **/code/include**: Header files and module implementations
- **/code/src/main.cpp**: Main firmware entry point
- **/code/src/eeprom_config.cpp**: Default settings and pattern configuration
- **/kicad_frontpanel**: Front panel PCB design files ([KiCad](https://kicad.org))
- **/kicad_main**: Main PCB design files ([KiCad](https://kicad.org))
- **diagram.json**: Hardware simulation ([WOKWI](https://wokwi.com))
- **platformio.ini**: Build configuration ([PlatformIO](https://platformio.org))
- **flash_sequencer.sh**: Automated flashing script with EEPROM configuration

## Disclaimer

This is a personal project and not a commercial product. The creator is not liable for any damage caused by the module. Use at your own risk. The project is licensed under a [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

If you have any questions, feel free to contact the creator. This is a hobby project, and the creator is a web developer, not a professional hardware embedded systems builder; some hardware design or code decisions may seem unconventional.

Special thanks to HAGIWO for providing foundational knowledge.

Happy Building!


[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg
