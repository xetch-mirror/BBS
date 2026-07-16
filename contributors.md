# Welcome to the Xetch Team!

Thank you for contributing! Below is the official environment setup and development guidelines for our 32-bit operating system.

---

## Environment Setup

Before contributing, you must install the following tools on your 32-bit development machine:

* **Compilers & Assemblers:** 
  *`gcc` (32-bit target toolchain, e.g., `i386-elf-gcc` or `i686-elf-gcc`)
  * `nasm` (configured for ELF32 output)
  * `linker` (needed to link kernel and files)
  * `make` (needed to build iso, especially for testing)
* **Utilities:** `gzip` and system build headers
* **Emulators:** `qemu-system-i386`, `bochs`, or any preferred x86 emulator to test the OS image

---

## Development Guidelines

We are primarily a C-based team learning and integrating Rust into our 32-bit kernel infrastructure. We welcome all contributions that help stabilize and expand the OS.

### Rules & Constraints

* **Allowed Subsets:** You may use `core::` libraries and standard bare-metal Rust primitives.
* **Error Handling:** While production kernels minimize crashes, you are permitted to use `panic()` for unrecoverable errors during this phase.
* **Execution Constraint:** Any written code must be compatible with **32-bit freestanding, bare-metal environments** (strictly no standard `std` library allocations, using 32-bit target specs like `i686-unknown-none`).

---

> **Note:** Thank you for your support in building **BBS/OS**! Let's build something awesome.
