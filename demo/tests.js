"use strict";

var assert = require('assert');
var mocha = require('mocha');
var chai = require('chai');
var expect = chai.expect;
var r = require('recommender');

describe('Recommender', () => {
    context('tfidf', () => {
        beforeEach(() => {
            this.query = "get current date time javascript";
            this.documents = [
                'get the current date and time in javascript',
                'get the current date and time in python',
                'something very different',
                'what is the time now'
            ];
            this.queryFilePath = './resources/document.txt';
            this.documentsFilePath = './resources/documents.txt';

            this.expectedSortedDocs = [
                'get the current date and time in javascript',
                'get the current date and time in python',
                'what is the time now',
                'something very different'
            ];
        });

        context('when correct params are sent', () => {
            describe('when a string and array are passed', () => {
                describe('when filterStopWords is not passed', () => {
                    it('returns the correct sorted docs', () => {
                        let sortedDocs = r.tfidf(this.query, this.documents);
                        expect(sortedDocs).to.eql(this.expectedSortedDocs);
                    });
                });

                describe('when filterStopWords is passed', () => {
                    it('returns the correct sorted docs', () => {
                        let sortedDocs = r.tfidf(this.query, this.documents, true);
                        expect(sortedDocs).to.eql(this.expectedSortedDocs);
                    });
                });
            });


            describe('when file paths are passed', () => {
                describe('when filterStopWords is not passed', () => {
                    it('returns the correct sorted docs', () => {
                        let sortedDocs = r.tfidf(this.queryFilePath, this.documentsFilePath);
                        expect(sortedDocs).to.eql(this.expectedSortedDocs);
                    });
                });

                describe('when filterStopWords is passed', () => {
                    it('returns the correct sorted docs', () => {
                        let sortedDocs = r.tfidf(this.query, this.documents, true);
                        expect(sortedDocs).to.eql(this.expectedSortedDocs);
                    });
                });
            });


            describe('when we pass callback', () => {
                describe('when filterStopWords is not passed', () => {
                    it('returns the correct sorted docs', (done) => {
                        let $this = this;
                        r.tfidf(this.queryFilePath, this.documentsFilePath, (sortedDocs) => {
                            expect(sortedDocs).to.eql($this.expectedSortedDocs);
                            done();
                        });
                    });
                });

                describe('when filterStopWords is passed', () => {
                    it('returns the correct sorted docs', (done) => {
                        let $this = this;
                        r.tfidf(this.query, this.documents, true, (sortedDocs) => {
                            expect(sortedDocs).to.eql($this.expectedSortedDocs);
                            done();
                        });
                    });
                });
            });
        });


        context('when invalid params are sent', () => {
            it('throws error', () => {
                expect(r.tfidf).to.throw('Invalid params');
            });
            it('throws error', () => {
                expect(r.tfidf.bind(this, this.queryFilePath)).to.throw('Invalid params');
            });
            it('throws error', () => {
                expect(r.tfidf.bind(this, this.documentsPath)).to.throw('Invalid params');
            });
        });


        context('when query does not exist in documents', () => {
            it('returns the original docuemnts without sorting', (done) => {
                let $this = this;
                r.tfidf('invalid query', this.documents, (sortedDocs) => {
                    expect(sortedDocs).to.eql($this.documents);
                    done();
                });
            });
        });


        context('when query is empty', () => {
            it('returns empty array', (done) => {
                let $this = this;
                r.tfidf('', this.documents, (sortedDocs) => {
                    expect(sortedDocs).to.eql([]);
                    done();
                });
            });
        });


        context('when documents is empty array', () => {
            it('returns empty array', (done) => {
                let $this = this;
                r.tfidf(this.query, [], (sortedDocs) => {
                    expect(sortedDocs).to.eql([]);
                    done();
                });
            });
        });


        context('when documents are empty', () => {
            it('returns the original docs', (done) => {
                let $this = this;
                let emptyDocuments = ['', '', '', '', ''];
                r.tfidf(this.query, emptyDocuments, (sortedDocs) => {
                    expect(sortedDocs).to.eql(emptyDocuments);
                    done();
                });
            });
        });
    });


    context('getRatingPrediction', () => {
        beforeEach(() => {
            this.ratings = [
                [4, 0, 0, 1, 1, 0, 0],
                [5, 5, 4, 0, 0, 0, 0],
                [0, 0, 0, 2, 4, 5, 0],
                [3, 0, 0, 0, 0, 0, 3]
            ];
            this.row = 0;
            this.col = 4;

            this.expectedRatingPrediction = 4;
        });

        context('when correct params are sent', () => {

        });


        context('when invalid params are sent', () => {
            
        });
    });
});