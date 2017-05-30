#include <vector>
#include <math.h>
#include <map>
#include "../include/Utils.h"

using namespace std;

double Utils::calculateDotProduct(const vector<double> &a, const vector<double> &b) {
	double sum = 0;
	int vectorSize = a.size();
	for (int i = 0; i < vectorSize; i++) {
		sum += a[i] * b[i];
	}

	return sum;
}

double Utils::normalizeVector(const vector<double> &a) {
	double normalized = 0;
	int vectorSize = a.size();
	for (int i = 0; i < vectorSize; i++) {
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
	int vectorSize = a.size();
	for (int i = 0; i < vectorSize; i++) {
		if (a[i] == 0) continue;
		sum += a[i];
		nonZeros++;
	}

	return sum / nonZeros;
}

void Utils::subtractRawMeanFromVector(vector<double> &a) {
	double rawMean = Utils::getRawMean(a);
	int vectorSize = a.size();
	for (int i = 0; i < vectorSize; i++) {
		if (a[i] == 0) continue;
		a[i] -= rawMean;
	}
}

vector<double> Utils::getSubtractRawMeanFromVector(vector<double> &a) {
	vector<double> result;
	double rawMean = Utils::getRawMean(a);
	int vectorSize = a.size();
	for (int i = 0; i < vectorSize; i++) {
		result.push_back(a[i]);
		if (a[i] == 0) continue;
		result[i] -= rawMean;
	}

	return result;
}

double Utils::getMean(const vector<vector<double>> &ratings) {
	double sum = 0;
	double counter = 0;

	int ratingsSize = ratings.size();
	for (int i = 0; i < ratingsSize; i++) {
		int currentRowSize = ratings[i].size();
		for (int j = 0; j < currentRowSize; j++) {
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
	int userRatingsSize = userRatings.size();
	for (int i = 0; i < userRatingsSize; i++) {
		if (userRatings[i] == 0) continue;
		sum += userRatings[i];
		counter++;
	}

	return sum / counter;
}

double Utils::getColMean(const vector<vector<double>> &ratings, int colIndex) {
	double sum = 0;
	double counter = 0;
	int ratingsSize = ratings.size();
	for (int i = 0; i < ratingsSize; i++) {
		if (ratings[i][colIndex] == 0) continue;
		sum += ratings[i][colIndex];
		counter++;
	}

	return sum / counter;
}