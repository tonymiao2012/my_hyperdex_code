#include<hyperclient.h>

using namespace std;

int main(){
	cout<<"test start"<<endl;
	const char* coordinator="127.0.0.1";
	uint16_t port=1987;		//caution here, the int type
	const char* space="test";	//space name
	//try{
	hyperclient* client1=hyperclient_create(coordinator,port);
	const char* key="user";
	//successful here
	const char* description="space test key user attributes pot";
	//add space here
	hyperclient_rm_space(client1,space);
	hyperclient_returncode r=hyperclient_add_space(client1, description);
	cout<<r<<endl;
	//decline the attribute of subspace
	hyperclient_attribute attr;	//a subspace may contain several of subspaces
	attr.attr="pot";
	attr.value="hello world";
	attr.value_sz=strlen(attr.value);
	attr.datatype=HYPERDATATYPE_STRING;
	hyperclient_returncode op_status;
	int64_t op_id=hyperclient_put(client1,//hyperclient*
			space,	//const char*
			key,	//const char*
			strlen(key),	//size_t
			&attr,	//hyperclient_attribute*
			1,		//size_t attrs_sz
			&op_status);	//hyperclient_returncode
	cout<<"op_id: "<<op_id<<endl;
	cout<<"op_status: "<<op_status<<endl;
	//get part testing
	size_t attrs_sz;
	hyperclient_attribute* attr1;
	hyperclient_returncode op_status1;
	int64_t op_id1=hyperclient_get(client1,
			space,
			key,
			strlen(key),
			&op_status1,
			&attr1,
			&attrs_sz);
	cout<<"Finish get part"<<endl;
	cout<<"op_id1: "<<op_id1<<endl;
	//hyperclient_attribute atr=attr1[0];
	//cout<<&atr.value<<endl;
	/*
	}catch(exception &e){
		cerr<<"error: "<<e.what()<<endl;
		return EXIT_FAILURE;
	}
	*/
}
