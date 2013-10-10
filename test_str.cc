/*
	this program is to test the function of sizeof() and strlen(). See return value of two different functions. 
*/	
#include<iostream>
#include<stdlib.h>
#include<string.h>
using namespace std;

int main(){
	char* testchar="test";
	string teststr="string";
	cout<<"Use sizeof(): "<<sizeof(testchar)<<" "<<sizeof(teststr)<<endl;
	cout<<"Use strlen(): "<<strlen(testchar)<<endl;
	return 0;

}
