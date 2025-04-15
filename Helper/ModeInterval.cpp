#include "pch.h"
#include "Helper.h"
#include <numeric>

using namespace cv;
using namespace std;

int OnMajority(vector<int>& vArray)
{
	int nCount = 1, nMajority = vArray[0], nPow[2] = { 1, -1 }; // pow(-1, vArray[i] != nMajority)

	for (int i = 1; i < vArray.size(); i++)
	{
		nCount += nPow[vArray[i] != nMajority];

		nMajority = (!!nCount ? nMajority : vArray[i]);

		nCount = (!!nCount ? nCount : 1);
	}
	return nMajority;
}

template<typename  T>
T Majority(std::vector<T>& vArray)
{
	int nMajority[2] = { 0, 0 };

	vector<int> vStep(vArray.size());

	double dTimer = (double)std::accumulate(vArray.begin(), vArray.end(), 0.0) / vArray.size(); // ��ʼ������

	while (true)
	{
		std::transform(vArray.begin(), vArray.end(), vStep.begin(), [&dTimer](double ele) { return /*round*/(ele / dTimer); });

		nMajority[0] = OnMajority(vStep);

		if (count_if(vStep.begin(), vStep.end(),
			[nMajority](int& ele) { return (nMajority[0] == ele); })
			< (vArray.size() * 0.5)) break;

		if (nMajority[0] < 0) break; // �����ݼ� | ����˥��

		dTimer *= 0.5;

		nMajority[1] = nMajority[0];
	}
	return (T)nMajority[1] * dTimer * 2.0;
}

template<typename  T>
vector<int> MostFrequency(vector<tuple<int, T>>& vArray)
{
	map<int, int> vFre1;

	for (auto& iArray : vArray)vFre1[get<0>(iArray)]++;

	vector<tuple<int, int>> vFre2(vFre1.size());

	transform(vFre1.begin(), vFre1.end(), vFre2.begin(), [](pair<const int, int>& v) { return make_tuple(v.first, v.second); });

	auto iMax = max_element(vFre2.begin(), vFre2.end(), [](tuple<int, int>& v1, tuple<int, int>& v2) { return get<1>(v1) < get<1>(v2); });

	vector<int> vMost;

	for (auto& iFre : vFre2)
	{
		if (get<1>(iFre) == get<1>(*iMax))
		{
			vMost.push_back(get<0>(iFre));
		}
	}
	return vMost;
}

template<typename  T>
int MostSizeInArray(vector<tuple<int, T>>& vArray, vector<int>& vMost)
{
	int nSize = 0;

	for (auto& iArray : vArray)
	{
		for (auto& iMost : vMost)
		{
			nSize += (get<0>(iArray) == iMost);
		}
	}
	return nSize;
}

bool MostIsInvalid(vector<int>& vMost)
{
	for (auto& iMost : vMost)
	{
		if (iMost < 0) return true;
	}
	return false;
}

template<typename  T>
void Mode(vector<int>& vMost, vector<tuple<int, T>>& vStep, vector<tuple<int, T>>& vMode)
{
	for (int i = 0; i < vStep.size(); ++i)
	{
		auto iMost = find(vMost.begin(), vMost.end(), get<0>(vStep[i]));

		get<0>(vMode[i]) = (iMost != vMost.end());

		get<1>(vMode[i]) = get<1>(vStep[i]);
	}
}

template<typename  T>
T Mode(vector<T>& vArray)
{
	int nThSize = ceil(0.5 * vArray.size());

	vector<tuple<int, T>> vTmp(vArray.size()), vEMost(vArray.size());

	double dMean = (*max_element(vArray.begin(), vArray.end()) - *min_element(vArray.begin(), vArray.end())); // ��ʼ������

	transform(vArray.begin(), vArray.end(), vTmp.begin(), [dMean](T& e) { return make_tuple(round((double)e / dMean), e); });

	do
	{
		vector<int> vMost = MostFrequency(vTmp);

		int nSize = MostSizeInArray(vTmp, vMost);

		if (nSize < nThSize) break;

		if (MostIsInvalid(vMost)) break; // �����ݼ� | ����˥��

		dMean *= 0.5;

		Mode(vMost, vTmp, vEMost);

		transform(vArray.begin(), vArray.end(), vTmp.begin(), [dMean](T& e) { return make_tuple(round((double)e / dMean), e); });
	} while (true);

	vArray.clear();

	/*vector<T> vCache; */

	for (auto& iMost : vEMost) if (get<0>(iMost)) vArray.push_back(get<1>(iMost));

	return std::accumulate(vArray.begin(), vArray.end(), 0.0) / vArray.size();
}

void Mode_tuple(std::vector<std::tuple<int, double>>& vArray, std::tuple<int, double>& vMode)
{
	int nThSize = ceil(0.5 * vArray.size());

	vector<tuple<int, tuple<int, double>>> vTmp(vArray.size()), vEMost(vArray.size());

	auto iMax = max_element(vArray.begin(), vArray.end(), [](tuple<int, double>& v1, tuple<int, double>& v2) { return get<0>(v1) < get<0>(v2); });

	auto iMin = min_element(vArray.begin(), vArray.end(), [](tuple<int, double>& v1, tuple<int, double>& v2) { return get<0>(v1) < get<0>(v2); });

	double dMean = get<0>(*iMax) - get<0>(*iMin); // ��ʼ������

	transform(vArray.begin(), vArray.end(), vTmp.begin(), [dMean](tuple<int, double>& e) { return make_tuple(round((double)get<0>(e) / dMean), e); });

	do
	{
		vector<int> vMost = MostFrequency(vTmp);

		int nSize = MostSizeInArray(vTmp, vMost);

		if (nSize < nThSize) break;

		if (MostIsInvalid(vMost)) break; // �����ݼ� | ����˥��

		dMean *= 0.5;

		Mode(vMost, vTmp, vEMost);

		transform(vArray.begin(), vArray.end(), vTmp.begin(), [dMean](tuple<int, double>& e) { return make_tuple(round((double)get<0>(e) / dMean), e); });
	} while (true);

	vArray.clear();

	//vector<tuple<int, double>> vCache;

	for (auto& iMost : vEMost) if (get<0>(iMost)) vArray.push_back(get<1>(iMost));

	vMode = accumulate(vArray.begin(), vArray.end(), tuple<int, double>(0, 0.0), [](tuple<int, double>& d, tuple<int, double>& v) { return make_tuple(get<0>(d) + get<0>(v), get<1>(d) + get<1>(v)); });

	vMode = make_tuple(round((double)get<0>(vMode) / vArray.size()), get<1>(vMode) / vArray.size());
}

int Mode_int(vector<int>& vArray)
{
	return Mode(vArray);
}

float Mode_float(vector<float>& vArray)
{
	return Mode(vArray);
}

double Mode_double(vector<double>& vArray)
{
	return Mode(vArray);
}

template<typename  T>
T ModeInterval(vector<T>& vArray)
{
	T iMajority = Majority<T>(vArray);

	for (auto iArray = vArray.begin(); iArray != vArray.end();)
	{
		if (0.9 * iMajority < (*iArray) && (*iArray) < iMajority * 1.1) { ++iArray; continue; }

		iArray = vArray.erase(iArray);
	}
	return iMajority;
}

int ModeInterval_int(std::vector<int>& vArray)
{
	return ModeInterval<int>(vArray);
}

float ModeInterval_float(std::vector<float>& vArray)
{
	return ModeInterval<float>(vArray);
}

double ModeInterval_double(std::vector<double>& vArray)
{
	return ModeInterval<double>(vArray);
}

int Majority_int(std::vector<int>& vArray)
{
	return Majority<int>(vArray);
}

float Majority_float(std::vector<float>& vArray)
{
	return Majority<float>(vArray);
}

double Majority_double(std::vector<double>& vArray)
{
	return Majority<double>(vArray);
}