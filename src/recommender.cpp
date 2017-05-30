#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include "../include/recommender.h"
#include "../include/Constants.h"
#include "../include/Utils.h"

using namespace std;

map<string, double> Recommender::tfidf(string documentFilePath, string documentsFilePath, bool useStopWords) {
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
	return result;
}

map<string, double> Recommender::tfidf(string query, vector<string> documents, bool useStopWords) {
	map<string, double> result;

	this->useStopWords = useStopWords;
	this->document = this->splitLineToWords(query);
	int totalDocumentsSize = documents.size();
	for (int i = 0; i < totalDocumentsSize; i++) {
		this->rawDocuments.push_back(documents[i]);
		this->documents.push_back(splitLineToWords(documents[i]));
	}

	int totalNumberOfTerms = this->document.size();
	for (int i = 0; i < totalNumberOfTerms; i++) {
		string currentTerm = this->document[i];
		int numberOfTimesTermAppears = this->getNumberOfTimesTermAppears(currentTerm, this->document);
		double tfidf = this->calculateTfIdf(numberOfTimesTermAppears, totalNumberOfTerms, currentTerm);
		result[currentTerm] += tfidf;
	}

	this->weights = result;
	return result;
}

vector<double> Recommender::recommend(map<string, double> weights) {
	vector<double> similarities;
	if (weights.size() == 0) return similarities;

	vector<double> queryVector;
	for (auto const &entry : weights) {
		queryVector.push_back(entry.second);
	}

	int totalDocumentsSize = this->documents.size();
	for (int i = 0; i < totalDocumentsSize; i++) {
		vector<double> documentVector(queryVector.size());
		bool hasEqualTerms = false;
		int documentSize = this->documents[i].size();
		for (int j = 0; j < documentSize; j++) {
			string currentTerm = this->documents[i][j];
			bool queryTermExistsInDocument = false;
			int foundIndex = 0;
			int idx = 0;
			for (auto const &entry : weights) {
				if (entry.first == currentTerm) {
					queryTermExistsInDocument = true;
					foundIndex = idx;
					hasEqualTerms = true;
					break;
				}
				idx++;
			}
			if (!queryTermExistsInDocument) continue;

			int numberOfTimesTermAppears = this->getNumberOfTimesTermAppears(currentTerm, this->documents[i]);
			int totalNumberOfTerms = documentSize;
			double tfidf = this->calculateTfIdf(numberOfTimesTermAppears, totalNumberOfTerms, currentTerm);
			documentVector[foundIndex] = tfidf;
		}
		if (!hasEqualTerms) {
			similarities.push_back(0);
			continue;
		}
		double dotProduct = Utils::calculateDotProduct(queryVector, documentVector);
		double queryNormalized = Utils::normalizeVector(queryVector);
		double documentNormalized = Utils::normalizeVector(documentVector);
		double centeredCosineSimilarity = dotProduct / (sqrt(queryNormalized) * sqrt(documentNormalized));
		similarities.push_back(centeredCosineSimilarity);
	}

	return similarities;
}

vector<string> Recommender::getSortedDocuments(vector<double> similarities) {
	vector<string> result;
	int similaritiesSize = similarities.size();
	if (similaritiesSize == 0) return result;

	vector<int> used;
	while (result.size() < similaritiesSize) {
		double max = -DBL_MAX;
		int maxIndex = 0;
		for (int i = 0; i < similaritiesSize; i++) {
			bool isUsed = false;
			int usedSize = used.size();
			for (int j = 0; j < usedSize; j++) {
				if (used[j] == i) {
					isUsed = true;
					break;
				}
			}
			if (isUsed) continue;
			if (similarities[i] > max) {
				max = similarities[i];
				maxIndex = i;
			}
		}
		used.push_back(maxIndex);
		result.push_back(this->rawDocuments[maxIndex]);
	}

	return result;
}

double Recommender::getRatingPrediction(vector<vector<double>> &ratings, int rowIndex, int colIndex) {
	vector<vector<double>> originalRatings;
	int ratingsSize = ratings.size();
	for (int i = 0; i < ratingsSize; i++) {
		originalRatings.push_back(ratings[i]);
	}

	double similaritiesSum = 0;
	double ratingsSum = 0;
	vector<pair<int, double>> neighbourhood = this->getNeighbourhood(rowIndex, colIndex, ratings, originalRatings);
	int neighbourhoodSize = neighbourhood.size();
	if (!neighbourhoodSize) return 0;
	for (int i = 0; i < neighbourhoodSize; i++) {
		similaritiesSum += neighbourhood[i].second;
		ratingsSum += originalRatings[neighbourhood[i].first][colIndex] * neighbourhood[i].second;
	}

	double res = ratingsSum / similaritiesSum;
	return res;
}

double Recommender::getGlobalBaselineRatingPrediction(vector<vector<double>> &ratings, int rowIndex, int colIndex) {
	double meanRating = Utils::getMean(ratings);
	double userMeanRating = Utils::getRowMean(ratings[rowIndex]);
	double itemMeanRating = Utils::getColMean(ratings, colIndex);

	double result = fabs(meanRating + (itemMeanRating - meanRating) + (userMeanRating - meanRating));
	if (isnan(result)) return 0;
	return result;
}

vector<pair<int, double>> Recommender::getTopCFRecommendations(vector<vector<double>> &ratings, int rowIndex, int limit) {
	vector<pair<int, double>> recommendations;
	vector<vector<double>> originalRatings;
	int ratingsSize = ratings.size();
	for (int i = 0; i < ratingsSize; i++) {
		originalRatings.push_back(ratings[i]);
		if (i == rowIndex) {
			Utils::subtractRawMeanFromVector(ratings[rowIndex]);
		}
	}

	vector<pair<int, double>> neighbourhood = this->getNeighbourhood(rowIndex, ratings, originalRatings);
	int neighbourhoodSize = neighbourhood.size();
	if (!neighbourhoodSize) return recommendations;

	int userRowSize = ratings[rowIndex].size();
	for (int i = 0; i < userRowSize; i++) {
		if (ratings[rowIndex][i] != 0) continue;

		double similaritiesSum = 0;
		double ratingsSum = 0;
		for (int j = 0; j < neighbourhoodSize; j++) {
			similaritiesSum += neighbourhood[j].second;
			ratingsSum += originalRatings[neighbourhood[j].first][i] * neighbourhood[j].second;
		}

		double predictedRating = ratingsSum / similaritiesSum;
		if (!isnan(predictedRating)) recommendations.push_back(make_pair(i, predictedRating));
	}

	int recommendationsSize = recommendations.size();
	if (!recommendationsSize) return recommendations;

	struct compareRecommendations {
		inline bool operator() (const pair<int, double>& a, const pair<int, double>& b) {
			return (a.second > b.second);
		}
	};
	sort(recommendations.begin(), recommendations.end(), compareRecommendations());

	if (limit != -1 && recommendationsSize > limit) {
		recommendations.erase(recommendations.begin() + limit, recommendations.end());
	}

	return recommendations;
}

vector<string> Recommender::readDocument(string documentFilePath) {
	vector<string> result;

	ifstream file(documentFilePath);
	if (!file.good()) return result;
	string str;
	vector<string> document;
	getline(file, str);
	result = this->splitLineToWords(str);

	return result;
}

vector<vector<string>> Recommender::getVocabulary(string documentsFilePath) {
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

vector<string> Recommender::splitLineToWords(const string &line) {
	vector<string> document;
	stringstream s(line);
	string word;
	while (s >> word) {
		transform(word.begin(), word.end(), word.begin(), ::tolower);
		if (this->useStopWords) {
			const bool isStopWord = STOP_WORDS.find(word) != STOP_WORDS.end();
			if (isStopWord) continue;
		}
		document.push_back(word);
	}

	return document;
}

int Recommender::getNumberOfTimesTermAppears(const string& term, vector<string> document) const {
	return std::count(document.begin(), document.end(), term);
}

int Recommender::getNumberOfDocumentsWithTerm(string& term) const {
	int count = 0;
	int totalDocumentsSize = this->documents.size();
	for (int i = 0; i < totalDocumentsSize; i++) {
		if (this->getNumberOfTimesTermAppears(term, this->documents[i]) >= 1) {
			count++;
		}
	}

	return count;
}

double Recommender::calculateTfIdf(int numberOfTimesTermAppears, int totalNumberOfTerms, string currentTerm) const {
	double totalDocumentsSize = this->documents.size();
	double tf = numberOfTimesTermAppears / (double)totalNumberOfTerms;
	int numberOfDocumentsWithTerm = this->getNumberOfDocumentsWithTerm(currentTerm);
	double idf = log(totalDocumentsSize / (double)numberOfDocumentsWithTerm);
	idf++;
	double tfidf = tf * idf;

	return tfidf;
}

vector<pair<int, double>> Recommender::getNeighbourhood(int rowIndex, int colIndex, vector<vector<double>> &ratings, vector<vector<double>> originalRatings) {
	double normA = Utils::normalizeVector(ratings[rowIndex]);
	vector<pair<int, double>> similarities = this->getSimilarities(ratings, normA, rowIndex, colIndex, originalRatings);

	struct comparePairs {
		inline bool operator() (const pair<int, double>& a, const pair<int, double>& b) {
			return (a.second > b.second);
		}
	};

	sort(similarities.begin(), similarities.end(), comparePairs());

	return similarities;
}

vector<pair<int, double>> Recommender::getNeighbourhood(int rowIndex, vector<vector<double>> &ratings, vector<vector<double>> originalRatings) {
	double normA = Utils::normalizeVector(ratings[rowIndex]);
	vector<pair<int, double>> similarities = this->getSimilarities(ratings, normA, rowIndex, originalRatings);

	struct comparePairs {
		inline bool operator() (const pair<int, double>& a, const pair<int, double>& b) {
			return (a.second > b.second);
		}
	};

	sort(similarities.begin(), similarities.end(), comparePairs());

	return similarities;
}

vector<pair<int, double>> Recommender::getSimilarities(vector<vector<double>> &ratings, double normA, int rowIndex, int colIndex, vector<vector<double>> originalRatings) {
	vector<pair<int, double>> similarities;
	int ratingsSize = ratings.size();
	for (int i = 0; i < ratingsSize; i++) {
		if ((int)i == rowIndex) continue;
		if (originalRatings[i][colIndex] != 0) {
			double dotProduct = Utils::calculateDotProduct(originalRatings[rowIndex], originalRatings[i]);
			vector<double> subtractedRawMeanVector = Utils::getSubtractRawMeanFromVector(ratings[i]);
			double normB = Utils::normalizeVector(subtractedRawMeanVector);
			double cosineSimilarity = Utils::calculateCosineSimilarity(dotProduct, normA, normB);
			similarities.push_back(make_pair(i, cosineSimilarity));
		}
	}

	return similarities;
}

vector<pair<int, double>> Recommender::getSimilarities(vector<vector<double>> &ratings, double normA, int rowIndex, vector<vector<double>> originalRatings) {
	vector<pair<int, double>> similarities;
	int ratingsSize = ratings.size();
	for (int i = 0; i < ratingsSize; i++) {
		if ((int)i == rowIndex) continue;
		double dotProduct = Utils::calculateDotProduct(originalRatings[rowIndex], originalRatings[i]);
		vector<double> subtractedRawMeanVector = Utils::getSubtractRawMeanFromVector(ratings[i]);
		double normB = Utils::normalizeVector(subtractedRawMeanVector);
		double cosineSimilarity = Utils::calculateCosineSimilarity(dotProduct, normA, normB);
		similarities.push_back(make_pair(i, cosineSimilarity));
	}

	return similarities;
}