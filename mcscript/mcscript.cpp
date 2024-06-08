#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <climits>
#include <cfloat>
#include <string>
#include <vector>
#include <algorithm>
#include <future>

#include "entity.h"
#include "reader.h"

const double sideAlignment = ((1 - 0.98F) / 2);

int* generateNextCombinations(int currentRatio[], const int startRatio[], const int finalRatio[], int index, int size)
{
	if (index == size)
	{
		// Reset the index to 0 and generate the next combination
		return generateNextCombinations(currentRatio, startRatio, finalRatio, 0, size);
	}
	else
	{
		if (currentRatio[index] < finalRatio[index])
		{
			// Increment the current value if it is less than the final value
			currentRatio[index]++;
			return currentRatio;
		}
		else
		{
			// Reset the current value to the start value and move to the next index
			currentRatio[index] = startRatio[index];
			return generateNextCombinations(currentRatio, startRatio, finalRatio, index + 1, size);
		}
	}
}

bool areEqual(const int* arr1, const int* arr2, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (arr1[i] != arr2[i])
		{
			return false;
		}
	}
	return true;
}

std::string printArray(int arr[], int size)
{
	std::string output;
	output.append("[");
	for (int i = 0; i < size - 1; i++)
	{
		output.append(std::to_string(arr[i]) + ", ");
	}
	output.append(std::to_string(arr[size - 1]));
	output.append("]");
	return output;
}

std::string printVector(std::vector<int> vec)
{
	std::string output;
	output.append("<");
	for (int i = 0; i < vec.size() - 1; i++)
	{
		output.append(std::to_string(vec[i]) + ", ");
	}
	output.append(std::to_string(vec[vec.size() - 1]));
	output.append(">");
	return output;
}

void copyArr(int outputArray[], int inputArr[], int size)
{
	for (int i = 0; i < size; i++)
	{
		outputArray[i] = inputArr[i];
	}
}

void formatConsole(){
	// Configuration of output results
	// Console text formatting, additional formatting available in entity.h
	std::cout << std::setprecision(STANDARD_PRECISION);
	std::cout << std::setfill(' ');
	std::cout << std::fixed;
}

void formatOuputFile(std::ofstream& outFile) {
	if (outFile.is_open()) {
		// output file text formatting
		outFile << std::setprecision(STANDARD_PRECISION);
		outFile << std::setfill(' ');
		outFile << std::fixed;
	}
	else {
		std::cerr << "Error opening file!" << std::endl;
	}
}

std::vector<double> generateZGains(std::vector<Tnt> pow, Tnt proj0) {
	std::vector<double> zGains(pow.size());
	Tnt proj1;
	for (int i = 0; i < pow.size(); i++) {
		proj1 = proj0;
		proj1.explosionFrom(pow[i], 0.9f);
		zGains[i] = proj1.getZ() - proj0.getZ();
	}
	return zGains;
}

std::vector<std::vector<int>> splitVector(int start, int end, int N) { //10
	std::vector<std::vector<int>> parameter(N); // Initialize with N empty vectors
	int counter = 0;
	for (int i = start; i <= end; i++) {
		parameter[counter].push_back(i);
		counter++;
		if (counter == N) {
			counter = 0;
		}
	}
	return parameter;
}

void runSimulaiton(int simulationId, std::vector<int> parameter, std::ofstream& outFile, std::mutex& coutMutex);

int main() {
	/*

		Re-writting code

	*/
	// takes care out the output file and console result file formatting
	formatConsole();

	// Creates an outputfile
	std::ofstream outFile("results.txt", std::ios::trunc); // Open the file in truncate mode (overrides)
	formatOuputFile(outFile);

	// Sets up multithreading support
	unsigned int numThreads = std::thread::hardware_concurrency() - 0; // Subtract to leave more threads free
	std::vector<std::thread> threads;
	std::vector<std::vector<int>> parameters(numThreads); // to assign each thread its own vector of parameters
	std::mutex printMutex;	// to have individual threads take turns accessing/writing to important data

	// Create the parameter
	parameters[0] = { 0 };

	// Loop through each thread
	for (unsigned int i = 0; i < numThreads; ++i) {
		// If there is something in the parameter vector for it, run the thread
		if (!parameters[i].empty()) {
			threads.push_back(std::thread(runSimulaiton, i, parameters[i], std::ref(outFile), std::ref(printMutex)));
			// Delays each thread by 10 ms only for visual appearance
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	for (auto& thread : threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}

	std::cout << "All simulations completed." << std::endl;
	outFile << "All simulations completed." << std::endl;
	outFile.close();
	return 0;
}

// Global variables, so all threads can read from it
double record = DBL_MAX;

void runSimulaiton(int simulationId, std::vector<int> parameter, std::ofstream& outFile, std::mutex& printMutex) {

	std::cout << "created << " << simulationId << std::endl;

	// setup of the test world
	World world;
	// create the blocks in the world
	Block yGuider(3359, -13, -16113);

	// Initialize all Entities
	Tnt power("3359.5 -18.0 -16113.5");
	Tnt proj("3384.5 -15.0 -16123.5");


	// Set their delays
	power.setAge(0);

	// Populate the world with blocks and Entities
	world.addBlocks(yGuider);
	world.addEntities({ &power, &proj }); // order is important

	// creates a backup of the world (for initial conditions)
	world.backup();

	//world.update();
	{
		std::lock_guard<std::mutex> lock(printMutex);
		std::cout << "World Blocks of " << simulationId << std::endl;
		world.printInformation();
	}

};