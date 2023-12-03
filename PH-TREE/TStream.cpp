#include "TStream.h"
#include "TStream.h"
#include <random>
#include <math.h>
#include <cmath>
#include<iostream>
TStream::TStream()
{
}

TStream::~TStream()
{
}

bool compareByN(Query& a, Query& b) {
	return a.n < b.n;
}
bool compareByNdes(Query& a, Query& b) {
	return a.n > b.n;
}

bool compareByS(Query& a, Query& b) {
	return a.s < b.s;
}

bool compareByK(const Query& a, const Query& b) {
	return a.k < b.k;
}

int generateRandomInteger(int a, int b) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distribution(a, b);
	return distribution(gen);
}

void initQindex(deque<Query>& workLoadVec, long maxn) {
	for (auto it = workLoadVec.begin(); it != workLoadVec.end(); it++)
	{
		it->index = maxn - it->n;
	}
}

long getPowOf2(long number)
{
	double exponent = log2(number);
	long closestPowerOf2;
	if (pow(2, exponent) == number)
	{
		return number;
	}
	else
	{
		closestPowerOf2 = round(pow(2, exponent + 1));
	}
	return closestPowerOf2;
}

long closestMultipleOfX(long n, long x) {
	int quotient = n / x;
	long lowerMultiple = x * quotient;
	long upperMultiple = x * (quotient + 1);

	if (quotient == 0)
	{
		return x;
	}
	if (abs(n - lowerMultiple) <= abs(n - upperMultiple)) {
		return lowerMultiple;
	}
	else {
		return upperMultiple;
	}
}




double getRandomPerc() {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> dis(1, 2);
	int choice = dis(gen);
	double number = 0.0;

	switch (choice) {
	case 1:
		number = 0.1;
		break;
	case 2:
		number = 0.01;
		break;
	case 3:
		number = 0.001;
		break;
	}

	return number;
}

long limit(long start, long end, int result) {
	if (result < start) {
		result = start;
	}
	else if (result > end) {
		result = end;
	}
	return result;
}

int generateRandomNormalInt(int start, int end, double mean, double stddev) {
	random_device rd;
	mt19937 gen(rd());
	normal_distribution<double> distribution(mean, stddev);
	double value = distribution(gen);
	int result = static_cast<int>(value);
	if (result < start) {
		result = start;
	}
	else if (result > end) {
		result = end;
	}
	return result;
}



void TStream::RandomNormalWorkLoad(int qnum, QParameter qparam) {
	Query q;
	long start = pow(2, qparam.n.first);
	long end = pow(2, qparam.n.second);
	double mean = (start + end) / 2.0;
	double stddev = (end - start) / 4;
	double value;
	long disresult;
	default_random_engine generator;
	normal_distribution<double> distribution(mean, stddev);
	normal_distribution<double> distribution2(qparam.k.first, qparam.k.second);
	try {
		for (int i = 0; i < qnum; i++)
		{
			value = distribution(generator);
			disresult = limit(start, end, static_cast<int>(value));
			q.n = disresult;
			value = distribution2(generator);
			disresult = limit(qparam.k.first - qparam.k.second, qparam.k.first + qparam.k.second, static_cast<int>(value));
			q.k = disresult;;
			q.s = 0;
			if (q.n > maxn)
			{
				maxn = q.n;
				maxnk = q.k;
			}
			if (q.k > maxk)
			{
				maxk = q.k;
			}
			workLoadVec.push_back(q);
		}
	}
	catch (std::bad_alloc& all) { std::cerr << "bad_alloc exception occurred: " << all.what() << '\n'; }


	auto result = minmax_element(workLoadVec.begin(), workLoadVec.end(), compareByN);
	minn = result.first->n;
	maxnk = result.second->k;
	result = minmax_element(workLoadVec.begin(), workLoadVec.end(), compareByS);
	mins = result.first->s;
	maxs = result.second->s;
	result = minmax_element(workLoadVec.begin(), workLoadVec.end(), compareByK);
	mink = result.first->k;
	sort(workLoadVec.begin(), workLoadVec.end(), compareByNdes);
	workLoadVec.begin()->n = getPowOf2(workLoadVec.begin()->n);
	maxn = workLoadVec.begin()->n;
	dataStreamBegin = winBegain = maxn - 1;
	winTag = 0;
	initQindex(workLoadVec, maxn);
}

int isPowerOfTwo(int n) {
	if (n < 1) {
		return 0;
	}
	else if ((n & n - 1) == 0) {
		return n;
	}
	else {				//如果不是2的次幂,向下取2的次幂
		int temp = 2;
		while (temp < n) {
			temp = temp * 2;
		}
		temp = temp / 2;
		return temp;
	}
}

vector<int> GetLeastNumbers_Solution(vector<int> input, int k)
{
	vector<int> ret;
	if (input.empty() || k > input.size())
		return ret;

	for (int i = 0; i < k; i++)
		ret.push_back(input[i]);
	make_heap(ret.begin(), ret.end(), less<int>());
	for (int i = k; i < input.size(); i++)
	{
		if (input[i] < ret[0])
		{
			pop_heap(ret.begin(), ret.end(), less<int>());
			ret.pop_back();
			ret.push_back(input[i]);
			push_heap(ret.begin(), ret.end(), less<int>());
		}
	}
	return ret;
}


void TStream::ReadDataFile()
{
	FILE* fp1;
	double dlNumber;
	double maxData = 0;
	double count = 0;
	Object obj;
	double top = 1024 * 1024 * 50;
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
				vecDataStreamQue.push_back(obj);
				count++;
				if (count > top)
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
	fclose(fp1);

}


int TStream::GetDataStreamLength()
{
	return vecDataStreamQue.size();
}

long TStream::GetVecSize()
{
	return vecDataStreamQue.size();
}

Object TStream::GetVecById(long index)
{
	return vecDataStreamQue[index];
}

Object TStream::GetDataById(long index)
{
	return vecDataStreamQue[index];
}

long  TStream::GetIndexByQWin(int n)
{
	return winTag - n;
}

double TStream::GetLength()
{
	return length;
}

int TStream::GetDataStreamBegin()
{
	return dataStreamBegin;
}

int TStream::GetDataStreamTag()
{
	return dataStreamTag;
}

void TStream::SetDataStreamBegin(int begin)
{
	dataStreamBegin = begin;
}

void TStream::SetDataStreamTag(int tag)
{
	dataStreamTag = tag;
}

void TStream::AddDataStreamBegin(int outflow)
{
	dataStreamBegin += outflow;
}

void TStream::AddDataStreamTag(int inflow)
{
	dataStreamTag += inflow;
}

int TStream::GetCommonPartition()
{
	return commonPartition;
}
int TStream::GetQueryTimes()
{
	return queryTimes;
}
void TStream::SetQueryTimes(int times)
{
	queryTimes = times;
}

int TStream::GetWorkLoadVecSize() {
	if (workLoadVec.empty())
	{
		return 0;
	}
	else
	{
		return workLoadVec.size();
	}
}

Query TStream::GetWorkLoadVecById(int id) {
	return workLoadVec[id];
}