#include "nan.h"
#include "../include/recommender.h"

using namespace std;
using namespace Nan;
using namespace v8;

class TfIdfFilesWorker : public AsyncWorker {
public:
	TfIdfFilesWorker(Callback * callback, string documentFilePath, string documentsFilePath, bool useStopWords) :
		AsyncWorker(callback),
		documentFilePath(documentFilePath),
		documentsFilePath(documentsFilePath),
		useStopWords(useStopWords) {}

	void Execute() {
		Recommender recommender;
		recommender.tfidf(this->documentFilePath, this->documentsFilePath, this->useStopWords);
		this->result = recommender.weights;
	}

	void HandleOKCallback() {
		Local<Object> obj = Nan::New<Object>();
		for (auto const &ent1 : this->result) {
			Local<String> prop = Nan::New<String>(ent1.first).ToLocalChecked();
			Nan::Set(obj, prop, Nan::New<Number>(ent1.second));
		}

		Local<Value> argv[] = { obj };
		callback->Call(1, argv);
	}

private:
	string documentFilePath;
	string documentsFilePath;
	bool useStopWords;
	map<string, double> result;
};