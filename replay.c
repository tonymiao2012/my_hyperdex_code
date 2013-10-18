/*
Author: zixiong miao Date: Oct 4th.
Description: Read trace and replay it on hyperdex.
 */
//#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<hyperclient.h>
//struct type of file

#define KEY_LENGTH 64
const char* coordinator="127.0.0.1";
uint16_t  port=1987;

typedef struct tracetype
{
	size_t offset;
	size_t size;
	char operation[21];
}tracetype;

void replay(){

	struct hyperclient* client=hyperclient_create(coordinator,port);
	if(!client){
		fprintf(stderr,"Cannot create hyperdex client.\n");
		return;
	}
	enum hyperclient_returncode retcode=0;
	enum hyperclient_returncode loop_retcode=0;
	int64_t ret=0,loop_ret=0;
	const char* space="space2";
	const char* description="space space2 key offset attributes data";
	//return value
	struct hyperclient_attribute* attrs_get;
	//use only one key and buf
	char* key;
	char* buf;
	size_t attrs_get_size,key_size;
	//remove space first
	retcode=0;

	retcode=hyperclient_rm_space(client,space);
	if(retcode==HYPERCLIENT_SUCCESS){
		printf("Code: %d,removed old space successfully.\n",retcode);
	}else{
		fprintf(stderr,"Code %d,failed to remove the old space.\n",retcode);
	}

	//add space
	retcode=hyperclient_add_space(client,description);
	if(retcode!=HYPERCLIENT_SUCCESS){
		perror("cannot add space.\n");
		retcode=hyperclient_rm_space(client,space);
		hyperclient_destroy(client);
		exit(1);
	}else{
		printf("Code:%d,created space successfully.\n",retcode);
	}
	//read file
	FILE* file1;
	file1=fopen("./data","r");
	if(file1==NULL){
		printf("Cannot open file");	
		retcode=hyperclient_rm_space(client,space);
		hyperclient_destroy(client);
		return;
	}else{
		//find how long of file and read by lines.
		//get new malloc space
		tracetype* t=(tracetype*)malloc(sizeof(tracetype));
		key=malloc(KEY_LENGTH);
		while(fscanf(file1,"%ld %20s %ld\n",&(t->offset),t->operation,&(t->size))!=EOF){
			//read by struct type
			//fscanf(file1,"%d %20s %d\n",&t->offset,t->operation,&t->size);
			//translate into hyperdex
			printf("offset:%ld,operation:%s,size:%ld.\n",t->offset,t->operation,t->size);
			if(strcmp(t->operation,"mpi_read()")==0){
				memset(key,0,KEY_LENGTH);
				sprintf(key,"%ld",t->offset);
				key_size=(size_t)strlen(key);
				//check if data has been in hyperspace already.
				ret=hyperclient_get(client,space,key,key_size,&retcode,&attrs_get,&attrs_get_size);
				while(loop_ret<ret){
					loop_ret=hyperclient_loop(client,500,&loop_retcode);
				}
				if(loop_retcode!=HYPERCLIENT_SUCCESS){
					printf("Get operation is not successful.Continue.");
					return;
				}
				printf("This is got operation, offset is %ld, replay succeed",t->offset);
				//destory attributes
				hyperclient_destroy_attrs(attrs_get,attrs_get_size);
			}else if(strcmp(t->operation,"mpi_write()")==0){
				//hyperclient_put
				memset(key,0,KEY_LENGTH);
				sprintf(key,"%ld",t->offset);
				//printf("%s\n",key);
				key_size=(size_t)strlen(key);
				//how about attributes?
				struct hyperclient_attribute* attrs_put=malloc(sizeof(struct hyperclient_attribute));
				if(attrs_put){
					size_t sss = t->size;
					attrs_put->attr="data";
					attrs_put->value_sz=sss;
					//create a buf of such size with data
					//attrs_put->value="test";

					printf("t----------->size: %ld.\n", t->size);
					printf("t----------->size: %ld.\n", sss);

					buf=malloc(sss);
					//memset(buf,'m',t->size);
					//printf("%s\n",buf);
					attrs_put->value=buf;	

					attrs_put->datatype=HYPERDATATYPE_STRING;
				}
				ret=hyperclient_put(client,space,key,key_size,attrs_put,1,&retcode);
				while(loop_ret<ret){
					loop_ret=hyperclient_loop(client,500,&loop_retcode);
				}
				if(loop_retcode==HYPERCLIENT_SUCCESS){
					//printf("Put operation successful,buf is %s\n",attrs_put->value);
				}else{
					fprintf(stderr,"Code %d:Failed to put data in.\n",loop_retcode);
					break;
				}
				free(buf);
			}else{
				printf("cannot read operation\n");
			}
		}
		free(t);
		free(key);
		//close file here.
		fclose(file1);
	}
	//free hyperclient.Or remove space
	hyperclient_destroy(client);

}

//main
int main(){
	//struct tracetype* t=(struct tracetype*)malloc(sizeof(tracetype));
	replay();
	printf("Finish replay.\n");
	return 0;
}

