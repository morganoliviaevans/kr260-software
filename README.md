# kr260-software

## Kria KR260 Project

Just a fun little project to learn the in's and out's of embedded software land.

## Purpose
This project will have a Yocto-based custom Linux image (kernel, device tree, root filesystem) and application code for the AMD Kria KR260, built directly against Yocto/BitBake rather than PetaLinux, for deeper build-system understanding.

## Toolchain
- Yocto/BitBake: [version, TBD]
- meta-xilinx branch: [pinned to match Vivado version once confirmed]

## Status
Nothing built yet. I am baby.

## Repo relationship
Consumes a tagged XSA artifact from [kr260-fpga](link) as input to device tree
generation. Does not build against a live branch of that repo.