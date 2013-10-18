/*
Author: Zixiong Miao Date: Sep 23rd, 2013
Description: Test put function for hyperdex
*/
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<hyperclient.h>
using namespace std;
#define x_64k 1024*64 //64kb
#define attr_size 2		//attribute size number
#define key_size 1		//key size number


/*design part
  In this part, we may include some basic design. For example, we design processes number is 8, 16, 64. And "space" name is process number. Each process has some jobs to run. Lets make it a 64kb job. Key is job number. 
*/
//void keyName(const char* key);

int main(){

	const char* coordinator="127.0.0.1";
	uint16_t port=1987;
	int i,n;
	cout<<"Test input begin"<<endl;
	hyperclient client1(coordinator,port);	//claim a client to use
	//add place before put
	const char* space="try1";
	client1.rm_space(space);
	const char* description="space try1 key test attributes a0, a1";
	hyperclient_returncode re=client1.add_space(description);
	cout<<re<<endl;
	//const char* key[key_size];	//processes names
	hyperclient_attribute attr[attr_size];	//declare 8 jobs per process
	//attribute value initialized
	//attr[0].attr="attr0";
	//attr[1].attr="attr1";
	for(i=0;i<attr_size;i++){

		//char* memory=malloc(sizeof(char)*x_64k);	//buffer for 64k char 
		char* memory=(char*)malloc(x_64k);
		if(memory==NULL){
			exit(1);
		}
		/*
		//add something to memory
		for(n=0;n<x_64k;n++){
			memory[n]=rand()%26+'a';
		}
		attr[i].value=memory;
		*/
		///*
		char* t=new char[sizeof(i)];
		sprintf(t,"%d",i);
		string temp="a";
		temp+=t;
		//cout<<temp.length()<<temp.c_str()<<endl;
		//*shallow copy here. bug.
		attr[i].attr=temp.c_str();
		//*/
		//*/
		//cout<<attr[i].attr<<endl;
		//attr[i].value_sz=strlen(attr[i].value);
		//cout<<attr[i].value_sz<<endl;
		attr[i].datatype=HYPERDATATYPE_STRING;
		//bugs here.
		free(memory);	//free malloc
	}
	hyperclient_returncode retcode;
	//put section here
	i=0;
	//while(i<key_size){ 
	const char* key="test";
		//ret=client1.put(space,key[i],strlen(key[i]),attr,attr_size,&retcode);
		int64_t ret=client1.put(space, key, strlen(key), attr, attr_size, &retcode);
		cout<<"put return is "<<ret<<" return code is "<<retcode<<endl;
	//	i++;
	//}
	//loop put
	int64_t loop_ret=client1.loop(1000,&retcode);
	cout<<"looput return is "<<loop_ret<<" return code is "<<retcode<<endl;
}
/*
void keyName(const char* key){	
	int i;
	char t[key_size];
	for(i=0;i<key_size;i++){
		//char t[key_size];
		sprintf(t,"%d",i);
		string temp="Process:";
		temp+=t;
		key[i]=temp.c_str();
	}
}
*/
	//key init
	//keyName(key[key_size]);
	/*
	char m[key_size];
	for(i=0;i<key_size;i++){
		sprintf(m,"%d",i);
		string temp="process";
		temp+=m;
		key[i]=temp.c_str();
		cout<<key[i]<<endl;
	}
	*/
