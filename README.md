# AES Text Encryption System: Android to Linux Kernel

An embedded system on the Achro-i.MX6 board in which an Android app encrypts short text with AES-128 through a custom Linux system call and stores it under a key. The app reads input and shows results on the board's FPGA I/O devices through a character device driver. The project runs through every layer, from the app to the native library, the kernel, and the hardware registers.

> Individual final project, Embedded System Software (CSE4116), Sogang University, June 2024.
>
> [Presentation](docs/presentation.pdf) · [Report](docs/final_report.pdf) (Korean)

## What it does

| Operation | Input | Result |
| --- | --- | --- |
| Insert | Text typed on the push switches | Encrypts the text, derives a numeric key from the ciphertext, stores the ciphertext under that key, and shows the key on the 7-segment display (FND) |
| Search | Key typed on the push switches | Finds the ciphertext, decrypts it, and shows the plaintext on the text LCD |
| Delete | Key typed on the push switches | Removes the entry |

After an insert or a search, the result stays on the display for three seconds while the dot matrix counts down, and then the devices are cleared.

## How the layers fit together

| Layer | Code | Role |
| --- | --- | --- |
| Android app (Java) | [`AESsystem/src`](AESsystem/src/com/example/aessystem) | `MainActivity` with insert, search, and delete buttons. Each operation runs in its own bound service. |
| Native library (C, JNI) | [`AESsystem/jni`](AESsystem/jni) | Connects Java to the kernel. Calls the AES system call, keeps a hash table of ciphertexts, and drives the FPGA devices through `ioctl`. A POSIX semaphore guards the shared native state. |
| System call (kernel) | [`aes/`](aes) | AES-128 encryption and decryption written from scratch in C and exposed as system call 379 (`sys_aes`). |
| Device driver (kernel module) | [`module/`](module) | Character device `/dev/dev_driver` (major 242) that maps the FND, dot matrix, text LCD, and push switches into memory with `ioremap` and exposes them through nine `ioctl` commands. |

**Insert path.** The insert service reads the text from the switches through the native library and hands it back for encryption. The library encrypts it through system call 379, derives a key from the ciphertext, shows the key on the FND, blocks for a three-second countdown, clears the devices, and stores the ciphertext in a 97-bucket hash table. Search looks up the bucket for the key, decrypts the matching ciphertext through the same system call, and shows the plaintext on the LCD.

**Blocking input from the switches.** A read request allocates a buffer, starts a kernel thread that collects switch presses into it, and puts the calling process on a wait queue. When the user presses the board's HOME button, the interrupt handler stops the thread and wakes the process, which then takes the finished text.

**Blocking countdown.** A countdown request sets a kernel timer and puts the caller on a wait queue. Each timer tick updates the dot matrix and decrements the count, and the caller wakes up when it reaches zero.

### Driver `ioctl` commands

| Command | Direction | Purpose |
| --- | --- | --- |
| `GET_TEXT`, `GET_NUMBER` | read | Collect text or a number from the push switches (blocking until HOME is pressed) |
| `SET_FND`, `SET_DOT`, `SET_LCD_FIRST`, `SET_LCD_SECOND` | write | Show a value on the FND, dot matrix, or one LCD line |
| `COUNT_WITH_BLOCK`, `COUNT_NONBLOCK` | write | Countdown on the dot matrix, with or without blocking the caller |
| `CLEAR_DEVICE` | — | Clear every output device |

## Building

This was built for the course's Achro-i.MX6 environment and is kept here as a record rather than a ready-to-run project.

- **Kernel module:** `cd module && make` cross-compiles against the board's kernel source (`KDIR` in the [Makefile](module/Makefile)).
- **System call:** [`aes/aes.c`](aes/aes.c) is compiled into the kernel and registered as call 379. The kernel's syscall-table patch is not included. [`aes/test.c`](aes/test.c) checks a round trip from user space.
- **Android app:** an Eclipse ADT project. Build the native library with `ndk-build` (see [`jni/Android.mk`](AESsystem/jni/Android.mk)). [`gen_header.sh`](AESsystem/gen_header.sh) regenerates the JNI headers with `javah`.

## Limitations

- Each entry holds one AES block, so text is limited to 16 bytes.
- The AES key is a fixed constant compiled into the kernel (the FIPS-197 example key).
- The key derived from the ciphertext is reduced to the range 0–9996, so two different texts can get the same key. A search then returns the most recently inserted one.

## Layout

```text
AESsystem/   Android app: Java sources, layouts, and the JNI native library (jni/)
aes/         AES-128 system call and a user-space test
module/      FPGA I/O character device driver and a user-space test
docs/        presentation and final report (Korean)
```
