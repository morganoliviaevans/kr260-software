# kr260-software

## Kria KR260 Project

Just a fun little project to learn the in's and out's of embedded software land.

## Purpose
Application code and, later, a custom Linux image for the AMD Kria KR260 Robotics Starter Kit. The same LED pattern is implemented over progressively more involved interfaces (GPIO, then I2C, then SPI) and in three languages (C, C++, Python), to compare how each layer of the stack works.

Development starts on the stock Kria Ubuntu image. A Yocto-based custom image (kernel, device tree, root filesystem), built directly with Yocto/BitBake + meta-xilinx rather than PetaLinux, is a later milestone.

## Repo relationship

Consumes tagged artifacts from [kr260-fpga](https://github.com/morganoliviaevans/kr260-fpga):

- `.bin` bitstreams, loaded on the board with `fpgautil`
- `.xsa` hardware description, used for device tree generation

Never builds against a live branch of that repo.

## Toolchain

| Component       | Version                                      |
|-----------------|----------------------------------------------|
| Vivado / Vitis  | 2025.2 (matches kr260-fpga)                  |
| Board OS        | Kria Ubuntu (stock image)                    |
| Yocto / BitBake | TBD                                          |
| meta-xilinx     | TBD, pinned to the 2025.2 release branch     |
| Dependencies    | Nix (planned)                                |

## Board Layout

```
~/kria/
  kr260-software/   # this repo; edit on host, push, git pull on the board
  bitstreams/       # .bin files copied from kr260-fpga releases
```

## Hardware

| Part     | Interface | Used in    | Role                                        |
|----------|-----------|------------|---------------------------------------------|
| 8x LED + 220-330 Ω resistors | PMOD GPIO | Tasks 3-4 | Pattern output, driven directly by PL pins |
| MCP23017 | I2C       | Tasks 5-6  | 16-bit GPIO expander driving the LEDs       |
| BME280   | SPI       | Task 7     | Temperature/humidity/pressure sensor        |

## Project Tasks

| # | Task | FPGA image | Status |
|---|------|------------|--------|
| 1 | Define the LED behavior (e.g. spiral, bounce, binary counter) | n/a | Done |
| 2 | Install Vivado and Vitis 2025.2 on the Ubuntu build machine (FPGA side) | n/a | Done |
| 3 | Load the initial FPGA image; verify breadboard wiring with the PMOD1-4 LED chaser test | kr260-fpga PR #1 (hardware-only chaser) | In progress |
| 4 | Load a new FPGA image exposing PS GPIO over EMIO on the PMOD pins; implement the pattern from software. C, C++, Python | EMIO GPIO → PMOD | Not started |
| 5 | Load an image with the AMD AXI I2C IP pinned out to a PMOD; implement the pattern on LEDs driven by the MCP23017. C, C++, Python | AXI I2C → PMOD | Not started |
| 6 | Load an image with a custom I2C controller in the PL, exposed to software as memory-mapped registers; implement the pattern via the MCP23017. C, C++, Python | Custom I2C | Not started |
| 7 | Load an image with a custom SPI controller in the PL; read the BME280 and display its readings as an LED pattern. C, C++, Python | Custom SPI | Not started |

### Notes

- PMOD headers connect to PL pins, so software reaches them through EMIO (or AXI GPIO). MIO pins are hardwired to PS peripherals and cannot drive the PMODs.
- MCP23017: tie `RESET` high and `A2`-`A0` to fixed levels (all low gives I2C address `0x20`). SDA/SCL need external pull-up resistors to 3.3 V. Pins power up as inputs (`IODIR` = `0xFF`), so write `0x00` to `IODIRA` before driving LEDs.
- BME280 is a 3.3 V part; will need to use a breakout that exposes the SPI pins (`CSB`, `SDO`).
- The stock-image bitstream does not persist across reboots until [kr260-fpga](https://github.com/morganoliviaevans/kr260-fpga) ships a device tree overlay and `shell.json` for `xmutil loadapp`.

### Stretch goals

- Custom Yocto image
- RTOS (e.g. FreeRTOS) on the Cortex-R5F real-time cores
- CI gating: linting, builds, and tests on every PR
- Hardware-software unit testing

## Status

- Initial FPGA image received. Board boots stock Ubuntu, reachable over SSH and serial console.
- Current Task: 3