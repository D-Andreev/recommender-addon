'use strict';

var assert = require('assert');
var bench = require('fastbench');
var recommender = require('recommender');
var documentPath = "./resources/document.txt";
var documentsPath = "./resources/documents.txt";
var longDocumentPath = "./resources/longDocument.txt";
var longDocumentsPath = "./resources/longDocuments.txt";
var ROWS = 1000;
var COLS = 1000;

function generateRandomRatingsMatrix() {
    var userRatings = [];
    for (var i = 0; i < ROWS; i++) {
        var row = [];
        for (var j = 0; j < COLS; j++) {
            row.push(getRandomInt(0, 5));
        }
        userRatings.push(row);
    }

    return userRatings;
}

function getRandomInt(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

var userRatings = generateRandomRatingsMatrix();

var run = bench([
    function tfidf(done) {
        var weights = recommender.tfidf(longDocumentPath, longDocumentsPath);
        var recommendations = recommender.recommend();
        var sortedDocs = recommender.getSortedDocs();
        done();
    },
    function ratingPrediction(done) {
        var rowIndex = 0;
        var colIndex = 4;
        var predictedRating = recommender.getRatingPrediction(userRatings, rowIndex, colIndex);
        assert.ok(predictedRating > 0)
        done();
    }
], 10);

run(run);