# How to Run This Code (Even If You Have Never Used C++)

This guide will help you run the `money.cpp` simulation, even if you have **no prior experience with C++**. You have two options:

- **Option 1: Use Docker** (working on Windows, macOS, and Linux)  
- **Option 2: Compile and run the code directly** (on Windows, this requires installing the Windows Subsystem for Linux, or WSL)

Each option is explained step-by-step so you can get started quickly, regardless of your setup.

## 🧪 Option 1a: Running the money.cpp Simulation on Windows with Docker

This section will help you run the C++ simulation code on Windows using Docker, also ensuring the CSV output files are properly saved in your computer.

## ✅ Step 1: Install Docker Desktop

Download and install Docker Desktop from [https://www.docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop). Verify Docker is working by opening PowerShell and typing:
   ```
   docker --version
   ```

## ✅ Step 2: Set Up Your Project Files

1. Create a new folder on your computer (e.g., `C:\Users\USERNAME\money-simulation`)
2. Inside this folder, place these files:

### 📄 File 1: money.cpp
Save your simulation code as `money.cpp` in the project folder.

### 📄 File 2: Dockerfile
Create a file named exactly `Dockerfile` (no file extension) with this content:

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

# Create app and output directories inside the container
WORKDIR /app
RUN mkdir -p /app/output

# Copy your C++ file into the container
COPY money.cpp .

# Modify the C++ code to save to the /app/output directory
RUN sed -i 's|outputFilename = oss.str();|outputFilename = "output/" + oss.str();|' money.cpp

# Compile the C++ code
RUN g++ -std=c++17 -pthread money.cpp -o money -O3

# Set the default command to run your compiled program
CMD ["./money"]
```

### 📄 File 3: run-simulation.ps1
Create a file named `run-simulation.ps1` with this content:

```powershell
# PowerShell script to build and run Docker container with volume mounting
# Create a directory for output files if it doesn't exist
$outputDir = "$PWD\output"
if (-not (Test-Path -Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir
    Write-Host "Created directory: $outputDir"
}

# Build the Docker image
Write-Host "Building Docker image..."
docker build -t money-simulation .

# Run the container with volume mount
Write-Host "Running simulation in Docker container..."
Write-Host "CSV files will be saved to: $outputDir"
docker run --rm -v "${outputDir}:/app/output" money-simulation

Write-Host "Simulation complete."
Write-Host "Check $outputDir for your CSV files."
```

## ✅ Step 3: Run the Simulation

1. Open PowerShell
2. Navigate to your project folder using the `cd` command:
   ```
   cd C:\path\to\your\money-simulation
   ```
3. Run the PowerShell script:
   ```
   .\run-simulation.ps1
   ```
4. This script will:
   - Create an `output` folder (if it doesn't exist)
   - Build the Docker image
   - Run the simulation
   - Save all CSV files to the `output` folder

## ✅ Step 4: Find Your Output Files

When the simulation completes, all CSV files will be in the `output` folder inside your project directory.

## ⚠️ Troubleshooting CSV File Issues

If you don't see CSV files in your output folder:

1. **Check Docker permissions**:
   - Make sure Docker Desktop has permission to access your drives
   - Open Docker Desktop → Settings → Resources → File sharing → Ensure your drive is shared

2. **Run PowerShell as Administrator**:
   - Right-click on PowerShell → "Run as administrator"
   - Navigate to your project folder and run the script again as before

3. **Bypass PowerShell policy restrictions for this script**
```powershell
   powershell.exe -ExecutionPolicy Bypass -File .\run-simulation.ps1
```

4. **Try explicit paths**:
   - Modify the script to use full paths instead of relative paths:
   ```powershell
   $projectDir = "C:\path\to\your\money-simulation"
   $outputDir = "$projectDir\output"
   # ... rest of script unchanged ...
   docker run --rm -v "${outputDir}:/app/output" money-simulation
   ```

5. **Check output folder permissions**:
   - Right-click on the output folder → Properties → Security
   - Ensure your user account has "Write" permissions

## 🔁 Optional: Rebuild After Code Changes

If you edit `money.cpp`, you need to rebuild the Docker image:

```bash
docker build -t money-simulation .
```

Then run again with the volume mount to ensure your CSV files are saved:

```powershell
docker run --rm -v "${PWD}\output:/app/output" money-simulation
```

Or simply use the PowerShell script:

```powershell
.\run-simulation.ps1
```

## 🐧🍎 Option 1b: Running the `money.cpp` Simulation on Linux or macOS with Docker

This section guides you through running the same C++ simulation using Docker on **Linux or macOS**, and saving CSV output files properly in your host machine.

---

## ✅ Step 1: Install Docker

1. **Linux**: Follow the instructions for your distribution at https://docs.docker.com/engine/install/
2. **macOS**: Download Docker Desktop from https://www.docker.com/products/docker-desktop/
3. After installation, verify Docker is working:
   ```bash
   docker --version
   ```

---

## ✅ Step 2: Set Up Your Project Files

Create a project folder and include the necessary files:

```bash
mkdir -p ~/money-simulation && cd ~/money-simulation
```

### 📄 `money.cpp`
Put your simulation code here.

### 📄 `Dockerfile`
Same as in your Windows guide (no file extension):

```dockerfile
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
RUN mkdir -p /app/output

COPY money.cpp .

RUN sed -i 's|outputFilename = oss.str();|outputFilename = "output/" + oss.str();|' money.cpp

RUN g++ -std=c++17 -pthread money.cpp -o money -O3

CMD ["./money"]
```

---

### 📄 `run-simulation.sh`

Create a **shell script** instead of PowerShell:

```bash
#!/bin/bash

# Make sure script exits on error
set -e

# Get the absolute path to the output directory
OUTPUT_DIR="$(pwd)/output"

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"
echo "Created output directory: $OUTPUT_DIR"

# Build Docker image
echo "Building Docker image..."
docker build -t money-simulation .

# Run simulation with volume mount
echo "Running simulation in Docker container..."
docker run --rm -v "$OUTPUT_DIR:/app/output" money-simulation

echo "Simulation complete."
echo "Check $OUTPUT_DIR for your CSV files."
```

Then make it executable:

```bash
chmod +x run-simulation.sh
```

---

## ✅ Step 3: Run the Simulation

```bash
./run-simulation.sh
```

This will:
- Create an `output` folder
- Build the Docker image
- Run the simulation
- Store CSV files in `./output`

---

## ✅ Step 4: Find Your Output Files

They'll be in the `output` folder next to your code.

---

## ⚠️ Troubleshooting (Linux/macOS)

1. **Permissions**:
   - Ensure you have write access to the folder you're using
   - Avoid using Docker with `sudo` unless necessary. If needed, add your user to the Docker group:
     ```bash
     sudo usermod -aG docker $USER
     ```

2. **macOS file sharing**:
   - Docker Desktop for macOS must have access to your folders under:
     **Settings → Resources → File Sharing**

3. **Check container logs**:
   - Run interactively to debug:
     ```bash
     docker run -it --rm -v "$(pwd)/output:/app/output" money-simulation /bin/bash
     ```

---

## 🔁 Rebuilding After Code Changes

If you change `money.cpp`:

```bash
docker build -t money-simulation .
./run-simulation.sh
```

---

Here’s a more polished, user-friendly, and cross-platform version of your guide that clearly separates instructions for Windows, Linux, and macOS, improves readability, and makes assumptions transparent:

---

## 🧑‍💻 Option 2: Compile and Run the Simulation on Your Computer

If you'd rather run the simulation **without Docker**, you can compile and execute the C++ code directly on your machine. Choose the method that fits your operating system.

---

## 🔵 Windows (Two Options)

### ✅ Option A: Use Windows Subsystem for Linux (WSL) — **Recommended**

1. **Install WSL**  
   Follow Microsoft’s official instructions here:  
   👉 https://learn.microsoft.com/en-us/windows/wsl/install

2. **Open your Ubuntu terminal** (e.g., search “Ubuntu” in your Start menu, or presso WINDOWS+R and then type "ubuntu")

3. **Install the C++ compiler inside WSL:**

   ```bash
   sudo apt update
   sudo apt install g++
   ```

4. **Navigate to your project folder:**  
   Use the `/mnt/c/` path to access your Windows files from within WSL:

   ```bash
   cd /mnt/c/Users/YOUR_USERNAME/path/to/money.cpp
   ```

5. **Compile and run the code:**

   ```bash
   g++ -std=c++17 -pthread money.cpp -o money -O3
   ./money
   ```

   > 📌 **Note**: Every time you edit `money.cpp`, you'll need to recompile it before running again.

---

### ⚠️ Option B: Use MinGW (Advanced Users)

- Download and install MinGW from: https://www.mingw-w64.org/
- Open the MinGW terminal (`mingw32.exe` or similar)
- Use `cd` to navigate to your folder
- Compile and run using:

  ```bash
  g++ -std=c++17 -pthread money.cpp -o money -O3
  ./money.exe
  ```

> ⚠️ **Caution**: Threading support on Windows via MinGW can be tricky. Use WSL if possible.

---

## 🟢 Linux (Ubuntu/Debian)

1. **Open a terminal and install the compiler:**

   ```bash
   sudo apt update
   sudo apt install g++
   ```

2. **Navigate to your project folder and compile:**

   ```bash
   cd path/to/your/folder
   g++ -std=c++17 -pthread money.cpp -o money -O3
   ./money
   ```

---

## 🍎 macOS

1. **Install Xcode Command Line Tools:**

   Open Terminal and run:

   ```bash
   xcode-select --install
   ```

2. **Navigate to your project folder and compile:**

   ```bash
   cd /path/to/your/folder
   g++ -std=c++17 -pthread money.cpp -o money -O3
   ./money
   ```

---

## 📁 Output Files

After running `./money`, you'll find the resulting `.csv` files in the **same folder**. These contain the simulation results and can be analyzed using the R scripts located in the `/R` folder.

---

## 📝 Notes

- 🧩 No need to install NetLogo or R to run the C++ model.
- 🖼️ If you want to visualize the model or experiment interactively, use the NetLogo version in the `/NetLogo` folder.

---
