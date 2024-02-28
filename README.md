simple code, using THREADS (defined in main.cpp) amount of threads

**optimizations:**  
    * mmap to allow simple parallelization  
    * string_view to have almost no (if any?) dynamic allocations  

samples can be generated using https://github.com/thebracket/one_billion_rows 's rust code

takes ~10 seconds on my Macbook M2 Pro
