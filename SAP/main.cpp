#include "TStream.h"
#include<iostream>
#include <algorithm>
using namespace std;

int main()
{
    clock_t startTime, endTime;
    //window length:100k,500k，1M，2M，5M，10M,  Default value：13-20(1M)
    int pow[] = { 11,17,12,19,13,20,14,21,15,22,16,23, 13,20,13,20,13,20,13,20,13,20,13,20 ,  13,20,13,20,13,20,13,20,13,20,13,20,13,20,   13,20,13,20,13,20,13,20,13,20,13,20 };
    int k_range[] = { 100,60,100,60,100,60,100,60,100,60,100,60, 10,7,50,20,100,50,200,100,500,200,1000,500 , 100,60,100,60,100,60,100,60,100,60,100,60,100,60, 100,60,100,60,100,60,100,60,100,60,100,60 };
    int s_range[] = { 4,4,4,4,4,4,4,4,4,4,4,4, 4,4,4,4,4,4,4,4,4,4,4,4 , 1024,1024,512,512,128,128,64,64,32,32,16,16,8,8,  4,4,4,4,4,4,4,4,4,4,4,4, };
    int query[] = { 100,100,100,100,100,100,100,100,100,100,100,100,  
                    100,100,100,100,100,100,100,100,100,100,100,100,  
                    100,100,100,100,100,100,100,100,100,100,100,100,100,100,  
                    10,10,50,50,100,100,500,500,1000,1000 };


    int wl_sap = 100;
    TStream tstream;
    //0：stock 100MB,1：trip_data50,2：random_data50,3：normal_data50
    tstream.Init(0);	
    int wl_SAP = 100;
    for (int i = 0; i < sizeof(pow); )
    {
        TStream::QParameter q;
        q.k.first = k_range[i];
        q.slide.first = s_range[i];
        q.n.first = pow[i++];
        q.k.second = k_range[i];
        q.n.second = pow[i++];
        cout << "parmeter    Window range: " << q.n.first << "-" << q.n.second << "    k range: " << q.k.first << "-" << q.k.second << "  work load: " << wl_SAP << endl;
        tstream.RandomNormalWorkLoad(wl_SAP, q);
        startTime = clock();
        tstream.updateDataFlow();
        endTime = clock();
        cout << "  Running Time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        cout << "Process " << 52428800 / ((double)(endTime - startTime) / CLOCKS_PER_SEC) << " data per second " << endl;
    }
}