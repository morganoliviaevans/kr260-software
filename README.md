# kr260-software

## Kria KR260 Project

Just a fun little project to learn the in's and out's of embedded software land.

## Purpose
Application code and, later, a custom Linux image for the AMD Kria KR260 Robotics Starter Kit. The same LED pattern is implemented over progressively more involved interfaces (GPIO, then I2C, then SPI) and in three languages (C, C++, Python), to compare how each layer of the stack works.

Development starts on the stock Kria Ubuntu image. A Yocto-based custom image (kernel, device tree, root filesystem), built directly with Yocto/BitBake + meta-xilinx rather than PetaLinux, is a stretch milestone.

## Repo relationship

Consumes tagged artifacts from [kr260-fpga](https://github.com/morganoliviaevans/kr260-fpga):

- `.bin` bitstreams, loaded on the board with `fpgautil`
- `.xsa` hardware description, used for device tree generation

## Loading the FPGA Image

- **From Linux on the PS**:
1. Copy from [kr260-fpga](https://github.com/morganoliviaevans/kr260-fpga): `out/bitstream/kr260.bin`
2. Unload starter-kit image: `sudo xmutil unloadapp`
3. Load new image: `sudo fpgautil -b kr260.bin`
4. **Optional**: Create shell functions for easy loading and unloading, save in `~/.bashrc`:

```
fpga-load() { sudo xmutil unloadapp && sudo fpgautil -b "$1"; } # Usage: fpga-load ~/kria/kr260-software/fpga/out/bitstream/kr260.bin

# or hardcoded if the path isn't going to change:
fpga-load() { sudo xmutil unloadapp && sudo fpgautil -b ~/kria/kr260-software/fpga/out/bitstream/kr260.bin; } # Usage: fpga-load

fpga-default() { sudo xmutil unloadapp && sudo xmutil loadapp k26-starter-kits; }
```

## Toolchain

| Component       | Version                      |
|-----------------|------------------------------|
| Vivado / Vitis  | 2025.2                       |
| Board OS        | Kria Ubuntu                  |
| Yocto / BitBake | TBD                          |
| meta-xilinx     | TBD                          |
| Dependencies    | Nix (TBD)                    |

## Board Layout

```
~/kria/
  kr260-software/   # this repo; edit on host, push, git pull on the board
  fpga/             # .bin files copied from kr260-fpga releases
```

## Hardware

| Part     | Interface | Used in    | Role                                        |
|----------|-----------|------------|---------------------------------------------|
| 8x LED + 330 Ω resistors | PMOD GPIO | Tasks 3-8 | Pattern output, driven directly by PL pins |
| MCP23017 | I2C       | Tasks 5-8  | 16-bit GPIO expander driving the LEDs       |
| BME280   | SPI       | Tasks 6 & 8     | Temperature, humidity, pressure sensor        |

## Project Tasks

| # | Task | Languages  | FPGA Image | Status |
|---|------|----------|------------|--------|
| 1 | Define LED behavior | n/a | n/a | Done |
| 2 | Install Vivado and Vitis 2025.2 on the Ubuntu build machine | n/a | n/a | Done |
| 3 | Load initial FPGA image; verify breadboard wiring with the PMOD1-4 LED chaser test | n/a | kr260-fpga PR #1 | Done |
| 4 | Load new image exposing PS GPIO over EMIO on PMOD2 pins; implement the pattern from software | C, C++, Python | EMIO GPIO → PMOD2 | **In Progress** |
| 5 | AMD AXI I2C IP pinned out to PMOD3; implement the pattern on LEDs driven by the MCP23017 | C, C++, Python | AXI I2C → PMOD | Not started |
| 6 | AMD AXI Quad SPI pinned out to PMOD4; read the BME280 and display its readings as an LED pattern | C, C++, Python | AXI SPI → PMOD | Not started |
| 7 | Load new image with custom I2C; exposed to software as memory-mapped registers; implement the pattern via the MCP23017 | C, C++, Python | Custom I2C | Not started |
| 8 | Load new image with custom SPI; read the BME280 and display its readings as an LED pattern | C, C++, Python | Custom SPI | Not started |