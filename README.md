# Memory Requirements:

- 1 Compute set
- 1 Vertex

**# Total Tensors**
- 2 Variable Tensors for IO

**StrideN**
- 1 Varaible Tensor, 1 Constant Tensor

*- to store N*

**Rand**
- 2 Variable Tensors, 1 Constant Tensor

*- to store the random indices and seed*

# Mapping Plan:

## Tensor Flow Diagram

![Tensor Flow Diagram](figures/Tensor%20Flow%20Diagram.png)

# Makefile actions:

**make prun:**
- git pull
- compiles code
- removes outputs from previous runs
- runs batch script
- watches code running in queue

**make show:**
- cats the input and output files
- cats the logs

# Operations list:
- Stride
- StrideN
- Rand