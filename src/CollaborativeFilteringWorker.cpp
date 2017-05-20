#include "nan.h"
#include "../include/recommender.h"

using namespace std;
using namespace Nan;
using namespace v8;

class CollaborativeFilteringWorker : public AsyncWorker {
public:
	CollaborativeFilteringWorker(Callback * callback, vector<vector<double>> ratings, int rowIndex, int colIndex):
		AsyncWorker(callback),
		ratings(ratings),
		rowIndex(rowIndex),
		colIndex(colIndex) {}

	void Execute() {
		Recommender recommender;
		this->ratingPrediction = recommender.getRatingPrediction(this->ratings, this->rowIndex, this->colIndex);
	}

	void HandleOKCallback() {
		Local<Number> result = Nan::New(this->ratingPrediction);
		Local<Value> argv[] = { result };
		callback->Call(1, argv);
	}

private:
	vector<vector<double>> ratings;
	int rowIndex;
	int colIndex;
	double ratingPrediction;
};