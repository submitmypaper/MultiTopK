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
	int qtime = 5, timestp = 0;;
	for (; i < vecDataStream.size(); i++)
	{
		timestp++;
		if (i > dataStreamTag)
		{
			updateSuperTopk();
			dataStreamBegin = dataStreamBegin + commonSlide;
			dataStreamTag = dataStreamTag + commonSlide;
			preWinVec[getPreWinIndex(overall_indx)].cur_k = 0;
			overall_indx++;
			purgeExpiredWindow();
			if (qtime)
			{
				cout << "candidate count£º " << resultList.size() << endl;
				qtime--;
			}
			else
			{
				break;
			}
			//printAdjacency();
		}

		updateResultList(vecDataStream[i]);
		if (timestp == 1024 && qtime)
		{
			startTime = clock();
			timestp = 0;
			outputTopKResult();
			endTime = clock();
			cout <<  " query time £º" << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
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
	if (overall_indx == 60)
	{
		int a = 0;
	}
	int ktemp = slideList.size() - 1;
	long newleftwin;
	list<Object>::iterator sit = slideList.begin();
	list<MTop>::iterator rit = resultList.begin();
	MTop m;
	m.leftwin = overall_indx;
	m.rightwin = overall_indx;
	newleftwin = overall_indx;

	while (sit != slideList.end())
	{
		m.value = sit->value;
		m.id = sit->id;
		if (sit->value < rit->value)
		{
			resultList.insert(rit, m);
			sit++;
			ktemp--;
			continue;
		}
		else if (sit->value == rit->value)
		{
			rit->rightwin = overall_indx;
			sit++;
			ktemp--;
			continue;
		}

		while (rit != resultList.end() && sit->value > rit->value)
		{
			rit->leftwin = rit->leftwin + ktemp;
			newleftwin = rit->leftwin;
			if (rit->leftwin > rit->rightwin)
			{
				rit = resultList.erase(rit);
			}
			else
			{
				rit++;
			}
		}
		m.leftwin = newleftwin;
		rit = resultList.insert(rit, m);
		ktemp--;
		sit++;
	}
}


void Qmeta::purgeExpiredWindow() {
	int temp_k = preWinVec[getPreWinIndex(overall_indx)].adpk;
	list<MTop>::reverse_iterator iter = resultList.rbegin();
	while (temp_k && iter != resultList.rend())
	{
		iter->leftwin++;
		if (iter->leftwin > iter->rightwin)
		{
			iter = list<MTop>::reverse_iterator(resultList.erase((++iter).base()));
		}
		else
		{
			++iter;
		}
		temp_k--;
	}
	preWinVec[getPreWinIndex(overall_indx)].cur_k = 0;	
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
			else         
			{
				if (n)
				{
					for (int i = 1; i < length; i++)
					{
						continue;
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
	list<MTop>::iterator rit;
	for (i = 0; i < preWinVec.size(); i++)
	{
		MTop m;
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
		rit = resultList.begin();
		m.leftwin = i;	
		m.rightwin = i;
		if (i == 0)
		{
			for (j = k; j > 0; j--)
			{
				m.value = tempvec[j].value;
				m.id = tempvec[j].id;
				if (resultList.empty())
				{
					resultList.push_back(m);
				}
				else if (resultList.front().value < m.value)
				{
					resultList.push_back(m);
				}
			}
		}
		else
		{

			for (j = k; j > 0; j--)
			{
				m.value = tempvec[j].value;
				m.id = tempvec[j].id;
				if (m.value < rit->value)
				{
					rit = resultList.insert(rit, m);
					continue;
				}
				else if (m.value == rit->value)
				{
					rit->rightwin = i;
					continue;
				}
				while (m.value > rit->value && rit != resultList.end())
				{
					++rit;
					if (m.id == rit->id)
					{
						rit->rightwin = i;
					}
					else
					{
						m.leftwin = rit->leftwin;
					}
				}
				if (m.id != rit->id)
				{
					rit = resultList.insert(rit, m);
				}
			}
		}
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
	long sum = 1024 * 1024 * 50;
	if ((fp1 = fopen("stock_data100.txt", "r")) != NULL)
	{
		try
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
		catch (bad_alloc& all)
		{
			cerr << "bad_alloc exception occurred: " << all.what() << '\n';
		}
	}
	length = maxData;
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



