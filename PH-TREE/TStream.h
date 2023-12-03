#pragma once
#include <algorithm>
#include <vector>
#include <list>
#include <math.h>
#include <map>
#include <deque>
#pragma warning(disable:4996)
using namespace std;

struct Object {
	long id;
	double value;
	int dom;
};

struct DescendingCompare {
	bool operator()(const double& k1, const double& k2) const {
		return k1 > k2;
	}
};

struct Query
{
	int k;
	long n;
	int s;
	long index;
	int multi;
};

struct TreeNode {
	long st;
	long ed;
	list<Object> nodelist;
	int lowerbound;
	int upperbound;
	int isleaf;
	TreeNode* left;
	TreeNode* right;
};

class TStream
{
public:
	TStream();
	~TStream();

	struct BaseSplit {
		int level;
		int partlen;
		int kmax;
	};


	struct QParameter {
		pair<int, int> slide;
		pair<int, int> n;
		pair<int, int> k;
	};

	void ReadDataFile();
	int GetDataStreamLength();
	double GetLength();
	int GetDataStreamBegin();
	int GetDataStreamTag();
	int GetCommonPartition();
	int GetQueryTimes();
	void SetQueryTimes(int times);
	void SetDataStreamBegin(int begin);
	void SetDataStreamTag(int tag);
	void AddDataStreamBegin(int outflow);
	void AddDataStreamTag(int inflow);
	long GetVecSize();
	long GetIndexByQWin(int n);
	void initTimeStamp();
	int updateTimestp(int count);
	Object GetVecById(long index);
	Object GetDataById(long index);
	int GetWorkLoadVecSize();
	Query GetWorkLoadVecById(int id);
	vector<Object>::const_iterator VecBegin();
	void generateMutiQuery(int qnum, QParameter qparam);
	void WorkLoadInit(int qnum, QParameter qparam);
	void RandomNormalWorkLoad(int qnum, QParameter qparam);
	double length;
	long dataStreamBegin = 0;
	long dataStreamTag = 0;
	long winTag;
	long maxn = 0, minn = 0;
	long winBegain = 0;
	int commonPartition;
	int queryTimes;
	int mink = 0, maxk = 0, mins = 0, maxs = 0, maxnk = 0;
	deque<Object> vecDataStreamQue;
	deque<Query> workLoadVec;
	vector<BaseSplit> levelkVec;
	map<long, int, DescendingCompare> queryMap;
	multimap<long, Query> timestpmap;
};

