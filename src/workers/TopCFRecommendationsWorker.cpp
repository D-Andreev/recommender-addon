#include "nan.h"
#include "../../include/recommender.h"

using namespace std;
using namespace Nan;
using namespace v8;

class TopCFRecommendationsWorker : public AsyncWorker {
public:
	TopCFRecommendationsWorker(Callback * callback, Recommender recommender, vector<vector<double>> ratings, int rowIndex, int limit, int includeRatedItems) :
		AsyncWorker(callback),
		recommender(recommender),
		ratings(ratings),
		rowIndex(rowIndex),
		limit(limit),
		includeRatedItems(includeRatedItems) {}

	void Execute() {
		this->result = this->recommender.getTopCFRecommendations(this->ratings, this->rowIndex, this->limit, this->includeRatedItems);
	}

	void HandleOKCallback() {
		Local<Array> result = New<v8::Array>();
		for (unsigned i = 0; i < this->result.size(); i++) {
			Local<Object> obj = Nan::New<Object>();
			Local<String> itemIdProp = Nan::New<String>("itemId").ToLocalChecked();
			Local<String> ratingProp = Nan::New<String>("rating").ToLocalChecked();
			obj->Set(itemIdProp, Nan::New<Number>(this->result[i].first));
			obj->Set(ratingProp, Nan::New<Number>(this->result[i].second));

			Nan::Set(result, i, obj);
		}
		Local<Value> argv[] = { result };
		callback->Call(1, argv);
	}

private:
	Recommender recommender;
	vector<vector<double>> ratings;
	int rowIndex;
	int limit;
	int includeRatedItems;
	vector<pair<int, double>> result;
};