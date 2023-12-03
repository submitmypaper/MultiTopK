#include "Qmeta.h"
#include "stdlib.h"
#include "time.h"
#include <math.h>
#include <algorithm>


bool descendingOrder(Object a, Object b) {
	return a.value > b.value;
}

bool ascOrder(Object a, Object b) {
	return a.value < b.value;
}

bool Qmeta::isAllk() {
	if (preWinVec[getPreWinIndex(overall_indx)].cur_k < preWinVec[getPreWinIndex(overall_indx)].adpk)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int getRandomNumber(int a, int b)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dis(a, b);
	return dis(gen);
}

void Qmeta::generateMutiQuery(int qnum, QParameter qparam) {
	if (qnum)
	{
		Query q;
		int slide, n, winsize, k;

		slide = getRandomNumber(qparam.slide.first, qparam.slide.second);
		slide = pow(2, slide);
		commonSlide = slide;
		for (int i = 0; i < qnum; i++)
		{
			n = rand() % (qparam.n.second - qparam.n.first + 1) + qparam.n.first;
			winsize = pow(2, n) * slide;
			if (winsize > maxWinSize) {
				maxWinSize = winsize;
			}
			k = rand() % (qparam.k.second - qparam.k.first) + qparam.k.first;
			q.k = k;
			q.slide = slide;
			q.winsize = winsize;
			mutiQuery.push_back(q);
		}
	}
	combineMutiWin_ComSlide();

}

void Qmeta::RandomNormalWorkLoad(int qnum, QParameter qparam) {
	Query q;
	int start = pow(2, qparam.n.first);
	int end = pow(2, qparam.n.second);
	double mean = (start + end) / 2.0; 
	double stddev = (end - start) / 4; 
	int slide = start / 16;
	default_random_engine generator;
	normal_distribution<double> distribution(mean, stddev);
	try {
		for (int i = 0; i < qnum; i++)
		{
			double value = distribution(generator);
			int result = static_cast<int>(value);
			if (result < start) {
				result = start;
			}
			else if (result > end) {
				result = end;
			}
			q.winsize = result;
			q.k = q.winsize / 1000;
			q.slide = slide;
			mutiQuery.push_back(q);
		}
	}
	catch (std::bad_alloc& all) { std::cerr << "bad_alloc exception occurred: " << all.what() << '\n'; }
	combineMutiWin_ComSlide();
}

void Qmeta::updateDataFlow() {
	clock_t startTime, endTime;
	int i = dataStreamTag + 1;
	int qtime = 10, timestp = 0;;
	for (; i < vecDataStream.size(); i++)
	{
		timestp++;
		if (i > dataStreamTag)
		{
			updateSuperTopk1();
			dataStreamBegin = dataStreamBegin + commonSlide;
			dataStreamTag = dataStreamTag + commonSlide;
			preWinVec[getPreWinIndex(overall_indx)].cur_k = 0;
			overall_indx++;
			purgeExpiredWindow();
			if (qtime)
			{
				counCandidate();
				qtime--;
			}
			else
			{
				break;
			}
		}
		updateResultList(vecDataStream[i]);
		if (timestp == 1024 && qtime)
		{
			startTime = clock();
			timestp = 0;
			outputTopKResult();
			endTime = clock();
			cout << " query time£º" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
			qtime--;
		}
	}
}

void Qmeta::updateResultList(Object onew) {
	if (preWinVec[getPreWinIndex(overall_indx)].cur_k < preWinVec[getPreWinIndex(overall_indx)].adpk)
	{
		if (preWinVec[getPreWinIndex(overall_indx)].cur_k == 0)
		{
			slideList.clear();
		}
		slideList.push_back(onew);
		preWinVec[getPreWinIndex(overall_indx)].cur_k++;
		if (preWinVec[getPreWinIndex(overall_indx)].cur_k == preWinVec[getPreWinIndex(overall_indx)].adpk)
		{
			slideList.sort(ascOrder);
		}
	}
	else
	{
		if (onew.value > slideList.front().value)
		{
			list<Object>::iterator it = lower_bound(slideList.begin(), slideList.end(), onew, ascOrder);
			slideList.insert(it, onew);
			slideList.pop_front();
		}
	}

}


void Qmeta::updateSuperTopk() {
	if (slideList.empty())
	{
		return;
	}
	list<Object>::iterator sit;
	list<Object>::iterator rrit;
	list<list<Object>>::reverse_iterator rit = resultList.rbegin();
	while (rit != resultList.rend())
	{
		sit = slideList.begin();
		if (slideList.end()->value > rit->begin()->value)
		{
			rrit = rit->begin();
			while (sit != slideList.end())
			{
				if (rrit == rit->end() || sit->value < rrit->value)
				{
					if (rrit == rit->end() || sit->value != rrit->value)
					{
						rit->insert(rrit, *sit);
						rit->pop_front();
						rrit = rit->begin();
					}
					++sit;
				}
				else
				{
					++rrit;
				}
			}
		}
		else
		{
			break;
		}
		++rit;
	}
	resultList.push_back(slideList);
}

void Qmeta::updateSuperTopk1() {
	if (slideList.empty())
	{
		return;
	}
	list<Object>::reverse_iterator sit;
	list<Object>::reverse_iterator rrit;
	list<list<Object>>::reverse_iterator rit = resultList.rbegin();
	while (rit != resultList.rend())
	{
		sit = slideList.rbegin();
		if (sit->value > rit->begin()->value)
		{
			rrit = rit->rbegin();
			while (rrit != rit->rend() && sit != slideList.rend())
			{
				while (rrit != rit->rend() && sit->value < rrit->value)
				{
					rrit++;
				}
				if (rrit == rit->rend())break;
				if (sit->value != rrit->value)
				{
					rit->insert(rrit.base(), *sit);
					rit->pop_front();
					++sit;
				}
				else
				{
					++sit;
				}
			}
		}
		else
		{
			break;
		}
		++rit;
	}
	resultList.push_back(slideList);
	slideList.clear();
}


void Qmeta::purgeExpiredWindow() {
	if (slideList.empty())
	{
		return;
	}
	resultList.pop_front();
}

void Qmeta::outputTopKResult() {
	int  qindex, curk, circle = 10000;
	long bound;
	Query curq;
	while (circle--)
	{
		qindex = getRandomNumber(0, mutiQuery.size() - 1);
		curq = mutiQuery[qindex];
		curk = curq.k;
		for (auto iter = resultList.rbegin(); iter != resultList.rend(); iter++) {
			curk--;
			if (curk == 0)
			{
				break;
			}
		}
	}
}

int Qmeta::getPreWinIndex(long index) {
	if (index >= preWinVec.size())
	{
		return index % preWinVec.size();
	}
	else
	{
		return index;
	}

}

void Qmeta::combineMutiWin_ComSlide() {				
	int stdex, n, index, tempslide;
	for (int j = 0; j < mutiQuery.size(); j++)
	{
		stdex = maxWinSize - mutiQuery[j].winsize;	
		pair<int, int>	p(j, 1);		
		if (winIndMap.count(stdex))
		{
			winIndMap[stdex].push_back(p);
		}
		else
		{
			vector<pair<int, int>> veca;
			veca.push_back(p);
			winIndMap.insert(pair<int, vector<pair<int, int>>>(stdex, veca));
		}
		n = mutiQuery[j].winsize / mutiQuery[j].slide;
		if (n) {	
			p.second = 0;
			for (int i = 1; i < n; i++)
			{
				index = maxWinSize - i * mutiQuery[j].slide;
				if (winIndMap.count(index))
				{
					winIndMap[index].push_back(p);
				}
				else
				{
					vector<pair<int, int>> veca;
					veca.push_back(p);
					winIndMap.insert(pair<int, vector<pair<int, int>>>(index, veca));
				}


			}
		}
	}
	integrateMutiWin();
}


void Qmeta::combineMutiWin() {
	int stdex, n, u, index, tempslide;
	for (int j = 0; j < mutiQuery.size(); j++)
	{
		map<int, int> temp;
		stdex = maxWinSize - mutiQuery[j].winsize;
		n = maxWinSize / mutiQuery[j].winsize;
		u = maxWinSize % mutiQuery[j].winsize;
		temp.insert(pair<int, int>(j, j));
		if (!stdex) {	
			if (u) {		
				if (n) {		
					int slide_last = stdex / mutiQuery[j].slide;	
					for (int i = 1; i <= slide_last; i++)
					{
						index = stdex - i * mutiQuery[j].slide;
					}
				}
			}
		}
	}
}


void Qmeta::integrateMutiWin() {	
	map<int, vector<pair<int, int>>>::iterator it;
	Win win;
	int j = 0;
	for (it = winIndMap.begin(); it != winIndMap.end(); it++) {
		int k = 0;

		win.index = it->first;
		win.slide = commonSlide;
		win.cur_k = 0;
		for (int i = 0; i < it->second.size(); i++)
		{
			int temp = it->second[i].first;
			if (mutiQuery[temp].k > k)
			{
				k = mutiQuery[temp].k;
			}
		}
		win.adpk = k;
		preWinVec.push_back(win);
		j++;
	}
	overall_indx = preWinVec.size() - 1;
}

void Qmeta::initMTopList() {	
	SetDataStreamTag(maxWinSize - 1);
	int startind, presize;
	int i, j, k, count;
	vector<Object> tempvec;
	list<Object> klist;
	for (i = 0; i < preWinVec.size(); i++)
	{
		startind = preWinVec[i].index;
		presize = maxWinSize - preWinVec[i].index;
		tempvec.resize(presize);
		copy(vecDataStream.begin() + startind, vecDataStream.begin() + maxWinSize, tempvec.begin());
		if (tempvec.size() >= preWinVec[i].adpk)
		{
			count = preWinVec[i].adpk;
			nth_element(tempvec.begin(), tempvec.begin() + count, tempvec.end(), descendingOrder);
			sort(tempvec.begin(), tempvec.begin() + preWinVec[i].adpk, descendingOrder);
			k = preWinVec[i].adpk;
		}
		else
		{
			k = tempvec.size();
		}
		preWinVec[i].cur_k = 0;
		for (j = k; j > 0; j--)
		{
			klist.push_back(tempvec[j]);
		}
		resultList.push_back(klist);
		klist.clear();
		tempvec = vector<Object>();
	}
}

bool deccomp(Object i, Object j) {
	return (i.value > j.value);
}

Qmeta::Qmeta()
{
}

Qmeta::~Qmeta()
{
}

void Qmeta::ReadDataFile()
{
	FILE* fp1;
	double dlNumber;
	double maxData = 0;
	long count = 0;
	Object obj;
	long sum = 1024 * 1024 * 25;
	//stock_data100£¬trip_data50£¬random_data50£¬normal_data50
	if ((fp1 = fopen("normal_data50.txt", "r")) != NULL)
	{
		while (fscanf(fp1, "%lf", &dlNumber) != EOF)
		{
			if (maxData < dlNumber)
				maxData = dlNumber;
			obj.id = count;
			obj.value = dlNumber;
			vecDataStream.push_back(obj);
			count++;
			if (count > sum)
			{
				break;
			}
		}
	}
	else
	{
		cout << "error data read" << endl;
	}
	length = maxData;
}

void Qmeta::counCandidate()
{
	long sum = 0;
	for (auto it = resultList.begin(); it != resultList.end(); it++)
	{
		if (!it->empty())
		{
			sum = sum + it->size();
		}
	}
	cout << "candidate count£º  " << sum << "   " << endl;
}

Object Qmeta::GetDataStream(long intObjectNumber)
{
	return vecDataStream[intObjectNumber];
}

long Qmeta::GetDataStreamLength()
{
	return vecDataStream.size();
}


double Qmeta::GetLength()
{
	return length;
}

long Qmeta::GetDataStreamBegin()
{
	return dataStreamBegin;
}

long Qmeta::GetDataStreamTag()
{
	return dataStreamTag;
}

void Qmeta::SetDataStreamBegin(long begin)
{
	dataStreamBegin = begin;
}

void Qmeta::SetDataStreamTag(long tag)
{
	dataStreamTag = tag;
}

void Qmeta::AddDataStreamBegin(long outflow)
{
	dataStreamBegin += outflow;
}

void Qmeta::AddDataStreamTag(long inflow)
{
	dataStreamTag += inflow;
}



