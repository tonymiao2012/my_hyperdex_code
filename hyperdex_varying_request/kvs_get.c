#include <stdio.h>
#include <string.h>
#include <hyperclient.h>
#include <mpi.h>

const char* coordinator = "127.0.0.1";
uint16_t coordinator_port = 1987;

int request_size = 64*1024;
int range = 8*1024*1024;

int kvs_get(int myrank) {

    /* create the hyperdex client */
    char* space = malloc(16);
    //char* description = malloc(64);
    sprintf(space, "datafile%d", myrank);
    //sprintf(description, "space %s key offset attributes data", space);

    enum hyperclient_returncode retcode, loop_retcode;
    int64_t ret, loop_ret;

    struct hyperclient* client = NULL;
    client = hyperclient_create(coordinator, coordinator_port);

    /* make sure the client is OK */
    if (!client) {
        fprintf(stderr, "Cannot create hyperdex client.\n");
        return;
    }
    
    uint64_t offset = 0;
    char* key = malloc(16);
    size_t key_size;
    char* buf;// = NULL;
    struct hyperclient_attribute* attrs_got;
    size_t attrs_got_size = 0;
    
    int loop_count = 0;
    while (offset < range) {
        /* prepare the key */
        memset(key, 0, 16);
        sprintf(key, "%ld", offset);
        key_size = strlen(key);

        ret = 0; loop_ret = 0;
        retcode = 0; loop_retcode = 0;
        ret = hyperclient_get(
                client, space,
                key, key_size,
                &retcode,
                &attrs_got, &attrs_got_size);
        loop_count = 0;
        //while (loop_retcode != HYPERCLIENT_SUCCESS) 
        do {
            loop_ret = hyperclient_loop(client, 50000, &loop_retcode);
            loop_count++;
            if (loop_count>10) {
                printf("(%d|%d)  ", myrank, loop_count);
            }
        } while (loop_ret < ret);
		printf("Rank %d offset %ld ret %ld loop_ret %ld\n", myrank, offset, ret, loop_ret);
        //} while (loop_retcode != HYPERCLIENT_SUCCESS && loop_ret < ret);
        offset += request_size;
        hyperclient_destroy_attrs(attrs_got, attrs_got_size);
        if (attrs_got->value_sz != request_size) {
            printf("Wrong!!! Actual size: %d\n", (int)attrs_got->value_sz);
        }
    }
    free(key);

    /*
     * Destroy the client at the end.
     */
    hyperclient_destroy(client);

    free(space);
} 

int main(int argc, char* args[]) {

    int myrank;
    MPI_Init(&argc, &args);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	//get MPI size.
	if(myrank==0){
		int size;
		MPI_Comm_size(MPI_COMM_WORLD,&size);
		printf("MPI size: %d\n",size);
	}

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
        kvs_get(myrank);

        MPI_Barrier(MPI_COMM_WORLD);
        gettimeofday(&endTime, NULL);
        printf("Get Req_size: %d, time: %lu\n", request_size, (endTime.tv_sec-beginTime.tv_sec)*1000000+endTime.tv_usec-beginTime.tv_usec );
    } else {
        MPI_Barrier(MPI_COMM_WORLD);

        /* do the I/O */
        kvs_get(myrank);

        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Finalize();
}

