#include <iostream>
#include "Qmeta.h"
//#include "TStream.h"
using namespace std;


int main()
{
    clock_t startTime, endTime;
    int plusN[] = { 3,10,3,10,3,10,3,10,3,10,3,10,3,10,  3,10,3,10,3,10,3,10,3,10,3,10,    6,13,4,11,3,10,2,9,1,8,1,7,1,6   ,3,10,3,10,3,10,3,10,3,10,3,10 };
    int powS[] = { 7,7,9,9,10,10,11,11,12,12,13,13,14,14,   10,10,10,10,10,10,10,10,10,10,    7,7,9,9,10,10,11,11,12,12,13,13,14,14   ,10,10,10,10,10,10,10,10,10,10 };
    int k_range[] = { 50,150, 50,150, 50,150, 50,150, 50,150, 50,150,  7,10,10,50,50,100,100,200,200,500,500,1000,   50,150, 50,150, 50,150, 50,150, 50,150, 50,150,50,150  ,50,150, 50,150, 50,150, 50,150, 50,150, 50,150 };
    int query[] = { 100,100,100,100,100,100,100,100,100,100,100,100,100,100,
                100,100,100,100,100,100,100,100,100,100,100,100,
                100,100,100,100,100,100,100,100,100,100,100,100,100,100,
                10,10,50,50,100,100,200,200,500,500,1000,1000 };
    int wl_MtopS = 100;
    for (int i = 0; i < sizeof(plusN); )
    {
        Qmeta qmeta;
        qmeta.ReadDataFile();
        Qmeta::QParameter q;
        q.slide.first = powS[i];
        q.slide.second = powS[i];
        q.n.first = plusN[i];
        wl_MtopS = query[i];
        q.k.first = k_range[i++];
        q.n.second = plusN[i];
        q.k.second = k_range[i++];
        cout << "parmeter    Window range: " << q.n.first << "-" << q.n.second << "    k range: " << q.k.first << "-" << q.k.second << "  work load: " << wl_MtopS << endl;
        qmeta.generateMutiQuery(wl_MtopS, q);
        startTime = clock();
        qmeta.initMTopList();
        endTime = clock();
        cout << "  Init time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        startTime = clock();
        qmeta.updateDataFlow();
        endTime = clock();
        cout << "  Running Time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        cout << "Process " << 52428800 / ((double)(endTime - startTime) / CLOCKS_PER_SEC) << " data per second " << endl;
    }









}


