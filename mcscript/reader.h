#pragma once
#ifndef READER_H
#define READER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <iomanip>

class Reader
{
    std::string path;

public:
    Reader(std::string path) : path(path) {};

    std::vector<std::vector<double>> getData()
    {
        std::vector<std::vector<double>> data;
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Error opening file\n";
            return data;
        }

        std::string lastLine, line;
        while (std::getline(file, line))
        {
            if (!line.empty())
            {
                lastLine = line;
            }
        }

        file.close();

        // std::cout << "Last line of the file: " << lastLine << std::endl;

        std::vector<std::string> words;
        std::stringstream ss(lastLine);
        //cout << "Input String: " << endl;
        //cout << ss.str() << endl;

        std::string trash, input;
        ss >> trash;
        ss >> trash;
        ss >> trash;
        ss >> trash;
        ss >> trash;
        std::vector<double> mixedData;
        while (!ss.eof())
        {
            ss >> input;
            mixedData.push_back(std::stod(input));
        }
        std::vector<double> dataLine(6); // ordered data
        double jumpToVel = mixedData.size() / 2;
        for (int i = 0; i < (mixedData.size() / 2); i += 3)
        {
            dataLine[0] = mixedData[i + 0];
            dataLine[1] = mixedData[i + 1];
            dataLine[2] = mixedData[i + 2];

            dataLine[3] = mixedData[i + 0 + jumpToVel];
            dataLine[4] = mixedData[i + 1 + jumpToVel];
            dataLine[5] = mixedData[i + 2 + jumpToVel];
            data.push_back(dataLine);
        }

        return data;
    }
    std::vector<std::vector<double>> getEntityList()
    {
        std::vector<std::vector<double>> data;
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Error opening file\n";
            return data;
        }

        std::string lastLine, line;
        while (std::getline(file, line))
        {
            if (!line.empty())
            {
                lastLine = line;
            }
        }

        file.close();

        // std::cout << "Last line of the file: " << lastLine << std::endl;

        std::vector<std::string> words;
        std::stringstream ss(lastLine);
        //cout << "Input String: " << endl;
        //cout << ss.str() << endl;
        std::string x, y, z, u, v, w;

        std::string trash, input;
        ss >> trash;
        ss >> trash;
        ss >> trash;
        ss >> trash;
        ss >> trash;
        std::vector<double> mixedData;
        while (!ss.eof())
        {
            ss >> input;
            mixedData.push_back(std::stod(input));
        }
        std::vector<double> dataLine(6); // ordered data
        double jumpToVel = mixedData.size() / 2;
        for (int i = 0; i < (mixedData.size() / 2); i += 3)
        {
            dataLine[0] = mixedData[i + 0];
            dataLine[1] = mixedData[i + 1];
            dataLine[2] = mixedData[i + 2];

            dataLine[3] = mixedData[i + 0 + jumpToVel];
            dataLine[4] = mixedData[i + 2 + jumpToVel];
            dataLine[5] = mixedData[i + 2 + jumpToVel];
            data.push_back(dataLine);
        }

        return data;
    }
};

#endif // READER_H