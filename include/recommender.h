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

	void tfidf(string documentFilePath, string documentsFilePat, bool useStopWords);
	void tfidf(string query, vector<string> documents, bool useStopWords);
	vector<double> recommend();
	vector<string> getSortedDocuments(vector<double> similarities);
	double getRatingPrediction(vector<vector<double>> ratings, int rowIndex, int colIndex);
private:
	string readDocument(string documentFilePath);
	vector<vector<string>> vocabulary;
	vector<vector<string>> getVocabulary(string documentsFilePath);
	vector<string> splitLineToWords(const string &line);
	int getNumberOfTimesTermAppears(const string& term, vector<string> document) const;
	int getNumberOfDocumentsWithTerm(string& termt) const;
	double calculateTfIdf(int numberOfTimesTermAppears, int totalNumberOfTerms, string currentTerm) const;
	double calculateDotProduct(vector<double> query, vector<double> document) const;
	double normalizeVector(vector<double> queryVector) const;
	vector<double> setCenterCosine(int index, vector<double>& ratings);
	void getCosineSimilarities(vector<vector<double>>& ratings, int rowIndex, double targetRowNormalized, map<int, double>& similarities);
	bool isUsed(vector<int> used, int index) const;
	bool useStopWords;
};

#endif