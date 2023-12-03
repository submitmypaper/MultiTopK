#pragma once
#include<iostream>
#include<vector>
#include <stack>
#include <list>
#include <queue>
#include <map>
#include <math.h>
#include <random>
using namespace std;

struct Object
{
	double value;
	long oid;
	int don;		
};

class TStream
{
public:

	struct QParameter {
		pair<int, int> slide;	
		pair<int, int> n;		
		pair<int, int> k;		
	};

	struct Query
	{
		int k;
		long n;
		long s;
		long index;
	};


	double min_pmk;					
	deque<Query> workLoadVec;
	int commonsplit;
	long conmmonslide;
	list<Object> resultList;	
	list<Object> slideList;
	void ReadDataSourceFile(int i);
	long GetDataStreamBegin();
	long GetDataStreamTag();
	void updateDataFlow();
	void updateResultList(Object onew, int k);
	void RandomNormalWorkLoad(int qnum, QParameter qparam);	
	void outputTopKResult(long index, int k);
	void updateExpire(long allindex);
	Object searchMax();
	void updateResult();
	void initM0(int partsize);
	void Init(int num);							
	void SetDataStreamBegin(int begin);				
	void SetDataStreamTag(int tag);					
	void initCandidate(Query& query);
	FILE* fp1;
	deque<Object> vecData;							
	list<Object> candidatelist;
	vector<Object> pm1kSet;			
	vector<Object> p0candidates;
	pair<int, double> maxData;			
	double maxtest;
	double minData = 0x3f3f3f3f;
	long dataStreamBegin = 0;						
	long dataStreamTag = 0;							
	long dataStreamEnd = 0;
	int dominatenum = 0;			
	vector<Object> out_objs;
	vector<Object> in_objs;
};

