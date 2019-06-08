#include <iostream>
#include <vector>
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

int main() {
    std::cout << "Cartasian product\n";

	// functional test
	std::vector<short int> a = { 1, 2 };
	std::vector<short int> b = { 3, 4, 5 };
	std::vector<short int> c = { 6 };
	std::vector<short int> d = { 7, 8 };

	std::vector<std::vector<short int>> arrays = { a, b, c, d };

	auto product = cartasian_product(arrays, 0, 10000);
	print_result(product);

	product = cartasian_product(arrays, 5, 10);
	print_result(product);


	// performance test
	std::vector<std::vector<short int>> table;
	short int v = 0;
	size_t length = 1;
	for (int i = 0; i < 10; i++) {
		std::vector<short int> row;
		int size = 4;
		length *= size;
		for (int j = 0; j < size; j++) {
			row.push_back(v++);
		}
		table.push_back(row);
	}

	clock_t begin = clock();
	product = cartasian_product(table, 0, length);
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "Generated " << product.size() << " entries in " << elapsed_secs << " seconds\n";
}
