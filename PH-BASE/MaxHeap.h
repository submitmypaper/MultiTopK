#include <iostream>
#include <vector>
#include <list>
#include "TStream.h"
#pragma warning(disable:4996)
using namespace std;




struct ResultParm {
	double value;
	long id;
	TreeNode* node;
	list<Object>::iterator pos;
	ResultParm(double value, long id, TreeNode* node, list<Object>::iterator position)
		: value(value), id(id), node(node), pos(position) {}

	ResultParm() : value(0), id(0), node(nullptr) {}
};

class MaxHeap {

public:
	MaxHeap() {}
	~MaxHeap() {}
	vector<ResultParm> heap;
	void siftUp(int index);
	void siftDown(int index);
	void insert(ResultParm obj);
	void changeMax(ResultParm obj);
	void removeMax();
	void clear();
	ResultParm getMax();
	bool empty();
	int getSize();
};


