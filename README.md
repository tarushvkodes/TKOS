# TKOS (Custom Operating System)

TKOS is a custom operating system built from scratch as an educational project to understand low-level system programming and OS development concepts.

## Project Overview

This project implements a basic operating system with the following core components:
- Custom bootloader
- Kernel with basic system services
- Memory management system
- Interrupt handling
- Device drivers
- File system
- Command-line interface

## Prerequisites

To build and run TKOS, you'll need:
- NASM (Netwide Assembler)
- GCC/Clang cross-compiler
- QEMU or Bochs emulator
- Make (build automation)

## Project Structure

```
/TKOS
  /bootloader   - Boot sector and early initialization code
  /kernel       - Core kernel implementation
  /drivers      - Hardware interface drivers
  /libs         - Supporting libraries
  /docs         - Documentation and specs
```

## Building TKOS

1. Install the required prerequisites
2. Clone this repository
3. Run the build process (detailed build instructions coming soon)

## Running TKOS

After building, you can run TKOS using QEMU:
```bash
qemu-system-x86_64 -fda bootloader.img
```

## Development Status

TKOS is under active development. Current features:
- [x] Basic bootloader implementation
- [x] Protected mode transition
- [ ] Kernel initialization
- [ ] Memory management
- [ ] Process scheduling
- [ ] File system
- [ ] Shell interface

## Contributing

This is a personal educational project, but feedback and suggestions are welcome through issues and discussions.

## Resources

For more detailed information about the project:
- See `docs/` directory for detailed documentation
- Check `context.md` for the full project roadmap and development plan

## License

[License information to be added]

## Acknowledgments

- [OSDev Wiki](https://wiki.osdev.org)
- [Phil Opp's Blog OS Tutorial](https://os.phil-opp.com/)
- Other resources listed in the project documentation