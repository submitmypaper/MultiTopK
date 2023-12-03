#include "Tree.h"

bool compbyval(const Object& a, const Object& b) {
	return a.value > b.value;
}


bool compbyid(const Object& a, const Object& b) {
	return a.id < b.id;
}

int getRandom(int a, int b)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dis(a, b);
	return dis(gen);
}


inline void copyListToQueue(const list<int>& sourceList, queue<int>& destinationQueue) {
	queue<int> emptyQueue;
	destinationQueue.swap(emptyQueue);
	for (const int& item : sourceList) {
		destinationQueue.push(item);
	}
}

bool is2Power(long nNum)
{
	return nNum > 0 ? ((nNum & (~nNum + 1)) == nNum ? true : false) : false;
}

int num2Power(long n)
{
	int power = 0;
	while (n > 1) {
		n >>= 1;
		power++;
	}
	return power;
}

bool ifInNode(Query q, pair<long, long>& enode) {
	if (q.index >= enode.first && q.index < enode.second + 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}



int countMergTimes(int psum) {
	int times, result;
	if (is2Power(psum))
	{
		times = num2Power(psum);
	}
	else
	{
		result = log2(psum);
		while (result)
		{
			psum = psum - pow(2, result);
			if (is2Power(psum))
			{
				times = num2Power(psum);
				break;
			}
			else
			{
				result = log2(psum);
			}
		}
	}
	return times;
}

TreeNode* Tree::deleteNode(TreeNode* left) {
	delete left;
	return nullptr;
}

TreeNode* Tree::creatNewNode(long st, long ed, int k) {
	TreeNode* newnode = new TreeNode;
	newnode->st = st;
	newnode->ed = ed;
	newnode->kmax = k;
	newnode->left = nullptr;
	newnode->right = nullptr;
	return newnode;
}


void Tree::initTreeParm(int kmin) {
	int split = 4, count = 0, kmax;
	long gap = enodeVec[0].ed - enodeVec[0].st;
	kmax = enodeVec[0].kmax;
	splitMap.insert(pair<int, pair<int, int>>(2, make_pair(0, kmin)));
	for (auto it = enodeVec.begin(); it != enodeVec.end(); it++)
	{
		if (it->ed - it->st < gap)
		{
			splitMap.insert(pair<int, pair<int, int>>(split, make_pair(count, kmax)));
			split = split * 2;
			gap = it->ed - it->st;
			count = 1;
			kmax = it->kmax;
		}
		else
		{
			count++;
		}
	}
	splitMap.insert(pair<int, pair<int, int>>(split, make_pair(count, kmax)));
	tstruct.leaflen = enodeVec[enodeVec.size() - 1].kmax;
	tstruct.leafnum = (enodeVec[enodeVec.size() - 1].ed + 1) / (enodeVec[enodeVec.size() - 1].ed - enodeVec[enodeVec.size() - 1].st + 1);	//最大窗口除以最小分片
	tstruct.partition = enodeVec[enodeVec.size() - 1].ed - enodeVec[enodeVec.size() - 1].st + 1;
}

void Tree::splitElist(TStream& ts, list<pair<long, long>>& elist) {
	if (elist.empty())return;
	long st, mid;
	vector<eNode> values;		
	list<pair<long, long>>::iterator it;
	while (elist.size() < 4)
	{
		it = elist.begin();
		while (it != elist.end())
		{
			st = it->first;
			mid = ((it->second + 1) - st) / 2 + st;
			it->first = mid;
			elist.insert(it, pair<long, long>(st, mid - 1));
			values.push_back(eNode(st, mid - 1, ts.mink));
			values.push_back(eNode(mid, it->second, ts.mink));
			it++;
		}
		initTreeStruct(values, ts.maxk);
		values = vector<eNode>();
	}
}


void Tree::initTreeStruct(vector<eNode>& values, int k) {
	if (values.empty()) return;

	int index = 0;
	while (!nodeQueue.empty() && index < values.size()) {
		TreeNode* currentNode = nodeQueue.front();
		nodeQueue.pop_front();

		if (index < values.size()) {

			currentNode->right = creatNewNode(values[index].st, values[index].ed, k);
			nodeQueue.push_back(currentNode->right);
			index++;
		}

		if (index < values.size()) {
			currentNode->left = creatNewNode(values[index].st, values[index].ed, k);
			nodeQueue.push_back(currentNode->left);
			index++;
		}


	}

}

void Tree::initEnodeVec(TStream& tstream) {
	int kmax, ktem;
	kmax = ktem = 0;
	pair<long, long> etop;
	list<pair<long, long>> elist;
	deque<Query>::iterator qit = tstream.workLoadVec.begin();
	elist.push_back(pair<long, long>(0, tstream.maxn - 1));
	root = creatNewNode(0, tstream.maxn - 1, tstream.maxk);
	nodeQueue.push_back(root);

	while (qit != tstream.workLoadVec.end())
	{
		while (elist.size() < 3)
		{
			splitElist(tstream, elist);
			ktem = ktem / 2;
			etop = elist.front();
			while ((*qit).index < etop.first)
			{
				enodeVec.push_back(eNode(etop.first, etop.second, ktem));
				elist.pop_front();
				etop = elist.front();
				if (!nodeQueue.empty())
				{
					nodeQueue.front()->kmax = ktem;
					nodeQueue.front()->isleaf = 1;
					nodeQueue.pop_front();
				}

			}

		}

		kmax = qit->k;
		while (qit != tstream.workLoadVec.end() && ifInNode(*qit, etop))
		{
			if (qit->k >= kmax)
			{
				kmax = qit->k;
			}
			qit++;
		}

		enodeVec.push_back(eNode(etop.first, etop.second, kmax > ktem ? kmax : ktem));
		elist.pop_front();
		etop = elist.front();
		nodeQueue.front()->kmax = kmax > ktem ? kmax : ktem;
		nodeQueue.front()->isleaf = 1;
		nodeQueue.pop_front();
		ktem = kmax > ktem ? kmax : ktem;
		while (qit != tstream.workLoadVec.end() && qit->index < etop.first)
		{
			enodeVec.push_back(eNode(etop.first, etop.second, ktem));
			elist.pop_front();
			etop = elist.front();
			nodeQueue.front()->kmax = ktem;
			nodeQueue.front()->isleaf = 1;
			nodeQueue.pop_front();
		}

	}
	while (!nodeQueue.empty())
	{
		nodeQueue.front()->kmax = ktem;
		nodeQueue.front()->isleaf = 1;
		enodeVec.push_back(eNode(nodeQueue.front()->st, nodeQueue.front()->ed, ktem));
		nodeQueue.pop_front();
	}
	initTreeParm(tstream.mink);
}

void Tree::initEnode(TStream& ts) {
	bool isfirst = true;
	int knum = 0, klast = 0;
	Object obj;
	eNode en;
	map<double, Object>::iterator thit = candiMap.begin(); 
	for (vector<eNode>::reverse_iterator it = enodeVec.rbegin(); it != enodeVec.rend(); ++it)
	{
		en = *it;
		knum = 0;
		if (!isfirst)
		{
			int count = it->kmax;
			thit = candiMap.begin();
			if (candiMap.size() > count)
			{
				while (count)
				{
					thit++;
					count--;
				}
			}
			else
			{
				thit = candiMap.end();
				thit--;
			}
		}
		klast = en.kmax;		
		for (long i = en.st; i < en.ed + 1; i++)
		{
			obj = ts.GetDataById(i);
			if (knum < en.kmax)
			{
				if (isfirst || obj.value > thit->first)
				{
					if (candiMap.count(obj.value))
					{
						RandomNumber++;
						obj.value = obj.value + RandomNumber * 0.00000003;
					}

					candiMap.insert(thit, pair<double, Object>(obj.value, obj));
					if (!isfirst)thit--;
					knum++;
					if (knum == en.kmax)  
					{
						if (it == enodeVec.rbegin())
						{
							thit--;
							isfirst = false;
						}

					}
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (obj.value > (*thit).first)
				{
					if (candiMap.count(obj.value))
					{
						RandomNumber++;
						obj.value = obj.value + RandomNumber * 0.00000003;
					}
					candiMap.insert(thit, pair<double, Object>(obj.value, obj));
					if (obj.id > (*thit).second.id) 
					{
						candiMap.erase(thit--);
					}
					else							
					{
						thit--;
					}
				}
			}

		}
	}
}

void Tree::initTreeNode() {
	list<TreeNode*> nodelist;
	nodelist.push_back(root);
	auto nlit = nodelist.begin();
	for (auto it = candiMap.begin(); it != candiMap.end(); )
	{
		while (it->second.id < (*nlit)->st)
		{
			nlit--;
		}
		while (it->second.id > (*nlit)->ed)
		{
			nlit++;
		}
		if ((*nlit)->nodelist.empty() || (*nlit)->nodelist.size() < (*nlit)->kmax)
		{
			(*nlit)->nodelist.push_back(it->second);
			it++;
		}
		else
		{
			if ((*nlit)->left != nullptr)
			{
				nodelist.insert(nlit, (*nlit)->right);
				nodelist.insert(nlit, (*nlit)->left);
			}

			if (nlit == prev(nodelist.end()))
			{
				nodelist.pop_back();
				nlit = prev(nodelist.end());
			}
			else
			{
				nlit = nodelist.erase(nlit);
			}
		}
	}
}

void Tree::updateBinaryTree(Object Onew, long ksum)
{
	long st;
	TreeNode* lastnode = headNodeList.back();
	if (ksum == 1)
	{
		partHeap.sortAndUpdateHeap(lastnode);
		st = lastnode->ed + 1;
		TreeNode* tnode = creatNewNode(st, st + tstruct.partition - 1, tstruct.leaflen);
		tnode->isleaf = 1;
		partHeap.clear();
		partHeap.insert(Onew);
		headNodeList.push_back(tnode);
	}
	else if (ksum > 1 && ksum <= lastnode->kmax)
	{
		partHeap.insert(Onew);
	}
	else
	{
		if (Onew.value > partHeap.getMin().value)
		{
			partHeap.insert(Onew);
			partHeap.removeMin();
		}
	}
}



void Tree::mergeNodes(TStream& tstream, int psum) {
	int split, times, gapnodes;
	long gaplen, combst, combed;
	double theata_temp;
	TreeNode* leftn;
	TreeNode* rightn;
	gapnodes = 0;
	split = (*splitMap.rbegin()).first;
	times = countMergTimes(psum);

	for (int i = 0; i < times; i++)
	{
		leftn = headNodeList.back();
		headNodeList.pop_back();
		rightn = headNodeList.back();
		headNodeList.pop_back();
		TreeNode* newnode = creatNewNode(rightn->st, leftn->ed, tstream.maxk);
		mergeTwoNeighbor(leftn, rightn, newnode, tstream.mink);
		newnode->left = leftn;
		newnode->right = rightn;
		headNodeList.push_back(newnode);

		gapnodes = (gapnodes + splitMap.find(split)->second.first) / 2;
		gaplen = leftn->ed - rightn->st + 1;
		combst = newnode->st - gapnodes * gaplen;
		combed = newnode->ed - gapnodes * gaplen;
		split = split / 2;
		if (split > 1)
		{
			shrunkNodes(combst, combed, splitMap.find(split)->second.second);
		}

	}
}

void Tree::shrunkNodes(long combst, long combed, int kmax) {
	TreeNode* tnode;

	for (auto it = headNodeList.begin(); it != headNodeList.end(); it++) {
		if (combst >= (*it)->st && combed <= (*it)->ed)
		{
			tnode = *it;
			break;
		}
	}

	queue<TreeNode*> q;
	q.push(tnode);

	while (!q.empty()) {
		TreeNode* current = q.front();
		q.pop();
		if (combst == current->st && combed == current->ed)
		{
			if (!current->nodelist.empty())
			{
				if (current->nodelist.size() < kmax)
				{
					mergeTwoNeighbor(current->left, current->right, current, kmax - current->nodelist.size());
				}
				current->right = deleteNode(current->right);
				current->left = deleteNode(current->left);
				current->isleaf = 1;
				current->kmax = kmax;
			}
			break;
		}
		else if (combst >= current->st && combed <= current->ed)
		{
			if (current->right) {
				q.push(current->right);
			}
			if (current->left) {
				q.push(current->left);
			}

		}
	}

}


void Tree::countCandidate()
{
	long sum = 0;
	for (auto it = headNodeList.begin(); it != headNodeList.end(); it++)
	{
		queue<TreeNode*> q;
		q.push(*it);
		while (!q.empty()) {
			TreeNode* current = q.front();
			sum = sum + current->nodelist.size();
			q.pop(); 
			if (current->left != nullptr) {
				q.push(current->left);
			}
			if (current->right != nullptr) {
				q.push(current->right);
			}
		}
	}
	//cout << "candidate count： " << sum << endl;
}

void Tree::mergeTwoNeighbor(TreeNode* left, TreeNode* right, TreeNode* tnode, int mink)
{
	int tempk = mink;
	auto rpi = right->nodelist.begin();
	auto lpi = left->nodelist.begin();
	while (rpi != right->nodelist.end() && lpi != left->nodelist.end() && tempk)
	{
		if (rpi->value > lpi->value)
		{
			tnode->nodelist.push_back(*rpi);
			rpi = right->nodelist.erase(rpi);
		}
		else
		{
			tnode->nodelist.push_back(*lpi);
			lpi = left->nodelist.erase(lpi);
		}
		tempk--;
	}


	while (tempk && rpi != right->nodelist.end()) {
		tnode->nodelist.push_back(*rpi);
		rpi = right->nodelist.erase(rpi);
		tempk--;
	}

	while (tempk && lpi != left->nodelist.end()) {
		tnode->nodelist.push_back(*lpi);
		lpi = left->nodelist.erase(lpi);
		tempk--;
	}
}

void Tree::purgeExpired(long tag) {
	TreeNode* tnode = headNodeList.front();
	if (tnode->st >= tag)
	{
		return;
	}
	else if (tnode->st<tag && tnode->ed>tag)
	{
		queue<TreeNode*> q;
		q.push(tnode); 

		while (!q.empty()) {
			TreeNode* current = q.front();
			q.pop();

			if (current->ed <= tag)
			{
				list<Object> emptyList;
				current->nodelist.swap(emptyList);
				continue;
			}
			else if (current->st < tag)
			{
				current->nodelist.sort(compbyid);
				auto it = current->nodelist.begin();
				while (it != current->nodelist.end())
				{
					if (it->id < tag)
					{
						it = current->nodelist.erase(it);
					}
					else
					{
						break;
					}
				}
				current->nodelist.sort(compbyval);
				if (current->left != nullptr && current->left->st < tag && current->ed > tag) {
					q.push(current->left);
				}
				if (current->right != nullptr && current->right->st < tag && current->ed > tag) {
					q.push(current->right);
				}
			}


		}
	}

}


void Tree::updateDataFlow(TStream& tstream) {
	clock_t startTime, endTime;
	long psum, ksum, timestp;
	int qtime = 25;
	ksum = timestp = 0;
	psum = 0;
	tstream.SetDataStreamTag(tstream.GetDataStreamTag() + tstream.GetCommonPartition());
	headNodeList.push_back(root);
	for (long i = tstream.GetDataStreamBegin(); i < tstream.GetVecSize(); i++)
	{
		ksum++;
		timestp++;
		tstream.winTag++;
		tstream.winBegain++;
		updateBinaryTree(tstream.GetDataById(i), ksum);
		if (ksum == tstruct.partition)
		{
			psum++;
			ksum = 0;
			tstream.SetDataStreamBegin(tstream.GetDataStreamBegin() + tstruct.partition);
			tstream.SetDataStreamTag(tstream.GetDataStreamTag() + tstruct.partition);
			if (psum != 0 && psum % 2 == 0)
			{
				mergeNodes(tstream, psum);
			}
			purgeExpired(tstream.winTag);
			if (psum > 16 && qtime)
			{
				countCandidate();
				qtime--;
			}


		}

		if (psum == tstruct.leafnum)
		{
			psum = 0;
			headNodeList.pop_front();
		}

		if (timestp == 1024 && qtime)
		{
			startTime = clock();
			timestp = 0;
			outputResult(tstream);
			endTime = clock();
			cout <<  " query time：" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
			qtime--;
		}
	}
}

void Tree::outputResult(TStream& tstream) {
	int  qindex, curk, circle = tstream.GetQueryTimes();
	long bound;
	Query curq;
	ResultParm rp;
	while (circle--)
	{
		qindex = getRandom(0, tstream.GetWorkLoadVecSize() - 1);
		curq = tstream.GetWorkLoadVecById(qindex);
		bound = tstream.winBegain - curq.n;
		curk = curq.k;
		nodeHeap.clear();
		initNodeHeap(bound);
		while (curk-- && !nodeHeap.empty() && rp.value != 0)
		{
			rp = nodeHeap.getMax();
			if (rp.id < bound)
			{
				while (rp.id < bound)
				{
					rp.pos++;
					if (rp.pos == rp.node->nodelist.end())
					{
						if (updateMaxHeap(&rp, bound))
						{
							rp = nodeHeap.getMax();
						}
						else
						{
							break;
						}

					}
					else
					{
						rp.id = rp.pos->id;
						rp.value = rp.pos->value;
						nodeHeap.changeMax(rp);
					}
				}
			}
			else
			{
				rp.pos++;
				if (rp.pos == rp.node->nodelist.end())
				{
					if (!updateMaxHeap(&rp, bound))break;
				}
				else
				{
					rp.id = rp.pos->id;
					rp.value = rp.pos->value;
					nodeHeap.changeMax(rp);
				}
			}
		}
	}
}

void Tree::initNodeHeap(long ed) {
	ResultParm r;
	for (auto it = headNodeList.begin(); it != headNodeList.end(); it++)
	{
		if (it == prev(headNodeList.end()) && (*it)->right == nullptr)
		{
			(*it)->nodelist.sort(compbyval);
		}
		if ((*it)->ed > ed)
		{
			r.id = (*it)->nodelist.begin()->id;
			r.value = (*it)->nodelist.begin()->value;
			r.node = *it;
			r.pos = (*it)->nodelist.begin();
			nodeHeap.insert(r);
		}
	}
}

int Tree::updateMaxHeap(ResultParm* rp, long bound) {

	if ((*rp).node->right == nullptr)
	{
		nodeHeap.removeMax();
		return 0;
	}
	else
	{
		ResultParm r;
		if (!(*rp).node->right->nodelist.empty() && (*rp).node->right->ed > bound)
		{
			r.id = (*rp).node->right->nodelist.begin()->id;
			r.value = (*rp).node->right->nodelist.begin()->value;
			r.node = (*rp).node->right;
			r.pos = (*rp).node->right->nodelist.begin();
			nodeHeap.insert(r);
		}

		if (!(*rp).node->left->nodelist.empty() && (*rp).node->left->ed > bound)
		{
			r.id = (*rp).node->left->nodelist.begin()->id;
			r.value = (*rp).node->left->nodelist.begin()->value;
			r.node = (*rp).node->left;
			r.pos = (*rp).node->left->nodelist.begin();
			nodeHeap.insert(r);
		}
		nodeHeap.removeMax();
	}
	return 1;
}