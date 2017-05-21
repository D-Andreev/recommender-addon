#include <nan.h>
#include "include/recommender.h"
#include "src/workers/CollaborativeFilteringWorker.cpp"
#include "src/workers/GlobalBaselineWorker.cpp"
#include "src/workers/TopCFRecommendationsWorker.cpp"
#include "src/workers/TfIdfFilesWorker.cpp"
#include "src/workers/TfIdfArraysWorker.cpp"

using namespace Nan;
using namespace v8;

string getStringParameter(int index, NAN_METHOD_ARGS_TYPE info) {
	v8::String::Utf8Value stringParam(info[index]->ToString());
	string parsedString(*stringParam);

	return parsedString;
}

Local<Array> convertArrayToV8Array(vector<string> arr, int length) {
	Local<Array> result = New<v8::Array>(length);

	for (int i = 0; i < length; i++) {
		Nan::Set(result, i, Nan::New<String>(arr[i].c_str()).ToLocalChecked());
	}

	return result;
}

vector<string> castV8ArrayToArray(int index, NAN_METHOD_ARGS_TYPE info) {
	vector<string> documents;
	Local<Array> inputDocuments = Local<Array>::Cast(info[index]);

	for (unsigned i = 0; i < inputDocuments->Length(); i++) {
		if (Nan::Has(inputDocuments, i).FromJust()) {
			v8::String::Utf8Value doc(Nan::Get(inputDocuments, i).ToLocalChecked()->ToString());
			string document(*doc);
			documents.push_back(document);
		}
	}

	return documents;
}

void callCallbackWithInt(int index, NAN_METHOD_ARGS_TYPE info, int result) {
	Callback *callback = new Callback(info[index].As<Function>());
	Local<Value> argv[] = { Nan::New(result) };
	callback->Call(1, argv);
}

void callCallbackWithEmptyArray(int index, NAN_METHOD_ARGS_TYPE info) {
	Callback *callback = new Callback(info[index].As<Function>());
	Local<Value> argv[] = { New<v8::Array>() };
	callback->Call(1, argv);
}

vector<vector<double>> getMatrixParameter(int index, NAN_METHOD_ARGS_TYPE info) {
	vector<vector<double>> matrix;
	Local<Array> array = Local<Array>::Cast(info[index]);

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
			matrix.push_back(row);
		}
	}

	return matrix;
}

bool isOutsideMatrix(vector<vector<double>> matrix, int rowIndex, int colIndex) {
	return rowIndex < 0 || rowIndex >= matrix.size() ||
		colIndex < 0 || (matrix.size() > 0 && colIndex >= matrix[0].size());
}

void tfidfFilePaths(Recommender r, NAN_METHOD_ARGS_TYPE info, bool useStopWords) {
	string documentFilePath = getStringParameter(0, info);
	string documentsFilePath = getStringParameter(1, info);

	if (info[2]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[2].As<Function>());
		AsyncQueueWorker(
			new TfIdfFilesWorker(callback, r, documentFilePath, documentsFilePath, useStopWords)
		);
	} else if (info[3]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[3].As<Function>());
		AsyncQueueWorker(new TfIdfFilesWorker(
			callback, r, documentFilePath, documentsFilePath, useStopWords)
		);
	} else {
		// Sync
		map<string, double> weights = r.tfidf(documentFilePath, documentsFilePath, useStopWords);
		vector<double> recs = r.recommend(weights);
		vector<string> sortedDocuments = r.getSortedDocuments(recs);
		Local<Array> result = convertArrayToV8Array(sortedDocuments, sortedDocuments.size());

		info.GetReturnValue().Set(result);
	}
}

void tfidfArrays(Recommender r, NAN_METHOD_ARGS_TYPE info, bool useStopWords) {
	string query = getStringParameter(0, info);
	vector<string> documents = castV8ArrayToArray(1, info);

	if (info[2]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[2].As<Function>());
		AsyncQueueWorker(new TfIdfArraysWorker(callback, r, query, documents, useStopWords));
	} else if (info[3]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[3].As<Function>());
		AsyncQueueWorker(new TfIdfArraysWorker(callback, r, query, documents, useStopWords));
	} else {
		// Sync
		r.tfidf(query, documents, useStopWords);
		vector<double> recs = r.recommend(r.weights);
		vector<string> sortedDocuments = r.getSortedDocuments(recs);
		Local<Array> result = convertArrayToV8Array(sortedDocuments, sortedDocuments.size());

		info.GetReturnValue().Set(result);
	}
}

Local<Array> convertVectorOfPairsToV8Array(vector<pair<int, double>>& recommendations) {
	Local<Array> result = New<v8::Array>();

	for (unsigned i = 0; i < recommendations.size(); i++) {
		Local<Object> obj = Nan::New<Object>();
		Local<String> itemIdProp = Nan::New<String>("itemId").ToLocalChecked();
		Local<String> ratingProp = Nan::New<String>("rating").ToLocalChecked();
		obj->Set(itemIdProp, Nan::New<Number>(recommendations[i].first));
		obj->Set(ratingProp, Nan::New<Number>(recommendations[i].second));

		Nan::Set(result, i, obj);
	}

	return result;
}

NAN_METHOD(TfIdf) {
	Recommender r;
	if (!info[0]->IsString() || info[0]->ToString().IsEmpty()) Nan::ThrowError("Invalid query passed");

	bool useStopWords = false;
	if (info[2]->IsBoolean() && info[2]->BooleanValue()) {
		useStopWords = info[2]->BooleanValue();
	}

	if (info[1]->IsString()) {
		tfidfFilePaths(r, info, useStopWords);
	} else if (info[1]->IsArray()) {
		tfidfArrays(r, info, useStopWords);
	} else {
		Nan::ThrowError("Invalid params");
	}
}

NAN_METHOD(GetRatingPrediction) {
	Recommender r;
	if (!info[0]->IsArray() || !info[1]->IsNumber() || !info[2]->IsNumber()) {
		if (info[3]->IsFunction()) return callCallbackWithInt(3, info, 0);
		else return info.GetReturnValue().Set(0);
	}

	vector<vector<double>> ratings = getMatrixParameter(0, info);
	int rowIndex = info[1]->IntegerValue();
	int colIndex = info[2]->IntegerValue();
	
	if (isOutsideMatrix(ratings, rowIndex, colIndex)) {
		if (info[3]->IsFunction()) return callCallbackWithInt(3, info, 0);
		else return info.GetReturnValue().Set(0);
	}

	if (info[3]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[3].As<Function>());
		AsyncQueueWorker(new CollaborativeFilteringWorker(callback, r, ratings, rowIndex, colIndex));
	} else {
		// Sync
		double predictedRating = r.getRatingPrediction(ratings, rowIndex, colIndex);
		Local<Number> result = Nan::New(predictedRating);

		info.GetReturnValue().Set(result);
	}
}

NAN_METHOD(GetGlobalBaselineRatingPrediction) {
	Recommender r;

	if (!info[0]->IsArray() || !info[1]->IsNumber() || !info[2]->IsNumber()) {
		if (info[3]->IsFunction()) return callCallbackWithInt(3, info, 0);
		else return info.GetReturnValue().Set(0);
	}

	vector<vector<double>> ratings = getMatrixParameter(0, info);
	int rowIndex = info[1]->IntegerValue();
	int colIndex = info[2]->IntegerValue();
	Local<Array> array = Local<Array>::Cast(info[0]);

	if (isOutsideMatrix(ratings, rowIndex, colIndex)) {
		if (info[3]->IsFunction()) return callCallbackWithInt(3, info, 0);
		else return info.GetReturnValue().Set(0);
	}

	if (info[3]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[3].As<Function>());
		AsyncQueueWorker(new GlobalBaselineWorker(callback, r, ratings, rowIndex, colIndex));
	} else {
		// Sync
		double predictedRating = r.getGlobalBaselineRatingPrediction(ratings, rowIndex, colIndex);
		Local<Number> result = Nan::New(predictedRating);

		info.GetReturnValue().Set(result);
	}
}

NAN_METHOD(GetTopCFRecommendations) {
	Recommender r;

	if (!info[0]->IsArray() || !info[1]->IsNumber()) {
		if (info[2]->IsFunction()) return callCallbackWithEmptyArray(2, info);
		else if (info[3]->IsFunction()) return callCallbackWithEmptyArray(3, info);
		else return info.GetReturnValue().Set(New<v8::Array>());
	}

	vector<vector<double>> ratings = getMatrixParameter(0, info);
	int rowIndex = info[1]->IntegerValue();
	int limit = -1;
	if (info[2]->IntegerValue()) limit = info[2]->IntegerValue();

	if (rowIndex < 0 || rowIndex > ratings.size()) {
		if (info[2]->IsFunction()) return callCallbackWithEmptyArray(2, info); 
		else if (info[3]->IsFunction()) return callCallbackWithEmptyArray(3, info);
		else return info.GetReturnValue().Set(New<v8::Array>());
	}

	if (info[2]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[2].As<Function>());
		AsyncQueueWorker(new TopCFRecommendationsWorker(callback, r, ratings, rowIndex, limit));
	} else if (info[3]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[3].As<Function>());
		AsyncQueueWorker(new TopCFRecommendationsWorker(callback, r, ratings, rowIndex, limit));
	} else {
		// Sync
		vector<pair<int, double>> recommendations = r.getTopCFRecommendations(ratings, rowIndex, limit);
		Local<Array> result = convertVectorOfPairsToV8Array(recommendations);
		
		info.GetReturnValue().Set(result);
	}
}

NAN_MODULE_INIT(Init) {
	Nan::Set(target, New<String>("tfidf").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(TfIdf)).ToLocalChecked());
	Nan::Set(target, New<String>("getRatingPrediction").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(GetRatingPrediction)).ToLocalChecked());
	Nan::Set(target, New<String>("getGlobalBaselineRatingPrediction").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(GetGlobalBaselineRatingPrediction)).ToLocalChecked());
	Nan::Set(target, New<String>("getTopCFRecommendations").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(GetTopCFRecommendations)).ToLocalChecked());
}

NODE_MODULE(hello_nan_addon, Init)