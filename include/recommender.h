#pragma once

#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#include <vector>
#include <string>
#include <map>

using namespace std;

class Recommender {
public:
	vector<string> rawDocuments;
	vector<string> document;
	vector<vector<string>> documents;
	map<string, double> weights;

	Recommender() {};

	map<string, double> tfidf(string documentFilePath, string documentsFilePat, bool useStopWords);
	map<string, double> tfidf(string query, vector<string> documents, bool useStopWords);
	vector<double> recommend(map<string, double> weights);
	vector<string> getSortedDocuments(vector<double> similarities);
	double getRatingPrediction(vector<vector<double>> &ratings, int rowIndex, int colIndex);
	double getGlobalBaselineRatingPrediction(vector<vector<double>> &ratings, int rowIndex, int colIndex);
	vector<pair<int, double>> getTopCFRecommendations(vector<vector<double>> &ratings, int rowIndex, int limit);
private:
	bool useStopWords;
	vector<vector<string>> vocabulary;

	vector<string> readDocument(string documentFilePath);
	vector<vector<string>> getVocabulary(string documentsFilePath);
	vector<string> splitLineToWords(const string &line);
	int getNumberOfTimesTermAppears(const string& term, vector<string> document) const;
	int getNumberOfDocumentsWithTerm(string& termt) const;
	double calculateTfIdf(int numberOfTimesTermAppears, int totalNumberOfTerms, string currentTerm) const;
	vector<pair<int, double>> getNeighbourhood(int index, int colIndex, vector<vector<double>> &ratings);
	vector<pair<int, double>> getSimilarities(vector<vector<double>> &ratings, double normA, int index, int colIndex);
};

#endif