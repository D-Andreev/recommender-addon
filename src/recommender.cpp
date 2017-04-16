#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <float.h>
#include <math.h>
#include "../include/Constants.h"

using namespace std;

class Recommender {
public:
	vector<string> rawDocuments;
	vector<string> document;
	vector<vector<string>> documents;
	map<string, double> weights;

	Recommender() {}

	void tfidf(string documentFilePath, string documentsFilePath, bool useStopWords) {
		map<string, double> result;

		this->useStopWords = useStopWords;
		this->document = this->readDocument(documentFilePath);
		this->documents = this->getVocabulary(documentsFilePath);

		int totalNumberOfTerms = this->document.size();
		for (int i = 0; i < totalNumberOfTerms; i++) {
			string currentTerm = this->document[i];
			int numberOfTimesTermAppears = this->getNumberOfTimesTermAppears(currentTerm, this->document);
			double tfidf = this->calculateTfIdf(numberOfTimesTermAppears, totalNumberOfTerms, currentTerm);
			result[currentTerm] += tfidf;
		}

		this->weights = result;
	}

	vector<double> recommend() {
		vector<double> similarities;
		if (this->weights.size() == 0) similarities;
		vector<double> queryVector;
		for (auto const &entry : this->weights) {
			queryVector.push_back(entry.second);
		}

		for (int i = 0; i < this->documents.size(); i++) {
			vector<double> documentVector(queryVector.size());
			for (int j = 0; j < this->documents[i].size(); j++) {
				string currentTerm = this->documents[i][j];
				bool queryTermExistsInDocument = false;
				int foundIndex = 0;
				int idx = 0;
				for (auto const &entry : this->weights) {
					if (entry.first == currentTerm) {
						queryTermExistsInDocument = true;
						foundIndex = idx;
						break;
					}
					idx++;
				}
				if (!queryTermExistsInDocument) continue;

				int numberOfTimesTermAppears = this->getNumberOfTimesTermAppears(currentTerm, this->documents[i]);
				int totalNumberOfTerms = this->documents[i].size();
				double tfidf = this->calculateTfIdf(numberOfTimesTermAppears, totalNumberOfTerms, currentTerm);
				documentVector[foundIndex] = tfidf;
			}
			double dotProduct = this->calculateDotProduct(queryVector, documentVector);
			if (dotProduct == 0) {
				similarities.push_back(0);
				continue;
			}
			double queryNormalized = this->normalizeVector(queryVector);
			double documentNormalized = this->normalizeVector(documentVector);
			double cosineSimilarity = dotProduct / (queryNormalized * documentNormalized);
			similarities.push_back(cosineSimilarity);
		}

		return similarities;
	}

	vector<string> getSortedDocuments(vector<double> similarities) {
		vector<string> sorted;
		if (similarities.size() == 0) return sorted;
		vector<int> usedIndices;

		while (sorted.size() <= similarities.size() - 1) {
			double currentMax = -DBL_MAX;
			int currentMaxIndex = 0;
			for (int i = 0; i < similarities.size(); i++) {
				if (this->isUsed(usedIndices, i)) continue;
				if (similarities[i] >= currentMax) {
					currentMax = similarities[i];
					currentMaxIndex = i;
				}
			}

			sorted.push_back(this->rawDocuments[currentMaxIndex]);
			usedIndices.push_back(currentMaxIndex);
		}

		return sorted;
	}

	double getRatingPrediction(vector<vector<double>> ratings, int rowIndex, int colIndex) {
		vector<vector<double>> originalRatings;
		int maxNeighbours = ratings.size() * MAX_NEIGHBOURS_PERCENT;
		map<int, double> similarities;
		vector<int> usedIndices;
		double sumOfNeighbourSimilarities = 0, weightedRatingsSum = 0;

		for (int i = 0; i < ratings.size(); i++) {
			originalRatings.push_back(ratings[i]);
		}
		this->setCenterCosine(rowIndex, ratings[rowIndex]);
		double targetRowNormalized = this->normalizeVector(ratings[rowIndex]);
		this->getCosineSimilarities(ratings, rowIndex, targetRowNormalized, similarities);

		this->calculateWeightedAverage(usedIndices, maxNeighbours, similarities,
			originalRatings, colIndex, sumOfNeighbourSimilarities, weightedRatingsSum);

		return weightedRatingsSum / sumOfNeighbourSimilarities;
	}
private:
	bool useStopWords = false;

	vector<string> readDocument(string documentFilePath) {
		vector<string> result;

		ifstream file(documentFilePath);
		if (!file.good()) return result;
		string str;
		vector<string> document;
		getline(file, str);
		result = this->splitLineToWords(str);

		return result;
	}

	vector<vector<string>> getVocabulary(string documentsFilePath) {
		vector<vector<string>> result;

		ifstream file(documentsFilePath);
		if (!file.good()) return result;
		string str;
		while (getline(file, str)) {
			this->rawDocuments.push_back(str);
			vector<string> document;
			document = splitLineToWords(str);
			result.push_back(document);
		}

		return result;
	}

	vector<string> splitLineToWords(const string &line) {
		vector<string> document;
		stringstream s(line);
		string word;
		while (s >> word) {
			transform(word.begin(), word.end(), word.begin(), ::tolower);
			if (this->useStopWords) {
				bool isStopWord = false;
				for (int i = 0; i < STOP_WORDS.size(); i++) {
					if (word == STOP_WORDS[i]) {
						isStopWord = true;
						break;
					}
				}

				if (isStopWord) continue;
			}
			document.push_back(word);
		}

		return document;
	}

	int getNumberOfTimesTermAppears(const string& term, vector<string> document) const {
		int count = 0;
		for (int i = 0; i < document.size(); i++) {
			if (document[i] == term) count++;
		}

		return count;
	}

	int getNumberOfDocumentsWithTerm(string& term) const {
		int count = 0;
		for (int i = 0; i < this->documents.size(); i++) {
			for (int j = 0; j < this->documents[i].size(); j++) {
				if (this->documents[i][j] == term) {
					count++;
					break;
				}
			}
		}

		return count;
	}

	double calculateTfIdf(int numberOfTimesTermAppears, int totalNumberOfTerms, string currentTerm) const {
		double tf = numberOfTimesTermAppears / (double)totalNumberOfTerms;
		int numberOfDocumentsWithTerm = this->getNumberOfDocumentsWithTerm(currentTerm);
		double idf = log((double)this->documents.size() / (double)numberOfDocumentsWithTerm);
		idf++;
		double tfidf = tf * idf;

		return tfidf;
	}

	double calculateDotProduct(vector<double> query, vector<double> document) const {
		double sum = 0;
		for (int i = 0; i < document.size(); i++) {
			sum += query[i] * document[i];
		}

		return sum;
	}

	double normalizeVector(vector<double> queryVector) const {
		double normalized = 0;
		for (int i = 0; i < queryVector.size(); i++) {
			normalized += queryVector[i] * queryVector[i];
		}

		return sqrt(normalized);
	}

	vector<double> setCenterCosine(int index, vector<double>& ratings) {
		double averageMean = 0;
		int ratedItems = 0;
		double ratingsSum = 0;
		int totalItemsInRow = ratings.size();

		for (int j = 0; j < totalItemsInRow; j++) {
			if (ratings[j] == 0) continue;
			ratingsSum += ratings[j];
			ratedItems++;
		}

		averageMean = ratingsSum / ratedItems;
		for (int j = 0; j < totalItemsInRow; j++) {
			if (ratings[j] == 0) continue;
			ratings[j] -= averageMean;
		}

		return ratings;
	}

	void getCosineSimilarities(vector<vector<double>>& ratings, int rowIndex, double targetRowNormalized, map<int, double>& similarities) {
		for (int i = 0; i < ratings.size(); i++) {
			if (i == rowIndex) continue;
			this->setCenterCosine(i, ratings[i]);
			double dotProduct = this->calculateDotProduct(ratings[rowIndex], ratings[i]);
			if (dotProduct == 0) {
				similarities[i] = 0;
				continue;
			}
			double currentRowNormalized = this->normalizeVector(ratings[i]);
			double cosineSimilarity = dotProduct / (targetRowNormalized * currentRowNormalized);
			similarities[i] = cosineSimilarity;
		}
	}

	bool isUsed(vector<int> usedIndices, int index) const {
		bool isUsed = false;
		for (int k = 0; k < usedIndices.size(); k++) {
			if (usedIndices[k] == index) {
				isUsed = true;
				break;
			}
		}

		return isUsed;
	}

	void calculateWeightedAverage(vector<int>& usedIndices,
		int maxNeighbours,
		map<int, double> similarities,
		vector<vector<double>> originalRatings,
		int colIndex,
		double& sumOfNeighbourSimilarities,
		double& weightedRatingsSum) const {
		while (usedIndices.size() <= maxNeighbours) {
			double currentMaxRating = -DBL_MAX;
			int currentMaxRatingIndex = 0;
			bool maxRatingFound = false;
			for (auto const &s : similarities) {
				if (s.second == 0) continue;
				if (this->isUsed(usedIndices, s.first)) continue;
				if (s.second > currentMaxRating) {
					currentMaxRating = s.second;
					currentMaxRatingIndex = s.first;
					maxRatingFound = true;
				}
			}
			if (!maxRatingFound) break;
			usedIndices.push_back(currentMaxRatingIndex);
			sumOfNeighbourSimilarities += currentMaxRating;
			weightedRatingsSum += similarities[currentMaxRatingIndex] * originalRatings[currentMaxRatingIndex][colIndex];
		}
	}
};