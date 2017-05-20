'use strict';

var assert = require('assert');
var bench = require('fastbench');
var recommender = require('recommender');
var documentPath = "./resources/document.txt";
var documentsPath = "./resources/documents.txt";
var longDocumentPath = "./resources/longDocument.txt";
var longDocumentsPath = "./resources/longDocuments.txt";
var userRatings =  [
	[ 4, 0, 0, 1, 1, 0, 0 ],
    [ 5, 5, 4, 0, 0, 0, 0 ],
    [ 0, 0, 0, 2, 4, 5, 0 ],
    [ 0, 3, 0, 0, 0, 0, 3 ]
]

var run = bench([
    function tfidf(done) {
        var sortedDocs = recommender.tfidf(longDocumentPath, longDocumentsPath);
        done();
    },
    function ratingPrediction(done) {
        var predictedRating = recommender.getRatingPrediction(userRatings, 0, 1);
        assert.ok(predictedRating === 5);
        done();
    },
    function getGlobalBaselineRatingPrediction(done) {
        var predictedRating = recommender.getGlobalBaselineRatingPrediction(userRatings, 0, 1);
        assert.ok(predictedRating === 2.6363636363636362);
        done();
    }
], 10);

run(run);