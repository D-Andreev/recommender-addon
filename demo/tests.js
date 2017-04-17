"use strict";

var assert = require('assert');
var mocha = require('mocha');
var chai = require('chai');
var expect = chai.expect;
var r = require('recommender');

describe('tfidf', function () {
    beforeEach(function () {
        this.query = "get current date time javascript";
        this.documents = [
            'get the current date and time in javascript',
            'get the current date and time in python',
            'something very different',
            'what is the time now'
        ];
        this.queryFilePath = './resources/document.txt';
        this.documentsFilePath = './resources/documents.txt';
        this.expectedWeights = {
            current: 0.3386294361119891,
            date: 0.3386294361119891,
            get: 0.3386294361119891,
            javascript: 0.47725887222397817,
            time: 0.2575364144903562
        };
    });
    describe('when correct params are sent', function () {
        describe('when a string and array are passed', function () {
            it('returns the correct weights', function () {
                var weights = r.tfidf(this.query, this.documents);
                assert.deepEqual(weights, this.expectedWeights);
            });
        });
        describe('when file paths are passed', function () {
            it('returns the correct weights', function () {
                var weights = r.tfidf(this.queryFilePath, this.documentsFilePath);
                assert.deepEqual(weights, this.expectedWeights);
            });
        });
        describe('when filterStopWords is true', function () {
            it('returns the correct weights', function () {
                var weights = r.tfidf(this.queryFilePath, this.documentsFilePath, true);
                assert.deepEqual(weights, this.expectedWeights);
            });
        });
    });
    describe('when invalid params are sent', function () {
        it('throws error', function () {
            expect(r.tfidf).to.throw('Invalid params');
        });
        it('throws error', function () {
            expect(r.tfidf.bind(this, this.queryFilePath)).to.throw('Invalid params');
        });
        it('throws error', function () {
            expect(r.tfidf.bind(this, this.documentsPath)).to.throw('Invalid params');
        });
    });
});