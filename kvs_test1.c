#include <stdio.h>
#include <string.h>
#include <hyperclient.h>

const char* coordinator = "127.0.0.1";
uint16_t coordinator_port = 1987;

int kvs_test() {

    /* create the hyperdex client */
    char* space = "datafile1";
    char* description = "space datafile1 key offset attributes data";

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
    }else{
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
    //uint32_t shift = 1024;
    uint32_t range = 32*1024; //32*1024*1024;
    size_t request_size = 64; //64*1024
    uint64_t offset;
    char* key = malloc(64);
    size_t key_size;
    struct hyperclient_attribute* attribute = malloc(sizeof(struct hyperclient_attribute));
    if (attribute) {
        attribute->attr = "data";
        attribute->value_sz = request_size;
        attribute->datatype = HYPERDATATYPE_STRING;
    }
    char* buf;// = NULL;

    offset = 0;
    int index = 1;
    while (offset < range ) {
        /* prepare the key */
        memset(key, 0, 64);
        sprintf(key, "%ld", offset); /* we use offset as key */
        key_size = (size_t)strlen(key);
        printf("Key: %s\n", key);
        
        /* prepare the attribute */
        buf = (char *) malloc(request_size);

        memset(buf, 0, request_size);
        if ( (index%2) == 0 ) {
            memset(buf, 'b', request_size-1);
        } else {
            memset(buf, 'a', request_size-1);
        }
        ++index;
        
        attribute->value = buf;

        ret = 0; loop_ret = 0;
        retcode = 0; loop_retcode = 0;
        ret = hyperclient_put(
                client, space, 
                key, key_size,
                attribute, 1,
                &retcode);
        while (loop_ret < ret) {
            loop_ret = hyperclient_loop(
                    client, 5000, &loop_retcode);
        }

        free(buf);
        printf("Ret: %ld\tRetCode: %d\tRet2: %ld\tRetCode2: %d\n", ret, retcode, loop_ret, loop_retcode);
        if (loop_retcode == HYPERCLIENT_SUCCESS) {
        //if (ret == loop_ret) {
            printf("Put data: %d\n", (int)request_size);
            offset += request_size;
        } else {
            fprintf(stderr, "Code %d: Failed to put data in.\n", loop_retcode);
            break;
        }
    }

    /* clean up */
    free(key);
    free(attribute);

    printf("*****************Now**Read**Data**Back*******************\n");
    /******************************************************
     * Now let's read the data back 
     *
     */
    key = malloc(64);
    struct hyperclient_attribute* attrs_got; // = malloc(sizeof(struct hyperclient_attribute));
    size_t attrs_got_size = 0;

    offset = 0;
    while (offset < range ) {
        memset(key, 0, 64);
        sprintf(key, "%ld", offset); 
        key_size = strlen(key);
        printf("Key: %s\n", key);

        ret = 0; loop_ret = 0;
        retcode = 0; loop_retcode = 0;
        ret = hyperclient_get(
                client, space,
                key, key_size,
                &retcode,
                &attrs_got, &attrs_got_size);
        while (loop_ret < ret) {
            loop_ret = hyperclient_loop(client, 5000, &loop_retcode);
        }

        printf("Ret: %ld\tRetCode: %d\tRet2: %ld\tRetCode2: %d\n", ret, retcode, loop_ret, loop_retcode);
        printf("Size: %d\n", (int)attrs_got_size); 
            
        /*if (attrs_got_size > 0) {*/
        //if (loop_ret == ret) {
        if (loop_retcode == HYPERCLIENT_SUCCESS) {
            printf("Got %d bytes of data: %s\n", (int)attrs_got_size, attrs_got->value);
            hyperclient_destroy_attrs(attrs_got, attrs_got_size);
        } else {
            printf("Wrong!\n");
        }
        offset += request_size;
    }
    free(key);

    /* remove the space */
    retcode = hyperclient_rm_space(client, space);
    if (retcode != HYPERCLIENT_SUCCESS) {
        fprintf(stderr, "Code %d: Failed to remove space\n", retcode);
    }

    /*
     * Destroy the client at the end.
     */
    hyperclient_destroy(client);
} 

int main(int argc, char* args[]) {

    kvs_test();
}
