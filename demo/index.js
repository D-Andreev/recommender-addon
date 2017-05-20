'use strict';

var assert = require('assert');
var recommender = require('recommender');

var documentPath = "./resources/document.txt";
var documentsPath = "./resources/documents.txt";

// TF-IDF with string and array params
console.log('TF-IDF with string and array params');
var query = 'get current date time javascript';
var documents = [
    'get the current date and time in javascript',
    'get the current date and time in python',
    'something very different',
    'what is the time now'
];

var weights = recommender.tfidf(query, documents);
console.log(weights);
var recommendations = recommender.recommend();
console.log(recommendations);
var sortedDocs = recommender.getSortedDocs();
console.log(sortedDocs);

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

// TF-IDF with files input
console.log('TF-IDF with files input');
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

// Colaborative filtering another example
userRatings = [
    [1, 0, 3, 0, 0, 5, 0, 0, 5, 0, 4, 0],
	[0, 0, 5, 4, 0, 0, 4, 0, 0, 2, 1, 3],
	[2, 4, 0, 1, 0, 0, 3, 0, 4, 3, 5, 0],
	[0, 2, 4, 0, 0, 0, 0, 4, 0, 0, 2, 0],
	[0, 0, 4, 3, 0, 2, 0, 0, 0, 0, 2, 5],
	[1, 0, 3, 0, 0, 0, 0, 2, 0, 0, 4, 0]
];

console.log('Colaborative filtering example');
var rowIndex = 0;
var colIndex = 4;
var predictedRating = recommender.getRatingPrediction(userRatings, rowIndex, colIndex);
console.log(predictedRating);
assert.equal(predictedRating, 0);

// Global baseline rating prediction
userRatings = [
    [1, 0, 3, 0, 0, 5, 0, 0, 5, 0, 4, 0],
	[0, 0, 5, 4, 0, 0, 4, 0, 0, 2, 1, 3],
	[2, 4, 0, 1, 2, 0, 3, 0, 4, 3, 5, 0],
	[0, 2, 4, 0, 5, 0, 0, 4, 0, 0, 2, 0],
	[0, 0, 4, 3, 4, 2, 0, 0, 0, 0, 2, 5],
	[1, 0, 3, 0, 3, 0, 0, 2, 0, 0, 4, 0]
];
console.log('Global baseline example');
predictedRating = recommender.getGlobalBaselineRatingPrediction(userRatings, rowIndex, colIndex);
console.log(predictedRating);
assert.equal(predictedRating, 3.928571428571429);

// Global baseline another example
console.log('Global baseline another example');
userRatings = [
    [4, 0, 0, 1, 1, 0, 0],
	[5, 5, 4, 0, 0, 0, 0],
	[0, 0, 0, 2, 4, 5, 0],
	[3, 0, 0, 0, 0, 0, 3]
];
rowIndex = 0;
colIndex = 1;
var predictedRating = recommender.getGlobalBaselineRatingPrediction(userRatings, rowIndex, colIndex);
console.log(predictedRating);
assert.equal(predictedRating, 3.6363636363636362);

// Get TOP CF Recommendations
console.log('Top CF Recommendations');
var expectedRecommendations = [
  { itemId: 1, rating: 5 },
  { itemId: 5, rating: 5 },
  { itemId: 2, rating: 4 }
];
var recommendations = recommender.getTopCFRecommendations(userRatings, 0, 100);
console.log(recommendations);
recommendations.forEach((item, index) => {
    assert.equal(item.itemId, expectedRecommendations[index].itemId);
    assert.equal(item.rating, expectedRecommendations[index].rating);
});