#include "mySort.h"

#define NUM		1000

using namespace mySort;

int main() {
	srand((unsigned int)time(nullptr));

	vector<int> sample;

	for (int i = 0; i < NUM; ++i) {
		int t = rand() % 100;
		sample.push_back(t);
	}

	std::cout << "<< Experiment start --- size : " << NUM << " >>" << endl;

	std::cout << "** bubble" << endl;
	checkTime(bubble, sample);

	std::cout << "** insertion" << endl;
	checkTime(insertion, sample);	

	std::cout << "** selection" << endl;
	checkTime(selection, sample);

	std::cout << "** heap" << endl;
	checkTime(heap, sample);

	std::cout << "** merge" << endl;
	checkTime(merge, sample);

	std::cout << "** quick" << endl;
	checkTime(quick, sample);

	return 0;
}