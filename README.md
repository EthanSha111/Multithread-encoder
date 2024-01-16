## Multithreaded RLE Encoder

### Introduction
This Multithreaded Run-Length Encoding (RLE) Encoder is a high-performance C application designed to efficiently compress data using the RLE algorithm. It showcases the perfect logic of a thread pool alongside robust synchronization mechanisms to guarantee thread safety and maximize throughput.

### Key Features
- **Thread Pool**: Utilizes a pool of worker threads to parallelize encoding tasks, enhancing performance on multi-core processors.
- **Mutex Locks**: Employs mutexes to ensure that shared resources are accessed in a thread-safe manner, preventing race conditions and data corruption.
- **Condition Variables**: Uses condition variables to efficiently signal threads about task availability and completion, reducing CPU idle time.
- **Dynamic Memory Management**: Implements dynamic allocation and reallocation of memory to hold encoded data, ensuring efficient use of resources.
Signal Handling: Properly handles termination signals for graceful shutdown and resource cleanup.
Error Handling: Robust error checking and handling to prevent unexpected crashes and memory leaks.
How It Works
The program accepts multiple input files and compresses each file's contents using RLE. The results are then written to standard output or a specified output file. It takes full advantage of modern multi-core CPUs by distributing the encoding workload across multiple threads.

### Usage
```php
Usage: rle_encoder [-j <num_workers>] <file1> [file2 ...] [output_file]
-j <num_workers>: Optional flag to specify the number of worker threads in the thread pool.
<file1> [file2 ...]: One or more files to encode.
[output_file]: Optional output file to write the encoded data (defaults to standard output).
```

### Building the Project
To compile the project, you will need a C compiler (e.g., gcc or clang) and make sure that the pthread library is available on your system.

```bash
gcc -o rle_encoder *.c -lpthread
```

### Running the Tests
To ensure the encoder works as expected, a suite of unit tests is provided. Run the tests using:

```bash
./run_tests.sh
```

### Profile Standards
This project is maintained to a high standard as it serves as a showcase on my professional profile. It is designed to be:

- \**Efficient**: Ensures the quickest compression times possible.
- \**Reliable**: Produces consistent outputs every time.
- \**Scalable**: Handles small to very large files without degradation in performance.
- \**Readable**: Code is well-commented and follows clean code principles for maintainability.

---
