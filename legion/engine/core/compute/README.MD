# This Core Module provides OpenCL functionality to the engine
Root Path legion/engine/core/compute/


- [x] It loads an OpenCL context of Version >2.0 `context.hpp`
- [x] It can create Buffers on the Computing device `buffer.hpp`
- [x] It can create Compute-Programs `program.hpp`
- [x] It can create Kernels and CommandQueues on the Computing device `kernel.hpp`

- [x] It abstracts the creation of Programs, Buffers, CommandQueues and Kernels on a high level `high_level/function.hpp`

Basic Usage Example:

```c
  //  assets/kernels/vadd_kernel.cl
  __kernel void vector_add(__global const int* A, __global const int* B,__global int* C) {
    int i = get_global_id(0);
  
    C[i] = A[i] + B[i];
  }
```

```cpp
  
  std::vector<int> A = ...;
  std::vector<int> B = ...;
  std::vector<int> Results(1024);
  
  using compute::out;
  
  auto vector_add = fs::view("assets://kernels/vadd_kernel.cl")
                      .load_as<compute::function>("vector_add");
                      
  vector_add(1024,A,B,out(Results));
  

```

