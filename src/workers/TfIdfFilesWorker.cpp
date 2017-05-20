#include "nan.h"
#include "../../include/recommender.h"

using namespace std;
using namespace Nan;
using namespace v8;

class TfIdfFilesWorker : public AsyncWorker {
public:
	TfIdfFilesWorker(Callback * callback, Recommender recommender, string documentFilePath, string documentsFilePath, bool useStopWords) :
		AsyncWorker(callback),
		recommender(recommender),
		documentFilePath(documentFilePath),
		documentsFilePath(documentsFilePath),
		useStopWords(useStopWords) {}

	void Execute() {
		this->recommender.tfidf(this->documentFilePath, this->documentsFilePath, this->useStopWords);
		this->recommender.tfidf(documentFilePath, documentsFilePath, useStopWords);
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
	string documentFilePath;
	string documentsFilePath;
	bool useStopWords;
	vector<string> result;
};