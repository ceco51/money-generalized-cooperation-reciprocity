# How to Run This Code (Even If You Have Never Used C++)

This guide will help you run the `money.cpp` simulation code **even if you've never used C++ before**.

## Quick Start Guide

### ✅ Option 1: Use **Docker** (No C++ Setup Required)

Docker creates a "virtual container" with everything already installed. 

#### Step 1: Install Docker  
- Windows: https://www.docker.com/products/docker-desktop  
- Mac: https://www.docker.com/products/docker-desktop  
- Linux: https://docs.docker.com/engine/install/

#### Step 2: Download the Code  
Open a terminal (or Command Prompt in Windows) and type:

```bash
git clone https://github.com/YOUR_USERNAME/YOUR_REPO.git
cd YOUR_REPO
```

Or just [download ZIP](https://github.com/YOUR_USERNAME/YOUR_REPO/archive/refs/heads/main.zip) from the repo and unzip it.

#### Step 3: Build the Docker Image

```bash
docker build -t money-sim .
```

#### Step 4: Run the Simulation

```bash
docker run --rm -v $(pwd):/sim money-sim ./money
```

The output `.csv` files will appear in the same folder on your computer.

### 🧑‍💻 Option 2: Compile and Run on Your Own Computer

If you're comfortable installing things, follow the guide for your system.

---

### 🔵 Windows (2 ways)

#### Option A: Windows Subsystem for Linux (WSL) – Recommended

1. **Install WSL**:  
   Follow this Microsoft guide: https://learn.microsoft.com/en-us/windows/wsl/install

2. **Install required tools inside WSL terminal:**

```bash
sudo apt update
sudo apt install g++
```

3. **Navigate to the folder where `money.cpp` is saved:**

```bash
cd /mnt/c/Users/YOUR_USERNAME/YOUR_PATH
```

4. **Compile and run:**

```bash
g++ -std=c++17 -pthread money.cpp -o money -O3
./money
```

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
- If you want to visualize the model or experiment interactively, see the NetLogo version in the `/netlogo` folder.

---
