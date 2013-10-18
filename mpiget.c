#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <hyperclient.h>
const char* coordinator="127.0.0.1";
uint16_t  port=1987;

int main(int argc, char* args[]){
	int myrank;
	MPI_Status status;
	struct timeval beginTime, endTime;

	int req_size=64,range=32*1024;
	int64_t ret=0,loop_ret=0;
	enum hyperclient_returncode retcode,loop_retcode;
	//get key which is 0 and read attributes of key 0
	struct hyperclient* client=hyperclient_create(coordinator,port);
	char* space="datafile1";
	struct hyperclient_attribute* attrs_get;
	size_t attrs_get_size;
	//init MPI
	MPI_Init(&argc,&args);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);	//get current rank number
	if(myrank==0){
		//time begin
		gettimeofday(&beginTime,NULL);
		//set barrier
		MPI_Barrier(MPI_COMM_WORLD);
		int offset=myrank*range;
		while(offset<range*(myrank+1)){
			//define key and buffer each time
			char* key=malloc(req_size);
			memset(key,0,req_size);	//init key
			sprintf(key,"%ld",offset);
			size_t key_size=strlen(key);
			//failure  here.
			ret=hyperclient_get(client,
					space,
					key,
					key_size,
					&retcode,&attrs_get,&attrs_get_size);
			while(loop_ret<ret){
				loop_ret=hyperclient_loop(client,5000,&loop_retcode);
			}
			printf("Rank: %d\tret: %d\tretcode: %d\tloop_ret: %d\tloop_retcode: %d\n",myrank,ret,(int)retcode,loop_ret,(int)loop_retcode);
			if(loop_retcode==HYPERCLIENT_SUCCESS){
				printf("Get %d size of attribute\n",(int)attrs_get_size);
				//destroy attribute
				hyperclient_destroy_attrs(attrs_get,attrs_get_size);
			}else{
				printf("Cannot get attributes\n");
			}
			offset+=req_size;
			free(key);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		gettimeofday(&endTime,NULL);
		printf("Req_size: %d, time: %lu\n",req_size,(endTime.tv_sec-beginTime.tv_sec)*1000000+endTime.tv_usec-beginTime.tv_usec);
		//Finish communication
		MPI_Finalize();
		/*remove space here.*/
		retcode=hyperclient_rm_space(client,space);
		if(retcode!=HYPERCLIENT_SUCCESS){
			printf("Code %d:Failed to remove space\n",retcode);
		}
		hyperclient_destroy(client);

	}else{
		//set barrier
		MPI_Barrier(MPI_COMM_WORLD);
		int offset=myrank*range;
		while(offset<range*(myrank+1)){
			char* key=malloc(req_size);
			memset(key,0,req_size);
			sprintf(key,"%ld",offset);
			size_t key_size=strlen(key);
			ret=hyperclient_get(client,
					space,
					key,
					key_size,
					&retcode,&attrs_get,&attrs_get_size);
			while(loop_ret<ret){
				loop_ret=hyperclient_loop(client,5000,&loop_retcode);
			}
			if(loop_retcode==HYPERCLIENT_SUCCESS){
				printf("Get %d size of attribute\n",(int)attrs_get_size);
				hyperclient_destroy_attrs(attrs_get,attrs_get_size);
			}else{
				printf("Cannot get attributes\n");
			}
			offset+=req_size;
			free(key);
		}
		//wait if finished
		MPI_Barrier(MPI_COMM_WORLD);
	}


}

