#pragma once
#include "TStream.h"
#include "MaxHeap.h"
#include "MinHeap.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <list>
#include <math.h>
#include <map>
#include <queue>
#include <random>

#pragma warning(disable:4996)
using namespace std;
class Tree
{
public:
	struct eNode {
		long st;
		long ed;
		int lbound;
		int ubound;
		eNode() {}
		eNode(long st, long ed, int lbound, int ubound) : st(st), ed(ed), lbound(lbound), ubound(ubound) {}
	};


	struct TreeStruct {
		int leafnum;
		int leaflen;
		int partition;
	};


	list<TreeNode*> nodeQueue;
	TreeNode* root;
	FILE* fp1;
	double RandomNumber = 1;
	double theata = 0;
	TreeStruct tstruct;
	void initEnodeVec(TStream& tstream);
	void initEnode(TStream& ts);
	void initTreeStruct(vector<eNode>& values, int kmin, int kmax);
	void splitElist(TStream& ts, list<pair<long, long>>& elist);
	void initTreeNode();
	void initTreeParm(int kmin);
	TreeNode* creatNewNode(long st, long ed, int kmin, int kmax);
	void updateDataFlow(TStream& tstream);
	void updateBinaryTree(Object Onew, int ksum, int kmax);
	void mergeNodes(TStream& tstream, int psum);
	void mergeTwoNeighbor(TreeNode* left, TreeNode* right, TreeNode* tnode, int mink);
	void countCandidate();
	void shrunkNodes(long combst, long combed, int kmax);
	TreeNode* deleteNode(TreeNode* left);
	void purgeExpired(long tag);
	void initNodeHeap(long ed);
	void outputResult(TStream& tstream);
	void localCandidateSelection(int upperbound, Object onew);
	int updateMaxHeap(ResultParm* rp, long bound);
	map<double, Object, DescendingCompare> candiMap;
	map<int, pair<int, int>> splitMap;
	vector<eNode> enodeVec;
	list<TreeNode*> headNodeList;
	MaxHeap nodeHeap;
	MinHeap partHeap;
private:
};

