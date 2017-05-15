#include <vector>
#include <math.h>
#include <map>
#include "../include/Utils.h"

using namespace std;

double Utils::calculateDotProduct(const vector<double> &a, const vector<double> &b) {
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

double Utils::getMean(const vector<vector<double>> &ratings) {
	double sum = 0;
	double counter = 0;

	for (unsigned i = 0; i < ratings.size(); i++) {
		for (unsigned j = 0; j < ratings[i].size(); j++) {
			if (ratings[i][j] == 0) continue;
			sum += ratings[i][j];
			counter++;
		}
	}

	return sum / counter;
}

double Utils::getRowMean(const vector<double> &userRatings) {
	double sum = 0;
	double counter = 0;

	for (unsigned i = 0; i < userRatings.size(); i++) {
		if (userRatings[i] == 0) continue;
		sum += userRatings[i];
		counter++;
	}

	return sum / counter;
}

double Utils::getColMean(const vector<vector<double>> &ratings, int colIndex) {
	double sum = 0;
	double counter = 0;

	for (unsigned i = 0; i < ratings.size(); i++) {
		if (ratings[i][colIndex] == 0) continue;
		sum += ratings[i][colIndex];
		counter++;
	}

	return sum / counter;
}