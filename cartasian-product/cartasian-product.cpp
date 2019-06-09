#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <thread>

std::vector<short int> int2indices(std::vector<std::vector<short int>> arrays, size_t value) {
	std::vector<short int> indices;
	for (int i = 0; i < arrays.size(); i++) {
		indices.push_back(0);
	}
	// base of nth position is the size of the nth array
	// [1,2] [3,4,5] [6,7] -> bases: 2, 3, 2
	// value = 5 -> indices = [0, 2, 1] -> value: 157
	// 136 137 146 147 156 >157< 5th position
	// 236 237 246 247 256 257
	for (int i = (int)arrays.size() - 1; i >= 0; i--) {
		auto arr = arrays[i];
		indices[i] = (short int)(value % arr.size());
		value /= arr.size();
		if (value == 0) {
			break;
		}
	}
	return indices;
}

std::vector<std::vector<short int>> cartasian_product(std::vector<std::vector<short int>> arrays, size_t start, size_t stop) {
	std::vector<std::vector<short int>> product;
	std::vector<short int> indices = int2indices(arrays, start);
	size_t step = start;
	bool isComplete = false;
	while (!isComplete && step < stop) {
		std::vector<short int> row;
		int i = 0;
		for (auto arr : arrays) {
			row.push_back(arr[indices[i]]);
			if (i == indices.size()-1) {
				// increment the last index only
				// the other indices are incremented on "overflow
				while (i >= 0) {
					indices[i]++;
					arr = arrays[i];
					if (indices[i] < arr.size()) {
						break;
					} else {
						// carry the overflow to the previous index
						indices[i] = 0;
						i--;
					}
				}
				if (i < 0) {
					isComplete = true;
				}
				break;
			}
			i++;
		}
		product.push_back(row);
		step++;
	}
	return product;
}

void print_result(std::vector<std::vector<short int>> table) {
	int i = 0;
	for (auto row : table) {
		std::cout << i << ". ";
		for (short int id : row) {
			std::cout << id << " ";
		}
		std::cout << "\n";
		i++;
	}

}

std::vector<std::vector<short int>> createTestData(int arrayCount, int entryCount, int& length) {
	std::vector<std::vector<short int>> table;
	short int v = 0;
	length = 1;
	for (int i = 0; i < arrayCount; i++) {
		std::vector<short int> row;
		length *= entryCount;
		for (int j = 0; j < entryCount; j++) {
			row.push_back(v++);
		}
		table.push_back(row);
	}
	return table;
}


void functionalTest() {
	int length;
	auto arrays = createTestData(3, 3, length);

	// test to generate all entries
	auto product = cartasian_product(arrays, 0, 10000);
	print_result(product);

	// test to generate entries in the range [5, 10]
	product = cartasian_product(arrays, 5, 10);
	print_result(product);
}

void performanceTestSingleThread() {
	int length;
	auto arrays = createTestData(12, 4, length);

	clock_t begin = clock();
	auto product = cartasian_product(arrays, 0, length);
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Generated " << product.size() << " entries in " << elapsed_secs << " seconds\n";
}

void performanceTestMultiThread() {
	int length;
	auto arrays = createTestData(12, 4, length);

	int threadCount = std::thread::hardware_concurrency();
	int entriesPerThread = length / threadCount;

	std::cout << "Starting " << threadCount << " threads\n";

	clock_t beginTime = clock();
	int start = 0, end = entriesPerThread;
	// running threads
	std::thread* threads = new std::thread[threadCount];
	// container of results
	std::vector<std::vector<short int>>* results = new std::vector<std::vector<short int>>[threadCount];

	for (int ti = 0; ti < threadCount; ti++) {
		// "capture" variables by reference except ti, start and end, they are copied (pass by value)
		threads[ti] = std::thread([&, ti, start, end]()->void { results[ti] = cartasian_product(arrays, start, end); });
		start = end;
		end += entriesPerThread;
	}

	// wait for threads
	size_t sum = 0;
	for (int ti = 0; ti < threadCount; ti++) {
		threads[ti].join();
		sum += results[ti].size();
	}

	clock_t endTime = clock();
	double elapsed_secs = double(endTime - beginTime) / CLOCKS_PER_SEC;
	
	std::cout << "Generated " << sum << " entries in " << elapsed_secs << " seconds\n";
}

int main() {
    std::cout << "Cartasian product\n";

	functionalTest();

	performanceTestSingleThread();

	performanceTestMultiThread();
}
