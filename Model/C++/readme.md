# How to Run This Code (Even If You Have Never Used C++)

This guide will help you run the `money.cpp` simulation code **even if you've never used C++ before**.

# 🧪 Option 1: Running the money.cpp Simulation on Windows with Docker

This guide will help you run the C++ simulation code on Windows using Docker, with a special focus on ensuring the CSV output files are properly saved to your computer.

## ✅ Step 1: Install Docker Desktop

1. Download Docker Desktop from [https://www.docker.com/products/docker-desktop](https://www.docker.com/products/docker-desktop)
2. Install Docker Desktop by following the installation wizard
3. Start Docker Desktop and wait for it to fully load (check for the Docker icon in your system tray)
4. Verify Docker is working by opening PowerShell and typing:
   ```
   docker --version
   ```

## ✅ Step 2: Set Up Your Project Files

1. Create a new folder on your computer (e.g., `C:\money-simulation`)
2. Inside this folder, create these files:

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
   - Navigate to your project folder and run the script again

3. **Try explicit paths**:
   - Modify the script to use full paths instead of relative paths:
   ```powershell
   $projectDir = "C:\path\to\your\money-simulation"
   $outputDir = "$projectDir\output"
   # ... rest of script ...
   docker run --rm -v "${outputDir}:/app/output" money-simulation
   ```

4. **Check output folder permissions**:
   - Right-click on the output folder → Properties → Security
   - Ensure your user account has "Write" permissions

5. **Verify the simulation ran correctly**:
   - The simulation should show progress and completion messages in the terminal
   - If the simulation fails or terminates early, check for error messages

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
# 🧑‍💻 Option 2: Compile and Run on Your Own Computer

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
