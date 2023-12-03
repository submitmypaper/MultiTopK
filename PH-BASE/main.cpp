#include <iostream>
#include "Tree.h"
#include "TStream.h"
int main()
{

    clock_t startTime, endTime;
    int pow[] = { 11,17,11,18,12,19,13,20,14,21,15,22,16,23,  13,20,13,20,13,20,13,20,13,20,13,20, 13,20,13,20,13,20,13,20,13,20,13,20,13,20 ,   13,20,13,20,13,20,13,20,13,20 };
    int k_range[] = { 100,60,100,60,100,60,100,60,100,60,100,60,100,60,  10,7,50,20,100,50,200,100,500,200,1000,500 , 100,60,100,60,100,60,100,60,100,60,100,60,100,60 ,  100,60,100,60,100,60,100,60,100,60 };
    int query[] = { 100,100,100,100,100,100,100,100,100,100,100,100,100,100,   100,100,100,100,100,100,100,100,100,100,100,100,  100,100,100,100,100,100,100,100,100,100,100,100,100,100,  10,10,100,100,200,200,500,500,1000,1000 };
    int wl;
    for (int i = 0; i < sizeof(pow); )
    {
        TStream tstream;
        Tree tree;
        TStream::QParameter q;
        tstream.ReadDataFile();
        tstream.SetQueryTimes(1000);
        q.k.first = k_range[i];
        wl = query[i];
        q.n.first = pow[i++];
        q.k.second = k_range[i];
        q.n.second = pow[i++];
        tstream.RandomNormalWorkLoad(wl, q);
        cout << "parmeter    Window range: " << q.n.first << "-" << q.n.second << "    k range: " << q.k.first << "-" << q.k.second << "  work load: " << wl << endl;
        startTime = clock();
        tree.initEnodeVec(tstream);
        tree.initEnode(tstream);
        tree.initTreeNode();
        endTime = clock();
        cout << "  Init time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        startTime = clock();
        tree.updateDataFlow(tstream);
        endTime = clock();
        cout << "  Running Time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        cout << "Process " << 52428800 / ((double)(endTime - startTime) / CLOCKS_PER_SEC) << " data per second " << endl;
    }
}

