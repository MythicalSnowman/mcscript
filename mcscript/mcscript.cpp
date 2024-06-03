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

#define boosters 8
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
string printArray(int arr[], int size)
{
	string output;
	output.append("[");
	for (int i = 0; i < size - 1; i++)
	{
		output.append(to_string(arr[i]) + ", ");
	}
	output.append(to_string(arr[size - 1]));
	output.append("]");
	return output;
}
string printVector(vector<int> vec)
{
	string output;
	output.append("<");
	for (int i = 0; i < vec.size() - 1; i++)
	{
		output.append(to_string(vec[i]) + ", ");
	}
	output.append(to_string(vec[vec.size()-1]));
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

double dist(Tnt a, Tnt b, bool TF)
{
	double dx = a.locX - b.locX;
	double dy = a.locY - b.locY;
	double dz = a.locZ - b.locZ;
	if (TF) {
		cout << "dx: " << dx << " dy: " << dy << " dz: " << dz;
		cout << "\t d: " << sqrt(dx * dx + dy * dy + dz * dz) << endl;
	}

	return sqrt(dx * dx + dy * dy + dz * dz);
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

vector<double> generateZGains(vector<Tnt> pow, Tnt proj0) {
	vector<double> zGains(pow.size());
	Tnt proj1;
	for (int i = 0; i < pow.size(); i++) {
		proj1 = proj0;
		proj1.explosionFrom(pow[i], 0.9f);
		zGains[i] = proj1.motZ - proj0.motZ;
	}
	return zGains;
}

double record = 0;
std::mutex coutMutex;
void runSimulaiton(int simulationId, vector<int> parameter, ofstream* outFile) {

	Tnt proj0("-70435.49000000954 159.0 87853.49000000954");
	Tnt proj1;
	Tnt hammer0("-70447.5 159.0199999809265 87873.50999999046");
	Tnt hammer1;

	Tnt a0("-70427.67750000954 159.0 87853.50999999046");
	Tnt a1("-70429.24000000954 159.0 87853.50999999046");
	Tnt a2("-70431.49000000954 159.0 87853.50999999046");
	Tnt a3("-70433.49000000954 159.0 87853.50999999046");
	Tnt a4("-70435.50999999046 159.0 87851.49000000954");
	Tnt a5("-70435.50999999046 159.0 87849.49000000954");
	Tnt a6("-70435.50999999046 159.0 87847.24000000954");
	Tnt a7("-70435.50999999046 159.0 87845.67750000954");
	vector<Tnt> tnts;
	tnts.push_back(a0);
	tnts.push_back(a1);
	tnts.push_back(a2);
	tnts.push_back(a3);
	tnts.push_back(a4);
	tnts.push_back(a5);
	tnts.push_back(a6);
	tnts.push_back(a7);

	// Generates a vector with explosure as elements
	vector<double> zGains = generateZGains(tnts, proj0);
	/*
	for (int i = 0; i < zGains.size(); i++) {
		cout << "zGain: " << i << ": " << zGains[i] << endl;
	}*/
	// Display the simulation parameters
	{
		std::lock_guard<std::mutex> lock(coutMutex);
		cout << "Running Simulation: " << simulationId << "\tDoing: " << printVector(parameter) << endl;
		*outFile << "Running Simulation: " << simulationId << "\tDoing: " << printVector(parameter) << endl;
	}
	// Initialize variables used to judge distances
	int counter = 1;
	double zLoc;
	double score;
	int max = 23; // max amount of boosters, (some are limited to this number
	for (int parameterIndex = 0; parameterIndex < parameter.size(); parameterIndex++) {
		const int startRatio[] = { 0, 0, 0, 0, 0, 0, 0, parameter[parameterIndex] }; // Initial values
		const int finalRatio[] = { 34, max, max, max, max, max, max, parameter[parameterIndex] }; // Final values //can increase last to 34
		int currentRatio[] = { -1, 0, 0, 0, 0, 0, 0, parameter[parameterIndex] }; // Starting values
		while (!areEqual(currentRatio, finalRatio, boosters))
		{
			generateNextCombinations(currentRatio, startRatio, finalRatio, 0, boosters);
			//printRatio(currentRatio, boosters);
				//reset projectile start

			//input ratio into booster amounts & explode
			zLoc =
				currentRatio[0] * zGains[0] +
				currentRatio[1] * zGains[1] +
				currentRatio[2] * zGains[2] +
				currentRatio[3] * zGains[3] +
				currentRatio[4] * zGains[4] +
				currentRatio[5] * zGains[5] +
				currentRatio[6] * zGains[6] +
				currentRatio[7] * zGains[7] +
				87853.49000000954;

			score = sqrt((159.0 - hammer0.locY) * (159.0 - hammer0.locY) + (zLoc - hammer0.locZ) * (zLoc - hammer0.locZ)); // dx loc is the same

			{
				std::lock_guard<std::mutex> lock(coutMutex);
				if (score < 8.0 && score > record && zLoc < hammer0.locZ) {

					cout << "Simulation Id " << simulationId << " found dist: " << score << endl;
					cout << " location: " << zLoc << endl;
					cout << " ratio: " << printArray(currentRatio, boosters) << endl;

					*outFile << "Simulation Id " << simulationId << " found dist: " << score << endl;
					*outFile << " location: " << zLoc << endl;
					*outFile << "\tratio: " << printArray(currentRatio, boosters) << endl;
					record = score;

				}
				if (counter % 500000000 == 0) {
					cout << printArray(currentRatio, boosters) << endl;
					cout << " Waiting, SimulationID " << simulationId << " is at: " << printArray(currentRatio, boosters) << endl;
					*outFile << " Waiting, SimulationID " << simulationId << " is at: " << printArray(currentRatio, boosters) << endl;

				}
			}
			counter++;
		}
		{
			std::lock_guard<std::mutex> lock(coutMutex);
			cout << "Simulation Parameter: " << parameter[parameterIndex] << " is done" << endl;
			*outFile << "Simulation Parameter: " << parameter[parameterIndex] << " is done" << endl;
		}
	}
	{
		std::lock_guard<std::mutex> lock(coutMutex);
		cout << "Ending Simulation: " << simulationId << endl;
		*outFile << "Ending Simulation: " << simulationId << endl;
	}

};

int main()
{
	/*
		Code to place tnt 7.9999999 blocks away
	*/
	// sets up output format, additional formatting available in entity.h
	cout << setprecision(STANDARD_PRECISION); // cout << fixed;
	cout << setfill(' ');
	cout << fixed;

	// Obtains the amount of threads code will run on
	unsigned int numThreads = std::thread::hardware_concurrency();
	std::cout << "total threads I can use: " << numThreads << endl;
	std::vector<std::thread> threads;
	std::vector<vector<int>> parameters(numThreads);

	// sets up output format, for the console and output file
	cout << setprecision(STANDARD_PRECISION); // cout << fixed;
	cout << setfill(' ');
	cout << fixed;
	ofstream outFile;
	outFile.open("results.txt");
	outFile << setprecision(STANDARD_PRECISION); // cout << fixed;
	outFile << setfill(' ');
	outFile << fixed;

	// Simulation Parameters
	vector<int> simulaitonSweep = { 0, 34 };
	// Every thread gets assigned which parameter it will calculate
	parameters = splitVector(simulaitonSweep[0], simulaitonSweep[1], numThreads);
	for (unsigned int i = 0; i < numThreads; ++i) {
		threads.push_back(std::thread(runSimulaiton, i, parameters[i], &outFile));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	for (auto& thread : threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}

	std::cout << "All simulations completed." << std::endl;
	outFile << "All simulations completed." << std::endl;
	while (true);
	return 0;

}