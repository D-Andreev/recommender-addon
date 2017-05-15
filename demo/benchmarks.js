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
        var weights = recommender.tfidf(longDocumentPath, longDocumentsPath);
        var recommendations = recommender.recommend();
        var sortedDocs = recommender.getSortedDocs();
        done();
    },
    function ratingPrediction(done) {
        var predictedRating = recommender.getRatingPrediction(userRatings, 0, 1);
        assert.ok(predictedRating === 5);
        done();
    },
    function getGlobalBaselineRatingPrediction(done) {
        var predictedRating = recommender.getGlobalBaselineRatingPrediction(userRatings, 0, 1);
        assert.ok(predictedRating === 4.090909090909092);
        done();
    }
], 10);

run(run);