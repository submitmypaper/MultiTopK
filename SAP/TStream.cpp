#include "TStream.h"


using namespace std;
#pragma warning(disable:4996)£»

void TStream::ReadDataSourceFile(int i)
{
	switch (i)
	{
		//0£ºstock_data100,1£ºtrip_data50,2£ºrandom_data50,3£ºnormal_data50
	case 0:fp1 = fopen("stock_data100.txt", "r"); break;
	case 1:fp1 = fopen("trip_data50.txt", "r"); break;
	case 2:fp1 = fopen("random_data50.txt", "r"); break;
	case 3:fp1 = fopen("normal_data50.txt", "r"); break;
	default:cout << "Sequence number error" << endl;
	}
	double data;
	int sum = 1024 * 1024 * 50;
	Object obj;
	obj.oid = 0;
	while (fscanf(fp1, "%lf", &data) != EOF)
	{
		if (data > maxtest)
			maxtest = data;
		obj.value = data;
		vecData.push_back(obj);
		obj.oid++;
		if (vecData.size() >= sum)
			break;
	}
	fclose(fp1);
}

bool cmp_value(const pair<int, double> left, const pair<int, double> right)
{
	return left.second < right.second;
}

void TStream::SetDataStreamBegin(int begin)
{
	dataStreamBegin = begin;
}

void TStream::SetDataStreamTag(int tag)
{
	dataStreamTag = tag;
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

void TStream::Init(int num)
{
	ReadDataSourceFile(num);
	commonsplit = 4;
}

bool val_des(const Object& b1, const Object& b2)
{
	return b1.value > b2.value;
}

bool time_des(const Object& b1, const Object& b2) {

	return b1.oid > b2.oid;
}

bool val_asc(const Object& left, const Object& right) {

	return left.value < right.value;
}

void TStream::RandomNormalWorkLoad(int qnum, QParameter qparam) {
	workLoadVec.clear();
	Query q;
	long start = pow(2, qparam.n.first);
	long end = pow(2, qparam.n.second);
	double mean = (start + end) / 2.0; 
	double stddev = (end - start) / 4; 
	double value;
	long disresult;
	commonsplit = qparam.slide.first;
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
			q.k = disresult;
			q.s = 0;
			workLoadVec.push_back(q);
		}
	}
	catch (std::bad_alloc& all) { std::cerr << "bad_alloc exception occurred: " << all.what() << '\n'; }
}

void TStream::initCandidate(Query& query) {
	resultList.clear();
	slideList.clear();
	int  count, ktemp;
	long  stindex;
	ktemp = query.k;
	query.n = (query.n / commonsplit) * commonsplit;
	conmmonslide = query.n / commonsplit;
	stindex = 0;
	count = 0;
	vector<Object> tempvec;
	list<Object>::iterator rit;
	for (int i = 0; i < 4; i++)
	{
		tempvec.resize(conmmonslide);
		copy(vecData.begin() + stindex, vecData.begin() + stindex + conmmonslide, tempvec.begin());
		nth_element(tempvec.begin(), tempvec.begin() + ktemp, tempvec.end(), val_des);
		sort(tempvec.begin(), tempvec.begin() + ktemp, val_des);
		count = ktemp - 1;
		rit = resultList.begin();
		for (int j = ktemp; j > 0; j--)
		{
			tempvec[j].don = count;
			if (i == 0)
			{
				resultList.push_back(tempvec[j]);
			}
			else
			{
				if (tempvec[j].value < rit->value)
				{
					rit = resultList.insert(rit, tempvec[j]);
					continue;
				}
				else if (tempvec[j].value == rit->value)
				{
					continue;
				}
				while (tempvec[j].value > rit->value && rit != resultList.end())
				{
					rit->don = rit->don + tempvec[j].don;
					if (rit->don > ktemp)
					{
						rit = resultList.erase(rit);
					}
					else
					{
						++rit;
					}
				}
				if (tempvec[j].oid != rit->oid)
				{
					rit = resultList.insert(rit, tempvec[j]);
				}
			}
			count--;
		}
		stindex = stindex + conmmonslide;
		tempvec = vector<Object>();
	}

}




void TStream::updateDataFlow() {
	int i, qtime = 10, timestp;
	clock_t startTime, endTime;
	long overall_indx, winTag;
	for (auto it = workLoadVec.begin(); it != workLoadVec.end(); it++)
	{
		startTime = clock();
		initCandidate(*it);
		endTime = clock();
		if (qtime == 1)
		{
			cout << "  Init time = " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
		}
		vector<Object> pmvec;
		dataStreamBegin = it->n;			
		dataStreamTag = 0;
		i = dataStreamBegin + 1;
		winTag = 0;
		timestp = 0;
		overall_indx = 0;
		for (; i < vecData.size(); i++)
		{
			timestp++;
			winTag++;
			if (i > dataStreamTag)
			{
				updateResult();
				dataStreamBegin = dataStreamBegin + conmmonslide;
				dataStreamTag = dataStreamTag + conmmonslide;
				if (dataStreamBegin >= vecData.size())
				{
					break;
				}
				updateExpire(winTag);
				slideList.clear();
				overall_indx++;

			}
			updateResultList(vecData[i], it->k);
			if (timestp == 1024 && qtime)
			{
				startTime = clock();
				timestp = 0;
				outputTopKResult(winTag, it->k);
				endTime = clock();
				cout << " query time£º" << (double)(endTime - startTime) / CLOCKS_PER_SEC * workLoadVec.size() << "s" << endl;
				qtime--;
			}
		}
	}

}

void TStream::updateResult()
{
	if (slideList.empty())
	{
		return;
	}
	int ktemp = slideList.size() - 1;
	long newleftwin;
	list<Object>::iterator sit = slideList.begin();
	list<Object>::iterator rit = resultList.begin();
	Object obj;
	while (sit != slideList.end())
	{
		obj.value = sit->value;
		obj.oid = sit->oid;
		obj.don = ktemp;
		if (sit->value < rit->value)
		{
			resultList.insert(rit, obj);
			sit++;
			ktemp--;
			continue;
		}
		else if (sit->value == rit->value)
		{
			rit->oid = sit->oid;
			sit++;
			ktemp--;
			continue;
		}

		while (rit != resultList.end() && sit->value > rit->value)
		{
			rit->don = rit->don + sit->don;
			if (rit->don > slideList.size())
			{
				rit = resultList.erase(rit);
			}
			else
			{
				rit++;
			}
		}
		if (rit->value == sit->value)
		{
			rit->oid = sit->oid;
		}
		else
		{
			rit = resultList.insert(rit, obj);
		}
		ktemp--;
		sit++;
	}
}

void TStream::updateResultList(Object onew, int k) {

	if (slideList.empty())
	{
		slideList.push_back(onew);
	}
	else
	{
		if (onew.value > slideList.front().value)
		{
			list<Object>::iterator it = lower_bound(slideList.begin(), slideList.end(), onew, val_asc);
			slideList.insert(it, onew);
			if (slideList.size() > k)
			{
				slideList.pop_front();
			}
		}
	}
}


void TStream::updateExpire(long index)
{
	if (slideList.empty())
	{
		return;
	}
	list<Object>::reverse_iterator iter = resultList.rbegin();
	while (iter != resultList.rend())
	{
		if (iter->oid < index)
		{
			iter = list<Object>::reverse_iterator(resultList.erase((++iter).base()));
		}
		else
		{
			++iter;
		}
	}
}


void TStream::outputTopKResult(long index, int k) {
	int  curk, circle = 10000;
	long bound;
	Query curq;
	while (circle--)
	{
		curk = k;
		auto iter = resultList.rbegin();
		while (iter != resultList.rend() && curk)
		{
			if (iter->oid >= index)
			{
				curk--;
			}
		}
	}
}




long TStream::GetDataStreamBegin()
{
	return dataStreamBegin;
}

long TStream::GetDataStreamTag()
{
	return dataStreamTag;
}