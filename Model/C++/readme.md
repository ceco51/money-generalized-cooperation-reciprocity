# How to Run This Code (Even If You Have Never Used C++)

This guide will help you run the `money.cpp` simulation code **even if you've never used C++ before**.

## Quick Start Guide

## 🧪 Option 1: Run the `money.cpp` Simulation with Docker (No C++ Setup Needed)

This guide lets you compile and run the C++ code without installing anything besides Docker.

#### ✅ Step 1: Install Docker

If you don’t already have Docker:

- **Windows/macOS**: Download and install Docker Desktop from [https://www.docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop)
- **Linux**: Install Docker via your package manager (e.g., `sudo apt install docker.io` on Ubuntu), then enable and start the service.

Once installed, open a terminal and check it's working:

```bash
docker --version
```

---

#### ✅ Step 2: Prepare Your Files

1. Create a new empty folder (e.g., `money-simulation`)
2. Inside that folder, save the following two files:

---

#### 📄 `money.cpp`  
Download the C++ simulation file into this folder. You can:

- Use the GitHub website: Click on `money.cpp` → **Raw** → Right click → **Save As...**
- Or copy-paste your C++ code into a new text file named `money.cpp`.

---

#### 📄 `Dockerfile`  
Create a new file in the same folder called `Dockerfile` (no extension) and paste the following:

```dockerfile
# Use a minimal Ubuntu image
FROM ubuntu:22.04

# Set environment variable to noninteractive to suppress prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Create an app directory inside the container
WORKDIR /app

# Copy your C++ file into the container
COPY money.cpp .

# Compile the C++ code
RUN g++ -std=c++17 -pthread money.cpp -o money -O3

# Set the default command to run your compiled program
CMD ["./money"]
```

---

### ✅ Step 3: Build the Docker Image

In a terminal, navigate to the folder where you saved `money.cpp` and `Dockerfile`. Then run:

```bash
docker build -t money-simulation .
```

This will download the necessary Linux environment, install C++ tools, compile the code, and package it all into a runnable image called `money-simulation`.

---

### ✅ Step 4: Run the Simulation

After building, you can run your simulation with:

```bash
docker run --rm money-simulation
```

---

### 🔁 Optional: Rebuild After Code Changes

If you edit `money.cpp`, you need to rebuild the Docker image:

```bash
docker build -t money-simulation .
```

Then re-run as before.

---

## 🧑‍💻 Option 2: Compile and Run on Your Own Computer

If you're comfortable installing things, follow the guide for your system.

---

### 🔵 Windows (2 ways)

#### Option A: Windows Subsystem for Linux (WSL) – Recommended

1. **Install WSL**:  
   Follow this Microsoft guide: https://learn.microsoft.com/en-us/windows/wsl/install

2. **Install required tools inside WSL terminal:**

Open a terminal (e.g., Windows + R, then type "Ubuntu")
```bash
sudo apt update
sudo apt install g++
```

3. **Navigate to the folder where `money.cpp` is saved:**

```bash
cd /mnt/c/Users/YOUR_PATH_TO_MONEY.CPP
```

4. **Compile and run:**

```bash
g++ -std=c++17 -pthread money.cpp -o money -O3
./money
```
Every time you modify the C++ file (e.g., by changing the parameter combinations to explore) you need to recompile and then re-run. 

#### Option B: MinGW (for advanced users)

- Install MinGW from https://www.mingw-w64.org/
- Open the MinGW terminal, navigate to your folder, then compile and run as above.

---

### 🟢 Linux (Ubuntu/Debian)

1. **Install the compiler:**

```bash
sudo apt update
sudo apt install g++
```

2. **Navigate to the code folder and compile:**

```bash
cd path/to/your/folder
g++ -std=c++17 -pthread money.cpp -o money -O3
./money
```

---

## 📁 What You Get After Running

After the program runs, you’ll see `.csv` files appear in the folder in which './money' is executed. These contain the results of the simulations. You can analyze them using the R scripts in the `/R` folder (see instructions in the README for R).

---

## 📎 Notes

- No installation of NetLogo or R is needed to run the C++ model itself.
- If you want to visualize the model or experiment interactively, see the NetLogo version in the `/NetLogo` folder.

---
