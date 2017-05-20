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
var sortedDocs = recommender.getSortedDocs(recommendations);
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
var sortedDocs = recommender.getSortedDocs(recommendations);

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
console.log('='.repeat(50));
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
console.log('='.repeat(50));
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
console.log('='.repeat(50));
console.log('Global baseline example');
predictedRating = recommender.getGlobalBaselineRatingPrediction(userRatings, rowIndex, colIndex);
console.log(predictedRating);
assert.equal(predictedRating, 3.928571428571429);

// Global baseline another example
console.log('='.repeat(50));
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
console.log('='.repeat(50));
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

// Colaborative filtering async example
userRatings = [
    [4, 0, 0, 1, 1, 0, 0],
	[5, 5, 4, 0, 0, 0, 0],
	[0, 0, 0, 2, 4, 5, 0],
	[3, 0, 0, 0, 0, 0, 3]
];
console.log('='.repeat(50));
console.log('Colaborative filtering async example');
var rowIndex = 0;
var colIndex = 4;
recommender.getRatingPrediction(userRatings, rowIndex, colIndex, (res) => {
    console.log('Colaborative filtering async example result', res);
    assert.equal(res, 4);
});

// Global baseline async example
console.log('='.repeat(50));
console.log('Global baseline async example');
userRatings = [
    [4, 0, 0, 1, 1, 0, 0],
	[5, 5, 4, 0, 0, 0, 0],
	[0, 0, 0, 2, 4, 5, 0],
	[3, 0, 0, 0, 0, 0, 3]
];
rowIndex = 0;
colIndex = 1;
recommender.getGlobalBaselineRatingPrediction(userRatings, rowIndex, colIndex, (res) => {
    console.log('Global baseline async example result', predictedRating);
    assert.equal(predictedRating, 3.6363636363636362);
});

// Get TOP CF Recommendations async
console.log('='.repeat(50));
console.log('Top CF Recommendations async');
var expectedRecommendations = [
  { itemId: 1, rating: 5 },
  { itemId: 5, rating: 5 },
  { itemId: 2, rating: 4 }
];
recommender.getTopCFRecommendations(userRatings, 0, 100, (res) => {
    console.log('Top CF Recommendations async result', res);
    res.forEach((item, index) => {
        assert.equal(item.itemId, expectedRecommendations[index].itemId);
        assert.equal(item.rating, expectedRecommendations[index].rating);
    });
});

// TF-IDF with files input async
console.log('TF-IDF with files input async');
recommender.tfidf(documentPath, documentsPath, (res) => {
    console.log('TF-IDF with files input async result', res);
    assert.deepEqual(res, expectedWeights);
});

// TF-IDF with string and array params async
console.log('TF-IDF with string and array params async');
var query = 'get current date time javascript';
var documents = [
    'get the current date and time in javascript',
    'get the current date and time in python',
    'something very different',
    'what is the time now'
];

recommender.tfidf(query, documents, (res) => {
    console.log('TF-IDF with string and array params async result', res);
    assert.deepEqual(res, expectedWeights);
});

// TF-IDF with files input async
console.log('TF-IDF with files input async');
recommender.tfidf(documentPath, documentsPath, (res) => {
    console.log('TF-IDF with files input async result', res);
    assert.deepEqual(res, expectedWeights);
});

// TF-IDF with string and array params and stop words async
console.log('TF-IDF with string and array params and stop words async');
var query = 'get current date time javascript';
var documents = [
    'get the current date and time in javascript',
    'get the current date and time in python',
    'something very different',
    'what is the time now'
];

recommender.tfidf(query, documents, false, (res) => {
    console.log('TF-IDF with string and array params async and stopWords  result', res);
    assert.deepEqual(res, expectedWeights);
});

// TF-IDF with files input and stopWords async
console.log('TF-IDF with files input and stopWords async');
recommender.tfidf(documentPath, documentsPath, false, (res) => {
    console.log('TF-IDF with files input and stopWords async result', res);
    assert.deepEqual(res, expectedWeights);
});

// TF-IDF recommend async
console.log('TF-IDF recommend async');
recommender.tfidf(documentPath, documentsPath, false, (res) => {
    console.log('TF-IDF with files input and stopWords async result', res);
    assert.deepEqual(res, expectedWeights);
    recommender.tfidf(documentPath, documentsPath, false, (res) => {
        console.log('TF-IDF recommend async result', res);
        assert.deepEqual(res, expectedWeights);
    });
});

// TF-IDF getSortedDocs async
console.log('TF-IDF getSortedDocs async');
recommender.tfidf(documentPath, documentsPath, false, (res) => {
    console.log('TF-IDF with files input and stopWords async result', res);
    assert.deepEqual(res, expectedWeights);
    recommender.tfidf(documentPath, documentsPath, false, (res) => {
        console.log('TF-IDF recommend async result', res);
        assert.deepEqual(res, expectedWeights);
        recommender.getSortedDocs(res, (sortedDocumentsResult) => {
            console.log('TF-IDF getSortedDocs async result', sortedDocumentsResult);
            assert.deepEqual(sortedDocs, expectedSortedDocs);
        });
    });
});