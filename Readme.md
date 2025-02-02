# TeensyFlexIO
## Credits
This project builds upon and extends the wonderful work of KurtE, whose original implementation can be found at https://github.com/KurtE/FlexIO_t4. We extend our sincere gratitude for providing such an excellent foundation for this work.

## Overview
TeensyFlexIO is a high-level API designed specifically for interfacing with the NXP FlexIO peripheral on Teensy 4/4.1 microcontrollers. This project aims to simplify the complexity of FlexIO programming by providing an intuitive and readable interface, making it more accessible for both learning and practical applications. Building upon the excellent foundation laid by KurtE's FlexIO_t4 library, this implementation focuses on clarity and ease of use while maintaining the powerful capabilities of the FlexIO peripheral.

The library serves as a comprehensive wrapper that abstracts the intricate details of FlexIO configuration, allowing developers to focus on their application logic rather than low-level peripheral setup. Whether you're implementing custom communication protocols or exploring the capabilities of the FlexIO peripheral, TeensyFlexIO provides the tools and abstractions needed for efficient development.

## Key Features
- Support for multiple FlexIO modules (FLEXIO1, FLEXIO2, FLEXIO3)
- Comprehensive serial communication interface via TeensyFlexSerial
- SPI communication capabilities through TeensyFlexSPI
- Flexible timer and shifter configuration options
- Pin selection and configuration utilities
- Built-in buffering for efficient data transmission and reception

## Architecture
- Core TeensyFlexIO class providing fundamental FlexIO functionality
- Specialized communication classes:
  - TeensyFlexSerial for serial protocol implementation
  - TeensyFlexSPI for SPI interface handling
- Buffered I/O support with configurable buffer sizes
- Event-driven callback system for efficient data handling
- Modular design allowing for easy extension and customization

The library includes several example applications demonstrating various use cases, from basic serial communication to MIDI output and SPI interfacing. These examples serve as practical starting points for your own projects and illustrate the library's capabilities in real-world scenarios. Whether you're a beginner learning about communication protocols or an experienced developer seeking an efficient FlexIO implementation, TeensyFlexIO provides the tools and abstraction needed for successful development on the Teensy 4/4.1 platform.