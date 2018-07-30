/*
 * Copyright (C) 2011-2018 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* file: Enclave.c */
#include <string.h>
#include <sgx_trts.h>
#include "Enclave_t.h"  /* Import OCalls, e.g., `ocall_print` */

/* A struct that stores and processes secrets */
struct secret_struct {
    void (*func)(const char* str);
    char data[64];
};

/* A pointer that should have been initialized somewhere but is not */
struct secret_struct* secret; /* = NULL, if not initialized */

/* A privileged attacker could possibly memory-map at page 0 and thus
 * effectively controls the `secret_struct` pointed by `secret`. On Linux,
 * memory-mapping at low memory addresses is usually forbidden. But this
 * restriction could be eliminated by using command `sudo sysctl -w
 * vm.mmap_min_addr="0"`. */

/* An ECall that consists of both **out-of-enclave** writes and reads */
void ecall_process_secret_sloppily(void) {
    /* Attack 1: a secret is leaked by writing to an out-of-enclave struct */
    char secret_bytes[64];
    sgx_read_rand((unsigned char*)secret_bytes, sizeof(secret_bytes));
    memcpy(&secret->data, secret_bytes, sizeof(secret_bytes));

    /* Attack 2: an arbitrary code chosen by attackers is executed by reading
     * from an out-of-enclave struct. The attacker could set the func pointer
     * to a dangerous OCall like `ocall_print`, thus leaking secret. */
    const char* secret_str = "Top Secret!!!\n";
    secret->func(secret_str);
}


uint64_t enclave_get_address_of_ocall_print(void){
    return (uint64_t)ocall_print_string;
}
