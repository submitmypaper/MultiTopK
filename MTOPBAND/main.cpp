#include <iostream>
#include "Qmeta.h"
using namespace std;


int main()
{
    clock_t startTime, endTime;
    int plusN[] = { 3,10,3,10,3,10,3,10,3,10,3,10,  3,10,3,10,3,10,3,10,3,10,3,10,    6,13,4,11,3,10,2,9,1,8,1,7,1,6 ,    3,10,3,10,3,10,3,10,3,10, };
    int powS[] = { 7,7,10,10,11,11,12,12,13,13,14,14,   10,10,10,10,10,10,10,10,10,10,    7,7,9,9,10,10,11,11,12,12,13,13,14,14   ,10,10,10,10,10,10,10,10,10,10 };
    int k_range[] = { 50,150, 50,150, 50,150, 50,150, 50,150, 50,150,  7,10,10,50,50,100,100,200,200,500,500,1000,   50,150, 50,150, 50,150, 50,150, 50,150, 50,150,50,150  ,50,150, 50,150, 50,150, 50,150, 50,150 };
    int query[] = { 100,100,100,100,100,100,100,100,100,100,100,100,   100,100,100,100,100,100,100,100,100,100,100,100,   100,100,100,100,100,100,100,100,100,100,100,100,100,100,  10,10,100,100,200,200,500,500,1000,1000 };
    int wl_MtopBand = 100;
    for (int i = 0; i < sizeof(plusN);)
    {
        Qmeta qmeta;
        qmeta.ReadDataFile();
        Qmeta::QParameter q1;
        q1.slide.first = powS[i];
        q1.slide.second = powS[i];
        q1.n.first = plusN[i];
        wl_MtopBand = query[i];
        q1.n.second = plusN[i + 1];
        q1.k.first = k_range[i];
        q1.k.second = k_range[i + 1];
        cout << "parmeter    Window range: " << q1.n.first << "-" << q1.n.second << "    k range: " << q1.k.first << "-" << q1.k.second << "  work load: " << wl_MtopBand << endl;
        qmeta.generateMutiQuery(wl_MtopBand, q1);
        startTime = clock();
        qmeta.initMTopList();
        endTime = clock();
        cout << "  Init time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        startTime = clock();
        qmeta.updateDataFlow();
        endTime = clock();
        cout << "  Running Time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
        cout << "Process " << 52428800 / ((double)(endTime - startTime) / CLOCKS_PER_SEC) << " data per second " << endl;
        i = i + 2;
    }









}


