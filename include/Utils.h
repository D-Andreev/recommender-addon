#ifndef UTILS_H
#define UTILS_H

#pragma once
#include <vector>
#include "Utils.h"

using namespace std;

class Utils {
public:
	static double calculateDotProduct(const vector<double> &a, const vector<double> &b);
	static double normalizeVector(const vector<double> &a);
	static double calculateCosineSimilarity(const double &dotProduct, const double &normA, const double &normB);
	static void subtractRawMeanFromVector(vector<double> &a);
	static double getRawMean(const vector<double> &a);
};

#endif