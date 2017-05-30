'use strict';

var assert = require('assert');
var bench = require('fastbench');
var recommender = require('recommender');
var documentPath = "./resources/document.txt";
var documentsPath = "./resources/documents.txt";
var userRatings =  [
	[ 4, 0, 0, 1, 1, 0, 0 ],
    [ 5, 5, 4, 0, 0, 0, 0 ],
    [ 0, 0, 0, 2, 4, 5, 0 ],
    [ 3, 0, 0, 0, 0, 0, 3 ]
]

var run = bench([
    function tfidf(done) {
        recommender.tfidf(documentPath, documentsPath, (sortedDocs) => {
            assert.equal(sortedDocs.length, 4);
            done();
        });
    },
    function ratingPrediction(done) {
        recommender.getRatingPrediction(userRatings, 0, 1, (predictedRating) => {
            assert.equal(predictedRating, 5);
            done();
        });
    },
    function getGlobalBaselineRatingPrediction(done) {
        recommender.getGlobalBaselineRatingPrediction(userRatings, 0, 1, (predictedRating) => {
            assert.equal(predictedRating, 3.6363636363636362);
            done();
        });
    },
    function getTopCFRecommendations(done) {
        recommender.getTopCFRecommendations(userRatings, 0, (recommendations) => {
            assert.equal(recommendations.length, 4);
            done();
        });
    }
], 100000);

run(run);