#pragma once
#include <vector>
#include <math.h>
#include <random>
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include<set>
#include <deque>
#pragma warning(disable:4996)
#define MAX 100
using namespace std;

struct Object {
	long id;		
	double value;	
};

class Qmeta
{
public:
	Qmeta();
	~Qmeta();

	struct Query
	{
		long winsize;
		int k;
		int slide;
	};
	struct Win
	{
		long index;
		int adpk;
		int cur_k;
		int slide;
		vector<int> serve;
	};

	struct Meta
	{
		long winsize;   
		vector<int> adaptk;	
		vector<Win> predictWin;	
	};

	struct MTop
	{
		double value;
		long id;
		long leftwin;			
		long rightwin;
		list<long> lbp;
		set<long> lbpset;
	};

	struct ENode {
		int lbp;			
		ENode* nextEdge;	
	};

	struct QParameter {
		pair<int, int> slide;	
		pair<long, long> n;		
		pair<int, int> k;		
	};

	long overall_indx = 0;			
	long maxWinSize = 0;			
	int commonSlide;
	double RandomNumber = 1;
	vector<Query> mutiQuery;	
	vector<Win>  preWinVec;		
	vector<long>  queryWinSize;
	map<int, vector<pair<int, int>>>  winIndMap;	
	list<MTop> resultList;	
	list<Object> slideList;		
	set<long> domWinNum;			
	void generateMutiQuery(int qnum, QParameter qparam);
	void printAdjacency();
	void combineMutiWin();
	void combineMutiWin_ComSlide();
	void integrateMutiWin();
	int combineMutiQuery();
	void initMTopList();
	void updateDataFlow();
	void outputTopKResult();
	void purgeExpiredWindow();
	void updateResultList(Object onew);
	void updateSuperTopk();
	void updateLifeSpan(MTop& iter, list<long>& lbplistin);
	void copyLbpset(MTop& m, MTop& iter);
	void mergeLeftWin(MTop& iterval);
	static bool deccomp(int i, int j);
	int getPreWinIndex(long index);
	bool isAllk();
	void RandomNormalWorkLoad(int qnum, QParameter qparam);
	void ReadDataFile();							
	Object GetDataStream(long intObjectNumber);		
	long GetDataStreamLength();						
	double GetLength();								
	long GetDataStreamBegin();						
	long GetDataStreamTag();							
	void SetDataStreamBegin(long begin);				
	void SetDataStreamTag(long tag);					
	void AddDataStreamBegin(long outflow);
	void AddDataStreamTag(long inflow);
private:
	deque<Object> vecDataStream;					
	double length;									
	long dataStreamBegin = 0;						
	int dataStreamTag = 0;							

};

