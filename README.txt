------------------------
Purpose of This Repo
------------------------
The project demonstrates about the effect of out-of-enclave bug.  
Such a bug can happens when the enclave trys to access unmaped address out of enclave.
If they want to write to a pointer with unintended value, this may leak secret. 
If they want to read, even more worse, the control flow integrity may be break. 

------------------------------------
How to Build/Execute the Sample Code
------------------------------------
1. Install Intel(R) SGX SDK for Linux* OS
2. Make sure your environment is set:
    $ source ${sgx-sdk-install-path}/environment
3. Build the project with the prepared Makefile:
    a. Hardware Mode, Debug build:
        $ make
    b. Hardware Mode, Pre-release build:
        $ make SGX_PRERELEASE=1 SGX_DEBUG=0
    c. Hardware Mode, Release build:
        $ make SGX_DEBUG=0
    d. Simulation Mode, Debug build:
        $ make SGX_MODE=SIM
    e. Simulation Mode, Pre-release build:
        $ make SGX_MODE=SIM SGX_PRERELEASE=1 SGX_DEBUG=0
    f. Simulation Mode, Release build:
        $ make SGX_MODE=SIM SGX_DEBUG=0
4. Before running, adjust mmap_min_addr to zero
    $ sysctl -w vm.mmap_min_addr="0"
5. Execute the binary directly:
    $ ./app
5. Remember to "make clean" before switching build mode

------------------------------------------
TODO
------------------------------------------
Dynamic map pages when a page fault out of enclave happens.
