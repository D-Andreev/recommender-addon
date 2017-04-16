'use strict';

var assert = require('assert');
var recommender = require('recommender');

var documentPath = "resources\\document.txt";
var documentsPath = "resources\\documents.txt";

// TF-IDF
var weights = recommender.tfidf(documentPath, documentsPath);
var recommendations = recommender.recommend();
var sortedDocs = recommender.getSortedDocs();

var expectedWeights = {
    current: 0.3386294361119891,
    date: 0.3386294361119891,
    get: 0.3386294361119891,
    javascript: 0.47725887222397817,
    time: 0.2575364144903562
};
var expectedRecommendations = [1, 0.801901630090658, 0, 0.3223967271549685];
var expectedSortedDocs = [
    'get the current date and time in javascript',
    'get the current date and time in python',
    'what is the time now',
    'something very different'
];
console.log(sortedDocs);
assert.deepEqual(weights, expectedWeights);
assert.deepEqual(recommendations, expectedRecommendations);
assert.deepEqual(sortedDocs, expectedSortedDocs);

// Colaborative filtering example
var userRatings = [
    [ 1, 0, 3, 0, 0, 5, 0, 0, 5, 0, 4, 0 ],
	[ 0, 0, 5, 4, 0, 0, 4, 0, 0, 2, 1, 3 ],
	[ 2, 4, 0, 1, 2, 0, 3, 0, 4, 3, 5, 0 ],
	[ 0, 2, 4, 0, 5, 0, 0, 4, 0, 0, 2, 0 ],
	[ 0, 0, 4, 3, 4, 2, 0, 0, 0, 0, 2, 5 ],
	[ 1, 0, 3, 0, 3, 0, 0, 2, 0, 0, 4, 0 ]
];

console.log('Colaborative filtering example');
var rowIndex = 0;
var colIndex = 4;
var predictedRating = recommender.getRatingPrediction(userRatings, rowIndex, colIndex);
console.log(predictedRating);
assert.equal(predictedRating, 2.586406866934817);