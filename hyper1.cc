#include <hyperclient.h>
/*
    Refined by zixiong miao on Sep 24th, 2013
    Add a space and a key with 3 attributes. First,Last,Phone.
*/
using namespace std;

int main()
{
    cout << "Entered main" << endl;
    hyperclient_attribute test_attr[3];

    test_attr[0].attr = "first";
    test_attr[0].value = "zixiong";
    test_attr[0].value_sz = strlen(test_attr[0].value);
    test_attr[0].datatype = HYPERDATATYPE_STRING;

    test_attr[1].attr = "last";
    test_attr[1].value = "miao";
    test_attr[1].value_sz = strlen(test_attr[1].value);
    test_attr[1].datatype = HYPERDATATYPE_STRING;

    test_attr[2].attr = "phone";
    test_attr[2].value = "7736009200";
    test_attr[2].value_sz = sizeof(int64_t);
    test_attr[2].datatype = HYPERDATATYPE_INT64;

    hyperclient_returncode retcode;
    const char *key = "zixiongmiao";
    hyperclient test_client("127.0.0.1", 1987);
    const char* description="space phonebook key zixiongmiao attributes first, last, int phone";
    hyperclient_returncode r=test_client.add_space(description);
    cout<<r<<endl;

    int64_t ret = test_client.put("phonebook", key, strlen(key), test_attr, 3, &retcode);

    cout << "put ret is " << ret << " return code is " << retcode << endl;

    int64_t loop_ret = 0;
/*
    while ((loop_ret = test_client.loop(1, &retcode)) != ret)
    {
        cout << ret << retcode << endl;
    }
*/
    loop_ret=test_client.loop(10,&retcode);
    cout << "loopput ret is " << loop_ret << " return code is " << retcode << endl;

    //define the container for the attribute get

    hyperclient_attribute *test_get_attr;
    hyperclient_returncode retcode1;
    size_t get_size = 0;
    int64_t ret1;
    ret1 = test_client.get("phonebook", key, strlen(key), &retcode1, &test_get_attr, &get_size);
    cout<< "get ret is "<< ret1 <<" return code is "<<retcode1<<endl; 
//#if 0
/*
    while ((ret = test_client.loop(10, &retcode)) != ret1)
    {
        cout << ret << retcode << endl;
    }
*/
//#endif
    loop_ret=test_client.loop(10,&retcode1);
    cout<<"loopget ret is "<<loop_ret<<" return code is "<<retcode1<<endl;
    if(get_size != 0)
    {
        cout << "we get something " << endl;

        for(int i = 0; i < get_size; i++)
        {
            cout << "attr " << i << ": " << test_get_attr[i].attr << ", " << test_get_attr[i].value <<endl;
        }
    }
    else
    {
        cout << "we get nothing" << endl;
    }

    return 0;
}

