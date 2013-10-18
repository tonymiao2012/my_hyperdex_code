#include <stdio.h>
#include <string.h>
#include <hyperclient.h>

const char* coordinator = "127.0.0.1";
uint16_t coordinator_port = 1987;

int main() {

    /* create the hyperdex client */
    char* space = "datafile1";
    //char* description = "space datafile1 key offset1 attributes data";
    char * description= ""
	enum hyperclient_returncode retcode;

    struct hyperclient* client = hyperclient_create(coordinator, coordinator_port);

    /* make sure the client is OK */
    if (!client) {
        fprintf(stderr, "Cannot create hyperdex client.\n");
        return;
    }

    /* remove the old space */
    retcode = hyperclient_rm_space(client, space);
    if (retcode != HYPERCLIENT_SUCCESS) {
        fprintf(stderr, "Code %d: removing the old space\n", retcode);
    }
    /* add a space */
    retcode = hyperclient_add_space(client, description);
    if (retcode != HYPERCLIENT_SUCCESS) {
        fprintf(stderr, "Code %d: Failed to add space\n", retcode);
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
    uint64_t offset = 0;
    char* key = malloc(64);
    size_t key_size;
    struct hyperclient_attribute* attrs = malloc(sizeof(struct hyperclient_attribute));
    if (attrs) {
        attrs->attr = "data";
        attrs->value_sz = request_size;
        attrs->datatype = HYPERDATATYPE_STRING;
    }
    char* buf;// = NULL;

    while (offset < range ) {
        /* prepare the key */
        sprintf(key, "%ld\n", offset); /* we use offset as key */
		//key="offset1";
        key_size = (size_t)strlen(key);
        
        /* prepare the attribute */
        buf = (char *) malloc(request_size);

        memset(buf, 0, request_size);
        memset(buf, 'a', request_size-1);
        printf("%s\n", buf);
        
        attrs->value = buf;

        int64_t putret=hyperclient_put(
                client, space, 
                key, key_size,
                attrs, 1,
                &retcode);
		//add test here
		/*
		enum hyperclient_returncode* loopretcode;
		int64_t loopret=hyperclient_loop(client,10,&loopretcode);
		printf("%d",loopret);
		*/
       if (retcode == HYPERCLIENT_SUCCESS) {
		//if(loopretcode==HYPERCLIENT_SUCCESS){
            printf("Put data: %d\n", (int)request_size);
            offset += request_size;
            free(buf);
        } else {
            fprintf(stderr, "Code %d: Failed to put data in.\n", retcode);
            free(buf);
            break;
        }
    }

    /* clean up */
    free(key);
    free(attrs);

    /******************************************************
     * Now let's read the data back 
     *
     */
	/*
    offset=0;
    key = malloc(64);
    struct hyperclient_attribute** attrs_got; // = malloc(sizeof(struct hyperclient_attribute));
    size_t* attrs_got_size;

    while (offset < range ) {
        sprintf(key, "%ld\n", offset); 
        key_size = strlen(key);

        printf("111\n");
        hyperclient_get(
                client, space,
                key, key_size,
                &retcode,
                attrs_got, attrs_got_size);

        printf("222\n");
        printf("Got %d bytes of data: %s\n", (int)*attrs_got_size, (*attrs_got)->value);
        printf("333\n");
        hyperclient_destroy_attrs(*attrs_got, *attrs_got_size);
        offset += request_size;
    }
    free(key);

    // remove the space 
    retcode = hyperclient_rm_space(client, space);
    if (retcode != HYPERCLIENT_SUCCESS) {
        fprintf(stderr, "Code %d: Failed to remove space\n", retcode);
    }

    // Destroy the client at the end.
    
    hyperclient_destroy(client);
	*/
} 


