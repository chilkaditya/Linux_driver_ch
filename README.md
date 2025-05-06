# 🌀Dynamic Circular Queue Character Device

A Linux kernel module that implements a dynamic circular queue accessed via a character device using IOCTL calls. This module supports dynamic resizing, and blocking read operations — perfect for inter-process communication (IPC)-like behavior.

Demo video - https://drive.google.com/file/d/15f8KXWZnLvYJ2LfgWiKxB-4E2KYvk6a_/view?usp=sharing

---

## 📖 About

This is a custom Linux character device (`/dev/chilka`) that maintains a byte-level circular queue in kernel space. Userspace programs can interact with it via IOCTL calls to dynamically configure, push data, and read data — with blocking behavior when no data is available to read.

This can be especially useful in scenarios like:
- Kernel-to-user data streaming
- Asynchronous IPC mechanisms
- Queue-based kernel buffering

---

## ✨ Features

- 🔧 **Dynamic queue sizing** via `SET_SIZE_OF_QUEUE`
- 📥 **Push arbitrary data** using `PUSH_DATA`
- 📤 **Blocking pop** using `POP_DATA` (waits if queue is empty)
- 🔁 Fully circular behavior — wraps around when end is reached
- 🧵 Wait queue-based blocking reads
- 🐞 Kernel logs for every push and pop (data and length)

---

## 🛠 Requirements
- Linux system with kernel headers
- GCC compiler
- Root privileges to load/unload kernel modules

## 🚀 Set Up and Run
- Cloning
```bash
git clone https://github.com/chilkaditya/Vichark-task.git
cd Vicharak-task
```

### 1. 🔨 Build the Kernel Module

```bash
make
```
(Note: make sure user space DRIVER_NAME and kernel space DEVICE_NAME is same.Ex - If you put ```c #define DRIVER_NAME "dev/your-name"``` in user space code then you have put ```c #define DEVICE_NAME "your-name"```  

### 1. 🔨 Insert the Module in kernel

```bash
sudo insmod "your-kernel-object-file"
```
### 2. Test the SET_SIZE_OF_QUEUE
```bash
gcc configurator.c -o conf
sudo ./conf
dmesg | tail 
```
### 3. Test the PUSH_DATA
```bash
gcc filler.c -o filler
sudo ./filler
dmesg | tail 
```
### 4. Test the POP_DATA
```bash
gcc reader.c -o reader
sudo ./reader
dmesg | tail 
```

