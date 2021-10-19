# Choose the Compilers (mpicc only required for compiling the MPI library)
cc         = clang++
mpicc      = mpicxx

# Set optimization flags
ccopt      = -O3 -Ofast -mavx

# Set the directory of the lila headers
liladir    = /mnt/home/awietek/Research/Software/lila
includes   = -I$(liladir)

# Choose the BLas/Lapack Backend

## Intel MKL
lilabackend = -DLILA_USE_MKL
libraries   = -lmkl_rt

# ## Accelerate on OSX
# lilabackend = -DLILA_USE_ACCELERATE
# libraries   = -framework Accelerate

# Additional parameters for compilation
ccarch     = -std=c++17 -Wall -pedantic -m64 -Wno-return-type-c-linkage
