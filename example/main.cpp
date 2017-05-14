#include <iostream>
#include <sstream>
#include <time.h>
#include "../src/recommender.cpp"
#include <stdexcept>


using namespace std::string_literals;
using namespace std;

Recommender r;

vector<vector<double>> ratings = {
	{ 4, 0, 0, 5, 1, 0, 0 },
	{ 5, 5, 4, 0, 0, 0, 0 },
	{ 0, 0, 0, 2, 4, 5, 0 },
	{ 0, 3, 0, 0, 0, 0, 3 }
};

bool assertEqualDoubles(double a, double b) {
	if (a != b) {
		throw std::runtime_error("Doubles are not equal");
	}
}

void test() {
	double dotProduct = Utils::calculateDotProduct(ratings[0], ratings[1]);
	assertEqualDoubles(dotProduct, 20);
	cout << "Test 1 passed..." << endl;

	vector<double> zeroesA = { 0,0,0,0,0,0,0 };
	assertEqualDoubles(Utils::calculateDotProduct(zeroesA, zeroesA), 0);
	cout << "Test 2 passed..." << endl;

	double normA = Utils::normalizeVector(ratings[0]);
	double normB = Utils::normalizeVector(ratings[1]);
	assertEqualDoubles(normA, 42);
	assertEqualDoubles(normB, 66);
	cout << "Test 3 passed..." << endl;

	double cosineSimilarity = Utils::calculateCosineSimilarity(dotProduct, normA, normB);
	assertEqualDoubles(cosineSimilarity, 0.37986858819879310);
	cout << "Test 4 passed..." << endl;

	dotProduct = Utils::calculateDotProduct(ratings[0], ratings[2]);
	normB = Utils::normalizeVector(ratings[2]);
	cosineSimilarity = Utils::calculateCosineSimilarity(dotProduct, normA, normB);
	assertEqualDoubles(cosineSimilarity, 0.32203059435976528);
	cout << "Test 5 passed..." << endl;

	double rawMean = Utils::getRawMean(ratings[0]);
	assertEqualDoubles(rawMean, 3.3333333333333335);
	cout << "Test 6 passed..." << endl;

	rawMean = Utils::getRawMean(ratings[1]);
	assertEqualDoubles(rawMean, 4.6666666666666670);
	cout << "Test 7 passed..." << endl;

	Utils::subtractRawMeanFromVector(ratings[0]);
	assertEqualDoubles(ratings[0][0], 0.66666666666666652);
	cout << "Test 8 passed..." << endl;

	assertEqualDoubles(ratings[0][1], 0);
	cout << "Test 9 passed..." << endl;

	assertEqualDoubles(ratings[0][2], 0);
	cout << "Test 10 passed..." << endl;

	assertEqualDoubles(ratings[0][3], 1.6666666666666665);
	cout << "Test 11 passed..." << endl;

	assertEqualDoubles(ratings[0][4], -2.3333333333333335);
	cout << "Test 12 passed..." << endl;

	assertEqualDoubles(ratings[0][5], 0);
	cout << "Test 13 passed..." << endl;

	assertEqualDoubles(ratings[0][6], 0);
	cout << "Test 14 passed..." << endl;

	Utils::subtractRawMeanFromVector(ratings[1]);
	assertEqualDoubles(ratings[1][0], 0.33333333333333304);
	cout << "Test 15 passed..." << endl;

	assertEqualDoubles(ratings[1][1], 0.33333333333333304);
	cout << "Test 16 passed..." << endl;

	assertEqualDoubles(ratings[1][2], -0.66666666666666696);
	cout << "Test 17 passed..." << endl;

	assertEqualDoubles(ratings[1][3], 0);
	cout << "Test 18 passed..." << endl;

	assertEqualDoubles(ratings[1][4], 0);
	cout << "Test 19 passed..." << endl;

	assertEqualDoubles(ratings[1][5], 0);
	cout << "Test 20 passed..." << endl;

	assertEqualDoubles(ratings[1][6], 0);
	cout << "Test 21 passed..." << endl;

	dotProduct = Utils::calculateDotProduct(ratings[0], ratings[1]);
	normA = Utils::normalizeVector(ratings[0]);
	normB = Utils::normalizeVector(ratings[1]);
	cosineSimilarity = Utils::calculateCosineSimilarity(dotProduct, normA, normB);
	assertEqualDoubles(cosineSimilarity, 0.092450032704204751);
	cout << "Test 22 passed..." << endl;

	Utils::subtractRawMeanFromVector(ratings[2]);
	dotProduct = Utils::calculateDotProduct(ratings[0], ratings[2]);
	normB = Utils::normalizeVector(ratings[2]);
	cosineSimilarity = Utils::calculateCosineSimilarity(dotProduct, normA, normB);
	assertEqualDoubles(cosineSimilarity, -0.55908524625168976);
	cout << "Test 23 passed..." << endl;

	Utils::subtractRawMeanFromVector(ratings[3]);
	dotProduct = Utils::calculateDotProduct(ratings[0], ratings[3]);
	normB = Utils::normalizeVector(ratings[3]);
	cosineSimilarity = Utils::calculateCosineSimilarity(dotProduct, normA, normB);
	assertEqualDoubles(cosineSimilarity, 0);
	cout << "Test 24 passed..." << endl;

	ratings = {
		{ 4, 0, 0, 5, 1, 0, 0 },
		{ 5, 5, 4, 0, 0, 0, 0 },
		{ 4, 4, 0, 5, 1, 5, 0 },
		{ 0, 3, 0, 0, 0, 0, 3 }
	};
	double rating = r.getRatingPrediction(ratings, 0, 1);
	assertEqualDoubles(rating, 4.0935486257446074);
	cout << "Test 25 passed..." << endl;

	ratings = {
		{ 4, 0, 0, 5, 1, 0, 0 },
		{ 5, 5, 4, 0, 0, 0, 0 },
		{ 0, 0, 0, 2, 4, 5, 0 },
		{ 0, 3, 0, 0, 0, 0, 3 }
	};
	rating = r.getRatingPrediction(ratings, 0, 1);
	assertEqualDoubles(rating, 5);
	cout << "Test 26 passed..." << endl;

	ratings = {
		{ 4, 0, 0, 1, 1, 0, 0 },
		{ 5, 5, 4, 0, 0, 0, 0 },
		{ 0, 0, 0, 2, 4, 5, 0 },
		{ 0, 3, 0, 0, 0, 0, 3 }
	};
	rating = r.getRatingPrediction(ratings, 1, 3);
	assertEqualDoubles(rating, 1);
	cout << "Test 27 passed..." << endl;
}

int main() {
	clock_t tStart = clock();

	test();

	string documentPath = "C:\\workspace\\RecommenderCppProject\\RecommenderCppProject\\resources\\document.txt";
	string documentsPath = "C:\\workspace\\RecommenderCppProject\\RecommenderCppProject\\resources\\documents.txt";
	string query = "get current date time javascript";
	vector<string> documents = {
		{ "get the current date and time in javascript" },
		{ "get the current date and time in python" },
		{ "something very different" },
		{ "what is the time now" }
	};

	r.tfidf(query, documents, false);
	vector<double> similarities = r.recommend();

	cout << "TF-IDF Example:" << endl;
	cout << "Target: " << endl << query << endl;
	cout << endl;
	cout << "Results: " << endl;

	vector<string> res = r.getSortedDocuments(similarities);
	for (int i = 0; i < res.size(); i++) {
		cout << res[i] << endl;
	}
	cout << "Final test passed" << endl;
	cout << "Time elapsed: " << (double)(clock() - tStart) / CLOCKS_PER_SEC << endl;
	system("pause");
	return 0;
}