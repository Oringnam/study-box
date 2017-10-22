#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

namespace mySort {

	// printing function
	void print(vector<int> v) {
		for (int a : v) {
			cout << a << " ";
		}
		cout << endl << endl;
	}

	// swap function
	void swap(int &a, int &b) {
		int tmp = a;
		a = b;
		b = tmp;
	}

	// time check function
	void checkTime(void (*func)(vector<int>), vector<int> v) {
		clock_t start, end;
		double times;

		start = clock();
		func(v);
		end = clock();
		times = (double)(end - start) / (CLOCKS_PER_SEC);
		cout << "실행 시간 : " << times << " second" << endl << endl;
	}


	/*
	*	standard - bubble
	*/
	inline void bubble(vector<int> v) {
		for (int i = 0; i < v.size(); ++i) {
			for (int j = 0; j < v.size() - i - 1; ++j) {
				if (v[j] > v[j + 1]) swap(v[j], v[j + 1]);
			}
		}

	//	print(v);
	}

	/*
	* priority queue
	* insertion, selection, heap sort
	*/
	// insertion sort
	inline void insertion(vector<int> v) {
		vector<int>::iterator itr;
		int save;	// value to check at now

		for (itr = v.begin()+1; itr != v.end(); ++itr) {
			save = *itr;
			vector<int>::iterator itr2 = itr;

			// compare values that i checked
			// if them bigger than save , push them
			// => process that find the position save
			while (itr2 != v.begin() && *(itr2-1) > save) {
				*itr2 = *(itr2 - 1);
				--itr2;
			}
			*itr2 = save;
		}

	//	print(v);		
	}

	// selection sort
	inline void selection(vector<int> v) {
		vector<int>::iterator minLoc;	// minimum value location
		vector<int>::iterator itr, itr2;
		int tmp;

		// if i got value number just 1, i dont need no more process
		for (itr = v.begin(); itr != v.end(); ++itr) {
			minLoc = itr;

			for (itr2 = itr + 1; itr2 != v.end(); ++itr2) {
				if (*itr2 < *minLoc) minLoc = itr2;
			}

			// swap
			swap(*itr, *minLoc);
		}

	//	print(v);
	}

	// heap sort
	void downHeap(vector<int> &v, int i, int n) {
		int pos = 2 * i + 1;
		vector<int>::iterator left, right, down;

		// check out of range
		if (pos >= n) return;

		// get left and right node
		left = v.begin() + pos;
		right = v.begin() + pos + 1;

		// to find the down pos
		if (pos+1 < n) {
			if (*left < *right) down = right;
			else down = left;
		}
		else
			down = left;
		
		// only now value is smaller than down value
		if (v[i] >= *down) return;
		
		// down
		swap(v[i], *down);
		// recur
		downHeap(v, (int)(down - v.begin()), n);		
	}

	void buildHeap(vector<int> &v) {
		int n = v.size();

		for (int i = (n - 2) / 2; i >= 0; i--) {
			downHeap(v, i, n);
		}
	}

	inline void heap(vector<int> v) {
		int n = v.size();

		// phase 1 Heap build
		buildHeap(v);

		// phase 2 Heap Sort
		while( n > 1 ) {
			swap(v[0], v[n-1]);
			--n;
			downHeap(v, 0, n-1);
		}
		
	//	print(v);
	}

	/*
	* Divide and Conquer
	* Merge, Quick Sort
	*/
	// Merge Sort
	void merging(vector<int> &v, int left, int size, int right) {
		vector<int> total;
		int i, j;

		i = left;	// L1 starting point
		j = size + 1;	// L2 starting point
		while (i <= size && j <= right) {
			if (v[i] > v[j]) {
				total.push_back(v[j++]);
			}
			else {
				total.push_back(v[i++]);
			}
		}

		// L2 is not empty
		if (i > size) {
			while (j <= right) {
				total.push_back(v[j++]);
			}
		}
		// L1 is not empty
		else {
			while (i <= size) {
				total.push_back(v[i++]);
			}
		}

		for (int x = left; x <= right; x++) {
			v[x] = total[x - left];
		}
	}

	void divide(vector<int> &v, int left, int right) {
		// recur base 
		if (left >= right) return;
		// divide point
		int size = (left + right) / 2;

		// recur
		divide(v, left, size);
		divide(v, size + 1, right);

		// combine
		merging(v, left, size, right);

	}

	inline void merge(vector<int> v) {
		divide(v, 0, v.size() - 1);

	//	print(v);
	}

	// Quck Sort
	int partition(vector<int> &v, int left, int right, int pivot) {
		int i = left;
		int j = right - 1;

		// while i is smaller than j
		while (i <= j) {
			// move i when left is smaller than pivot
			// move j when right is bigger than pivot
			// Until both are established, move and exchange
			while (i <= j && v[i] <= pivot) ++i;
			while (j >= i && v[j] >= pivot) --j;
			if (i < j) swap(v[i], v[j]);
		}
		// exchange pivot with i
		// --> pivot find the position
		swap(v[i], v[right]);

		return i;
	}

	void quickSort(vector<int> &v, int left, int right) {
		int Pi;

		// recur base
		if (left >= right) return;
		// get pivot point
		Pi = partition(v, left, right, v[right]);
		quickSort(v, left, Pi - 1);
		quickSort(v, Pi + 1, right);		
	}

	inline void quick(vector<int> v) {
		quickSort(v, 0, v.size()-1);

//		print(v);
	}
	
}	// namespace mySort
