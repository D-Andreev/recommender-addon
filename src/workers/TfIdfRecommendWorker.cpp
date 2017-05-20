#include "nan.h"
#include "../../include/recommender.h"

using namespace std;
using namespace Nan;
using namespace v8;

class TfIdfRecommendWorker : public AsyncWorker {
public:
	TfIdfRecommendWorker(Callback * callback, Recommender recommender) :
		AsyncWorker(callback),
		recommender(recommender) {}

	void Execute() {
		this->result = this->recommender.recommend();
	}

	void HandleOKCallback() {
		Local<Array> result = New<v8::Array>(this->result.size());

		for (unsigned i = 0; i < this->result.size(); i++) {
			Nan::Set(result, i, Nan::New<Number>(this->result[i]));
		}

		Local<Value> argv[] = { result };
		callback->Call(1, argv);
	}

private:
	Recommender recommender;
	string query;
	vector<double> result;
};