#include <stdio.h>
#include <string.h>
#include <hyperclient.h>
#include <mpi.h>

const char* coordinator = "127.0.0.1";
uint16_t coordinator_port = 1987;

int request_size = 64*1024;
int range = 8*1024*1024;

int kvs_put(int myrank) {

    /* create the hyperdex client */
    char* space = malloc(16);
    char* description = malloc(64);
    sprintf(space, "datafile%d", myrank);
    sprintf(description, "space %s key offset attributes data", space);

    enum hyperclient_returncode retcode, loop_retcode;
    int64_t ret, loop_ret;

    struct hyperclient* client = NULL;
    client = hyperclient_create(coordinator, coordinator_port);

    /* make sure the client is OK */
    if (!client) {
        fprintf(stderr, "Cannot create hyperdex client.\n");
        return;
    }

    /* remove the old space */
    retcode = 0;
    retcode = hyperclient_rm_space(client, space);
    if (retcode == HYPERCLIENT_SUCCESS) 
    {
        printf("Code: %d, removed old space successfully.\n", retcode);
    } else {
        fprintf(stderr, "Code %d, failed to remove the old space.\n", retcode);
    }

    /* add a space */
    retcode = 0;
    retcode = hyperclient_add_space(client, description);
    if (retcode == HYPERCLIENT_SUCCESS) {
        printf("Code: %d, created space successfully.\n", retcode);
    } else {
        fprintf(stderr, "Code %d, failed to create space.\n", retcode);
        if (retcode != HYPERCLIENT_DUPLICATE) {
            retcode = hyperclient_rm_space(client, space);
            hyperclient_destroy(client);
            return;
        }
    }
    
    /* LOOP
     *
     * In each iteration, we create a key-value pair and put it into
     * the key-value store. The value is an attribute
     */
    //uint32_t range = 32*1024; //32*1024*1024;
    //size_t request_size = 64; //64*1024
    uint64_t offset = 0;
    char* key = malloc(16);
    size_t key_size;
    char* buf;// = NULL;

    struct hyperclient_attribute* attribute = malloc(sizeof(struct hyperclient_attribute));
    if (attribute) {
        attribute->attr = "data";
        attribute->value_sz = request_size;
        attribute->datatype = HYPERDATATYPE_STRING;
    }

    buf = (char *)malloc(request_size);

    attribute->value_sz = request_size;
    int loop_count = 0;
    while (offset < range ) {
        /* prepare the key */
        memset(key, 0, 16);
        sprintf(key, "%ld", offset); /* we use offset as key */
        key_size = (size_t)strlen(key);
        
        /* prepare the attribute */
        attribute->value = buf;

        ret = 0; loop_ret = 0;
        retcode = 0; loop_retcode = 0;
        ret = hyperclient_put(
                client, space, 
                key, key_size,
                attribute, 1,
                &retcode);
        loop_count = 0;
        //while (loop_retcode != HYPERCLIENT_SUCCESS) 
        do {
            loop_ret = hyperclient_loop(
                    client, 50000, &loop_retcode);
            loop_count++;
            if (loop_count>10) {
               printf("(%d|%d)  ", myrank, loop_count);
            }
        } while (loop_retcode != HYPERCLIENT_SUCCESS && loop_ret < ret);
        offset += request_size;
    }

    /*
     * Destroy the client at the end.
     */
    hyperclient_destroy(client);

    /* clean up */
    free(buf);
    free(key);
    free(attribute);
    free(space);
    free(description);
} 

int main(int argc, char* args[]) {

    int myrank;
    MPI_Init(&argc, &args);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    if (argc < 2) {
        MPI_Finalize();
        return;
    }
    
    request_size = strtol(args[1], 0, 0) * 1024;
    
    if (myrank == 0) {
        struct timeval beginTime, endTime;
        MPI_Barrier(MPI_COMM_WORLD);
        gettimeofday(&beginTime, NULL);

        /* do the I/O */
        kvs_put(myrank);

        MPI_Barrier(MPI_COMM_WORLD);
        gettimeofday(&endTime, NULL);
        printf("Put Req_size: %d, time: %lu\n", request_size, (endTime.tv_sec-beginTime.tv_sec)*1000000+endTime.tv_usec-beginTime.tv_usec );
    } else {
        MPI_Barrier(MPI_COMM_WORLD);

        /* do the I/O */
        kvs_put(myrank);

        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Finalize();
}

