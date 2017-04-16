#include <nan.h>
#include "src/recommender.cpp"
#include <iostream>
#include <fstream>

using namespace Nan;
using namespace v8;

Recommender r;
vector<double> similarities;

NAN_METHOD(TfIdf) {
	if (!info[0]->IsString() || !info[1]->IsString()) {
		Nan::ThrowError("Invalid params");
	}

	v8::String::Utf8Value documentFilePathValue(info[0]->ToString());
	v8::String::Utf8Value documentsFilePathValue(info[1]->ToString());

	std::string documentFilePath(*documentFilePathValue);
	std::string documentsFilePath(*documentsFilePathValue);

	r.tfidf(documentFilePath, documentsFilePath);

	Local<Object> obj = Nan::New<Object>();
	for (auto const &ent1 : r.weights) {
		Local<String> prop = Nan::New<String>(ent1.first).ToLocalChecked();
		Nan::Set(obj, prop, Nan::New<Number>(ent1.second));
	}
	
	info.GetReturnValue().Set(obj);
}

NAN_METHOD(Recommend) {
	similarities = r.recommend();
	Local<Array> result = New<v8::Array>(similarities.size());

	for (int i = 0; i < similarities.size(); i++) {
		Nan::Set(result, i, Nan::New<Number>(similarities[i]));
	}

	info.GetReturnValue().Set(result);
}

NAN_METHOD(GetSortedDocs) {
	vector<string> sortedDocuments = r.getSortedDocuments(similarities);
	int sortedDocumentsSize = sortedDocuments.size();

	Local<Array> result = New<v8::Array>(sortedDocumentsSize);
	for (int i = 0; i < sortedDocumentsSize; i++) {
		Nan::Set(result, i, Nan::New<String>(sortedDocuments[i].c_str()).ToLocalChecked());
	}

	info.GetReturnValue().Set(result);
}

NAN_METHOD(GetRatingPrediction) {
	if (!info[0]->IsArray() || !info[1]->IsInt32() || !info[2]->IsInt32()) {
		Nan::ThrowError("Invalid params");
	}
	int rowIndex = info[1]->IntegerValue();
	int colIndex = info[2]->IntegerValue();
	Local<Array> array = Local<Array>::Cast(info[0]);
	vector<vector<double>> ratings;

	for (unsigned i = 0; i < array->Length(); i++) {
		if (Nan::Has(array, i).FromJust()) {
			vector<double> row;
			Local<Array> inputRow = Local<Array>::Cast(Nan::Get(array, i).ToLocalChecked());
			for (unsigned j = 0; j < inputRow->Length(); j++) {
				if (Nan::Has(inputRow, j).FromJust()) {
					double value = Nan::Get(inputRow, j).ToLocalChecked()->NumberValue();
					row.push_back(value);
				}
			}
			ratings.push_back(row);
		}
	}

	double predictedRating = r.getRatingPrediction(ratings, rowIndex, colIndex);
	Local<Number> result = Nan::New(predictedRating);
	info.GetReturnValue().Set(result);
}

NAN_MODULE_INIT(Init) {
	Nan::Set(target, New<String>("tfidf").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(TfIdf)).ToLocalChecked());
	Nan::Set(target, New<String>("recommend").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(Recommend)).ToLocalChecked());
	Nan::Set(target, New<String>("getSortedDocs").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(GetSortedDocs)).ToLocalChecked());
	Nan::Set(target, New<String>("getRatingPrediction").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(GetRatingPrediction)).ToLocalChecked());
}
NODE_MODULE(hello_nan_addon, Init)
