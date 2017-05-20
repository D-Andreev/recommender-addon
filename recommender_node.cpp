#include <nan.h>
#include "include/recommender.h"
#include "src/workers/CollaborativeFilteringWorker.cpp"
#include "src/workers/GlobalBaselineWorker.cpp"
#include "src/workers/TopCFRecommendationsWorker.cpp"
#include "src/workers/TfIdfFilesWorker.cpp"
#include "src/workers/TfIdfArraysWorker.cpp"
#include "src/workers/TfIdfRecommendWorker.cpp"
#include "src/workers/TfIdfGetSortedDocsWorker.cpp"

using namespace Nan;
using namespace v8;

Recommender r;

NAN_METHOD(TfIdf) {
	if (!info[0]->IsString()) Nan::ThrowError("Invalid params");
	if (info[1]->IsString()) {
		// Documents file path is passed
		v8::String::Utf8Value documentFilePathValue(info[0]->ToString());
		v8::String::Utf8Value documentsFilePathValue(info[1]->ToString());

		string documentFilePath(*documentFilePathValue);
		string documentsFilePath(*documentsFilePathValue);

		bool useStopWords = false;
		if (info[2]->IsBoolean() && info[2]->BooleanValue()) useStopWords = info[2]->BooleanValue();
		if (info[2]->IsFunction()) {
			// Async
			Callback *callback = new Callback(info[2].As<Function>());
			AsyncQueueWorker(new TfIdfFilesWorker(callback, r, documentFilePath, documentsFilePath, useStopWords));
		}
		else if (info[3]->IsFunction()) {
			// Async
			Callback *callback = new Callback(info[3].As<Function>());
			AsyncQueueWorker(new TfIdfFilesWorker(callback, r, documentFilePath, documentsFilePath, useStopWords));
		}
		else {
			// Sync
			r.tfidf(documentFilePath, documentsFilePath, useStopWords);

			Local<Object> obj = Nan::New<Object>();
			for (auto const &ent1 : r.weights) {
				Local<String> prop = Nan::New<String>(ent1.first).ToLocalChecked();
				Nan::Set(obj, prop, Nan::New<Number>(ent1.second));
			}

			info.GetReturnValue().Set(obj);
		}
	}
	else if (info[1]->IsArray()) {
		// Arrays are passed
		vector<string> documents;
		v8::String::Utf8Value inputQuery(info[0]->ToString());
		string query(*inputQuery);
		bool useStopWords = false;
		Local<Array> inputDocuments = Local<Array>::Cast(info[1]);
		for (unsigned i = 0; i < inputDocuments->Length(); i++) {
			if (Nan::Has(inputDocuments, i).FromJust()) {
				v8::String::Utf8Value doc(Nan::Get(inputDocuments, i).ToLocalChecked()->ToString());
				string document(*doc);
				documents.push_back(document);
			}
		}
		if (info[2]->IsBoolean() && info[2]->BooleanValue()) useStopWords = info[2]->BooleanValue();
		if (info[2]->IsFunction()) {
			// Async
			Callback *callback = new Callback(info[2].As<Function>());
			AsyncQueueWorker(new TfIdfArraysWorker(callback, r, query, documents, useStopWords));
		}
		else if (info[3]->IsFunction()) {
			// Async
			Callback *callback = new Callback(info[3].As<Function>());
			AsyncQueueWorker(new TfIdfArraysWorker(callback, r, query, documents, useStopWords));
		}
		else {
			// Sync
			r.tfidf(query, documents, useStopWords);

			Local<Object> obj = Nan::New<Object>();
			for (auto const &ent1 : r.weights) {
				Local<String> prop = Nan::New<String>(ent1.first).ToLocalChecked();
				Nan::Set(obj, prop, Nan::New<Number>(ent1.second));
			}

			info.GetReturnValue().Set(obj);
		}
	}
	else {
		Nan::ThrowError("Invalid params");
	}
}

NAN_METHOD(Recommend) {
	if (info[0]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[0].As<Function>());
		AsyncQueueWorker(new TfIdfRecommendWorker(callback, r));
	}
	else {
		// Sync
		vector<double> similarities = r.recommend();
		Local<Array> result = New<v8::Array>(similarities.size());

		for (unsigned i = 0; i < similarities.size(); i++) {
			Nan::Set(result, i, Nan::New<Number>(similarities[i]));
		}

		info.GetReturnValue().Set(result);
	}
}

NAN_METHOD(GetSortedDocs) {
	if (!info[0]->IsArray()) Nan::ThrowError("Invalid params");
	vector<double> similarities;
	Local<Array> array = Local<Array>::Cast(info[0]);
	for (unsigned j = 0; j < array->Length(); j++) {
		if (Nan::Has(array, j).FromJust()) {
			double value = Nan::Get(array, j).ToLocalChecked()->NumberValue();
			similarities.push_back(value);
		}
	}

	if (info[1]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[1].As<Function>());
		AsyncQueueWorker(new TfIdfGetSortedDocsWorker(callback, r, similarities));
	}
	else {
		// Sync
		vector<string> sortedDocuments = r.getSortedDocuments(similarities);
		int sortedDocumentsSize = sortedDocuments.size();

		Local<Array> result = New<v8::Array>(sortedDocumentsSize);
		for (int i = 0; i < sortedDocumentsSize; i++) {
			Nan::Set(result, i, Nan::New<String>(sortedDocuments[i].c_str()).ToLocalChecked());
		}

		info.GetReturnValue().Set(result);
	}
}

NAN_METHOD(GetRatingPrediction) {
	if (!info[0]->IsArray() || !info[1]->IsInt32() || !info[2]->IsInt32()) {
		Nan::ThrowError("Invalid params");
	}

	vector<vector<double>> ratings;
	int rowIndex = info[1]->IntegerValue();
	int colIndex = info[2]->IntegerValue();
	Local<Array> array = Local<Array>::Cast(info[0]);

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
	if (info[3]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[3].As<Function>());
		AsyncQueueWorker(new CollaborativeFilteringWorker(callback, r, ratings, rowIndex, colIndex));
	}
	else {
		// Sync
		double predictedRating = r.getRatingPrediction(ratings, rowIndex, colIndex);
		Local<Number> result = Nan::New(predictedRating);
		info.GetReturnValue().Set(result);
	}
}

NAN_METHOD(GetGlobalBaselineRatingPrediction) {
	if (!info[0]->IsArray() || !info[1]->IsInt32() || !info[2]->IsInt32()) {
		Nan::ThrowError("Invalid params");
	}

	vector<vector<double>> ratings;
	int rowIndex = info[1]->IntegerValue();
	int colIndex = info[2]->IntegerValue();
	Local<Array> array = Local<Array>::Cast(info[0]);

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

	if (info[3]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[3].As<Function>());
		AsyncQueueWorker(new GlobalBaselineWorker(callback, r, ratings, rowIndex, colIndex));
	}
	else {
		// Sync
		double predictedRating = r.getGlobalBaselineRatingPrediction(ratings, rowIndex, colIndex);
		Local<Number> result = Nan::New(predictedRating);

		info.GetReturnValue().Set(result);
	}
}

NAN_METHOD(GetTopCFRecommendations) {
	if (!info[0]->IsArray() || !info[1]->IsInt32()) {
		Nan::ThrowError("Invalid params");
	}

	vector<vector<double>> ratings;
	int rowIndex = info[1]->IntegerValue();
	int limit = -1;
	if (info[2]->IntegerValue()) limit = info[2]->IntegerValue();
	Local<Array> array = Local<Array>::Cast(info[0]);

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

	if (info[3]->IsFunction()) {
		// Async
		Callback *callback = new Callback(info[3].As<Function>());
		AsyncQueueWorker(new TopCFRecommendationsWorker(callback, r, ratings, rowIndex, limit));
	}
	else {
		// Sync
		vector<pair<int, double>> recommendations = r.getTopCFRecommendations(ratings, rowIndex, limit);
		Local<Array> result = New<v8::Array>();
		for (unsigned i = 0; i < recommendations.size(); i++) {
			Local<Object> obj = Nan::New<Object>();
			Local<String> itemIdProp = Nan::New<String>("itemId").ToLocalChecked();
			Local<String> ratingProp = Nan::New<String>("rating").ToLocalChecked();
			obj->Set(itemIdProp, Nan::New<Number>(recommendations[i].first));
			obj->Set(ratingProp, Nan::New<Number>(recommendations[i].second));

			Nan::Set(result, i, obj);
		}
		info.GetReturnValue().Set(result);
	}
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
	Nan::Set(target, New<String>("getGlobalBaselineRatingPrediction").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(GetGlobalBaselineRatingPrediction)).ToLocalChecked());
	Nan::Set(target, New<String>("getTopCFRecommendations").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(GetTopCFRecommendations)).ToLocalChecked());
}
NODE_MODULE(hello_nan_addon, Init)