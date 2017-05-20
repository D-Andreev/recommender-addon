#include "nan.h"
#include "../../include/recommender.h"

using namespace std;
using namespace Nan;
using namespace v8;

class TfIdfGetSortedDocsWorker : public AsyncWorker {
public:
	TfIdfGetSortedDocsWorker(Callback * callback, Recommender recommender, vector<double> similarities) :
		AsyncWorker(callback),
		recommender(recommender),
		similarities(similarities) {}

	void Execute() {
		this->result = this->recommender.getSortedDocuments(this->similarities);
	}

	void HandleOKCallback() {
		int sortedDocumentsSize = this->result.size();
		Local<Array> result = New<v8::Array>(sortedDocumentsSize);
		for (int i = 0; i < sortedDocumentsSize; i++) {
			Nan::Set(result, i, Nan::New<String>(this->result[i].c_str()).ToLocalChecked());
		}

		Local<Value> argv[] = { result };
		callback->Call(1, argv);
	}

private:
	Recommender recommender;
	vector<double> similarities;
	vector<string> result;
};