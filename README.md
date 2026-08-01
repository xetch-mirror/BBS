# BBS OS
### The Bold Operating System

![C](https://img.shields.io/badge/Language-C-blue?logo=c&logoColor=white)
![License](https://img.shields.io/badge/License-XOPL--1-blue)
![Stars](https://img.shields.io/github/stars/poopyking482-sudo/BBS?style=flat&color=blue)

## What is BBS?

BBS is an RTOS-based operating system with basic drivers and hardware support built in. It ships with familiar Unix-like commands, including `rm`, `grep`, `cat`, and `mkdir`.

## Why BBS?

- **BBUR Repository** — the Bold Based User Repository gives BBS a consistent, straightforward way to manage packages.
- **Small and portable** — the kernel is compact and embeddable, and BBS is portable enough to boot on very old hardware, down to a Pentium-era machine.
- **A working foundation** — BBS already has a functioning init system in place, with active development ongoing.

## Tools

BBS's `binutils` includes:

- **mininano** — a lightweight, embeddable text editor based on nano. Currently has known bugs (occasional page errors) but is usable for basic file editing.
- **MP** — a lightweight, embeddable interpreter built in C.

BBS's core also relies on **Xlibary**, a custom C library written specifically for the project.

## Building

BBS is built using Xlibary
Expect Bmake to be created soon
## Tips

`mininano` is currently unstable and prone to page errors. Until that's fixed, we recommend editing files on your host OS (Windows, macOS, or Linux) rather than inside BBS.
