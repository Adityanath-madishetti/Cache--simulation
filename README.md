# RISC-V Simulator Project

## Authors: Adityanath Madishetti , Kasturi Sai Rishi   
*Roll Number*: CS23BTECH11032 , CS23BTECH11024

---

## Project Overview

This project is a RISC-V *Cache-simulator* that interprets RISC-V assembly code and provides an interactive terminal for running the simulation with minimalistic debugging commands. This Simulator also supports cache-simulation (which includes various policies for write and replacement )

It gives statistics of cache simulation and cpmmands to get some deeper look into cache at any point in program

---

## Few Interactive Terminal Features(over the yo of Original simulator from LAB-4)

The simulator provides an interactive terminal and  cache memory with the following commands:

- **Cache_sim enable config_file*: This command enables **cache simulation* based on configuration file provided
- *`cache_sim disable `*: this disables cache simulation by invalidating cache entries 

- **cache_sim dump filename**:  This command dumps all valid entries into the given file ,this command helps to know cache values at any point in simulation

- **cache_sim stats**: Displays the HIT , MISS , HIT RATE of the Dcache

- **cache_sim status**: Displays the cache properties as given in config file.

- **cache_sim invalidate**: this invalidates the entries in cache  


- this also gives a did you mean suggestion 
for any wrong commands 
  
These extra command helps to understand the cache simulation based on config file. 

---

## Provisions
This simulator provides

1) cache with associativity of powers of 2 
2) Write policies ( Write back with write allocate  and
write through with not allocate )
3) replacement policy : FIFO , LRU, RANDOM 

using dump command and stats command we can know complete information of cache at any time



## Cache implementation Over-View

Cache is made up of Association and line is made up of
lines so 

- **cache_line** : this class has attributes such as dirty bit valid bit and data buts and tag bits

- *`cache_association: `* This class is vector of lines , the size of vector is nothing but in n-way cache

- **cache_table** : This is the main class which is built by vector of associations , it contains all the information about the cache aong with its properties
---
Instantiating Cache_table by giving it appropriate parameters  create cache for our program

USAGE:

use cache_sim enable config_file command to enable the cache simulation 

use cache_sim disable  to disable cache_simulations


---
1. **main.cpp**:  
   The main driver of the simulator, responsible for initializing the simulation and handling the interactive terminal.
   
2. **memoryclass.cpp**:  
   This file contains the implementation of the memory management system for the simulator.
   
3. **conversionspace.cpp**:  
   Handles various data format conversions needed during the simulation.
   
4. **Helper.cpp**:  
   Includes helper functions that assist in various simulation tasks, like managing breakpoints and handling input/output.
   
5. **encoder_functions.cpp**:  
   Contains the functions that encode the instructions and manage the execution pipeline.

6. **extras.cpp**:  
   Contains extra utilities and miscellaneous functions used throughout the project.

7. **resources.hh**:  
   The header file that defines shared resources like class declarations and constants.

8. **project_RISC_V**:  
   A directory containing the necessary files for the RISCV ASSEMBLER

---

## Makefile

A Makefile is provided to CARRY OUT the build process in an EASIER WAY. It includes targets to compile individual components and link them together, as well as clean-up targets.

### Makefile Targets:

- **all**:  
   Compiles and links all the source files into an executable named riscv_sim.
   
   bash
   make all
   
   
   running the executables setups the interactive terminal 

   this takes commands and based in them simulates the codde using allocated memory and register
