#include "nan.h"
#include "../../include/recommender.h"

using namespace std;
using namespace Nan;
using namespace v8;

class TfIdfArraysWorker : public AsyncWorker {
public:
	TfIdfArraysWorker(Callback * callback, Recommender recommender, string query, vector<string> documents, bool useStopWords) :
		AsyncWorker(callback),
		recommender(recommender),
		query(query),
		documents(documents),
		useStopWords(useStopWords) {}

	void Execute() {
		this->recommender.tfidf(this->query, this->documents, this->useStopWords);
		vector<double> recs = this->recommender.recommend(this->recommender.weights);
		vector<string> sortedDocuments = this->recommender.getSortedDocuments(recs);
		this->result = sortedDocuments;
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
	string query;
	vector<string> documents;
	bool useStopWords;
	vector<string> result;
};