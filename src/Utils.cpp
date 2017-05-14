#include <vector>
#include <math.h>
#include <map>
#include "../include/Utils.h"

using namespace std;

double Utils::calculateDotProduct(const vector<double> &a, const vector<double> &b) {
	if (a.size() != b.size()) {
		throw std::invalid_argument("Vectors are not equal lengths");
	}

	double sum = 0;
	for (unsigned i = 0; i < a.size(); i++) {
		sum += a[i] * b[i];
	}

	return sum;
}

double Utils::normalizeVector(const vector<double> &a) {
	double normalized = 0;
	for (unsigned i = 0; i < a.size(); i++) {
		normalized += a[i] * a[i];
	}

	return normalized;
}

double Utils::calculateCosineSimilarity(const double &dotProduct, const double &normA, const double &normB) {
	if (dotProduct == 0 || normA == 0 || normB == 0) return 0;
	return dotProduct / (sqrt(normA) * sqrt(normB));
}

double Utils::getRawMean(const vector<double> &a) {
	double sum = 0;
	double nonZeros = 0;
	for (unsigned i = 0; i < a.size(); i++) {
		if (a[i] == 0) continue;
		sum += a[i];
		nonZeros++;
	}

	return sum / nonZeros;
}

void Utils::subtractRawMeanFromVector(vector<double> &a) {
	double rawMean = Utils::getRawMean(a);
	for (unsigned i = 0; i < a.size(); i++) {
		if (a[i] == 0) continue;
		a[i] -= rawMean;
	}
}