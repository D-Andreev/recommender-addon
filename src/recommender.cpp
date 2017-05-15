#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <float.h>
#include <math.h>
#include "../include/Constants.h"
#include "../include/Utils.h";

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

	void tfidf(string query, vector<string> documents, bool useStopWords) {
		map<string, double> result;

		this->useStopWords = useStopWords;
		this->document = this->splitLineToWords(query);
		for (int i = 0; i < documents.size(); i++) {
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
	}

	vector<double> recommend() {
		vector<double> similarities;
		if (this->weights.size() == 0) return similarities;

		vector<double> queryVector;
		for (auto const &entry : this->weights) {
			queryVector.push_back(entry.second);
		}

		for (unsigned i = 0; i < this->documents.size(); i++) {
			vector<double> documentVector(queryVector.size());
			bool hasEqualTerms = false;
			for (unsigned j = 0; j < this->documents[i].size(); j++) {
				string currentTerm = this->documents[i][j];
				bool queryTermExistsInDocument = false;
				int foundIndex = 0;
				int idx = 0;
				for (auto const &entry : this->weights) {
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
				int totalNumberOfTerms = this->documents[i].size();
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

	vector<string> getSortedDocuments(vector<double> similarities) {
		vector<string> result;
		if (similarities.size() == 0) return result;

		vector<int> used;
		while (result.size() < similarities.size()) {
			double max = -DBL_MAX;
			int maxIndex = 0;
			for (unsigned i = 0; i < similarities.size(); i++) {
				bool isUsed = false;
				for (unsigned j = 0; j < used.size(); j++) {
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

	double getRatingPrediction(vector<vector<double>> &ratings, int rowIndex, int colIndex) {
		vector<vector<double>> originalRatings;
		for (unsigned i = 0; i < ratings.size(); i++) {
			originalRatings.push_back(ratings[i]);
		}

		vector<int> ids;
		double similaritiesSum = 0;
		double ratingsSum = 0;
		vector<double> neighbourhood = this->getNeighbourhood(rowIndex, colIndex, ratings, ids);
		if (!neighbourhood.size()) return 0;
		for (unsigned i = 0; i < neighbourhood.size(); i++) {
			similaritiesSum += neighbourhood[i];
			ratingsSum += originalRatings[ids[i]][colIndex] * neighbourhood[i];
		}

		double res = ratingsSum / similaritiesSum;
		return res;
	}

	double getGlobalBaselineRatingPrediction(vector<vector<double>> &ratings, int rowIndex, int colIndex) {
		double meanRating = Utils::getMean(ratings);
		double userMeanRating = Utils::getRowMean(ratings[rowIndex]);
		double itemMeanRating = Utils::getColMean(ratings, colIndex);

		return meanRating + (meanRating - itemMeanRating) + (meanRating - userMeanRating);
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
				const bool isStopWord = STOP_WORDS.find(word) != STOP_WORDS.end();
				if (isStopWord) continue;
			}
			document.push_back(word);
		}

		return document;
	}

	int getNumberOfTimesTermAppears(const string& term, vector<string> document) const {
		return std::count(document.begin(), document.end(), term);
	}

	int getNumberOfDocumentsWithTerm(string& term) const {
		int count = 0;
		for (unsigned i = 0; i < this->documents.size(); i++) {
			if (this->getNumberOfTimesTermAppears(term, this->documents[i]) >= 1) {
				count++;
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

	vector<double> getNeighbourhood(int index, int colIndex, vector<vector<double>> &ratings, vector<int> &ids) {
		vector<double> neighbourhood;
		Utils::subtractRawMeanFromVector(ratings[index]);
		double normA = Utils::normalizeVector(ratings[index]);
		neighbourhood = this->getSimilarities(ratings, normA, index, colIndex, ids);

		return neighbourhood;
	}

	vector<double> getSimilarities(vector<vector<double>> &ratings, double normA, int index, int colIndex, vector<int> &ids) {
		vector<double> similarities;
		for (unsigned i = 0; i < ratings.size(); i++) {
			if (i == index) continue;
			if (ratings[i][colIndex] == 0) continue;
			Utils::subtractRawMeanFromVector(ratings[i]);
			double dotProduct = Utils::calculateDotProduct(ratings[index], ratings[i]);
			double normB = Utils::normalizeVector(ratings[i]);
			double cosineSimilarity = Utils::calculateCosineSimilarity(dotProduct, normA, normB);
			if (cosineSimilarity < 0) continue;
			similarities.push_back(cosineSimilarity);
			ids.push_back(i);
		}

		return similarities;
	}
};