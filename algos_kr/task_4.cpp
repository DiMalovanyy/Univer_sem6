#include <iostream>
#include <queue>


int main() {

	std::priority_queue<int, std::vector<int>, std::greater<int>> nums;
	int num;
	std::cout << "Enter set ended by 0" << std::endl;
	std::cin >> num;
	nums.push(num);
	
	while(num != 0) {
		std::cin >> num;
		nums.push(num);
	}

	if (nums.size() == 1) {
		std::cout << "Min element: " << nums.top() << std::endl;
		return 0;
	}

	int min = nums.top();
	nums.pop();
	int second_min = nums.top();

	if (min == second_min) {
		std::cout << "There is no smallest element in set" << std::endl;
	} else {
		std::cout << "Smallest element: " << min << std::endl;
	}


	return 0;
}
