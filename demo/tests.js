"use strict";

var assert = require('assert');
var mocha = require('mocha');
var chai = require('chai');
var expect = chai.expect;
var r = require('recommender');
var largeRatingsTable = require('./resources/large_ratings_table.json');

const LONG_TIMEOUT = 1.2e+6; // 20 minutes
function generateMatrix(rows, cols) {
    let matrix = [];
    for (let i = 0; i < rows; i++) {
        let row = [];
        for (let j = 0; j < cols; j++) {
            let rand = Math.floor(Math.random() * (5 - 1 + 1)) + 1;
            if (rand == 1 && (Math.floor(Math.random() * (5 - 1 + 1)) + 1) <= 3) rand = 0;
            row.push(rand);
        }
        matrix.push(row);
    }

    return matrix;
}

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
                context('sync', () => {
                    describe('when filterStopWords is not passed', () => {
                        it('returns the correct sorted docs', () => {
                            let sortedDocs = r.tfidf(this.query, this.documents);
                            expect(sortedDocs).to.eql(this.expectedSortedDocs);
                        });
                    });
                });

                context('async', () => {
                    describe('when filterStopWords is not passed', () => {
                        it('returns the correct sorted docs', (done) => {
                            let $this = this;
                            r.tfidf(this.query, this.documents, (sortedDocs) => {
                                expect(sortedDocs).to.eql($this.expectedSortedDocs);
                                done();
                            });
                        });
                    });
                });

                context('sync', () => {
                    describe('when filterStopWords is passed', () => {
                        it('returns the correct sorted docs', () => {
                            let sortedDocs = r.tfidf(this.query, this.documents, true);
                            expect(sortedDocs).to.eql(this.expectedSortedDocs);
                        });
                    });
                });

                context('async', () => {
                    describe('when filterStopWords is passed', () => {
                        it('returns the correct sorted docs', (done) => {
                            let $this = this;
                            r.tfidf(this.query, this.documents, (sortedDocs) => {
                                expect(sortedDocs).to.eql($this.expectedSortedDocs);
                                done();
                            });
                        });
                    });
                });
            });

            describe('when file paths are passed', () => {
                context('sync', () => {
                    describe('when filterStopWords is not passed', () => {
                        it('returns the correct sorted docs', () => {
                            let sortedDocs = r.tfidf(this.query, this.documents, true);
                            expect(sortedDocs).to.eql(this.expectedSortedDocs);
                        });
                    });
                });

                context('async', () => {
                    describe('when filterStopWords is not passed', () => {
                        it('returns the correct sorted docs', (done) => {
                            let $this = this;
                            r.tfidf(this.query, this.documents, (sortedDocs) => {
                                expect(sortedDocs).to.eql($this.expectedSortedDocs);
                                done();
                            });
                        });
                    });
                });

                context('sync', () => {
                    describe('when filterStopWords is passed', () => {
                        it('returns the correct sorted docs', () => {
                            let sortedDocs = r.tfidf(this.query, this.documents, true);
                            expect(sortedDocs).to.eql(this.expectedSortedDocs);
                        });
                    });
                });

                context('async', () => {
                    describe('when filterStopWords is passed', () => {
                        it('returns the correct sorted docs', (done) => {
                            r.tfidf(this.query, this.documents, true, (sortedDocs) => {
                                expect(sortedDocs).to.eql(this.expectedSortedDocs);
                                done();
                            });
                        });
                    });
                });
            });

            describe('when we pass callback', () => {
                context('when filterStopWords is not passed', () => {
                    it('returns the correct sorted docs', (done) => {
                        let $this = this;
                        r.tfidf(this.queryFilePath, this.documentsFilePath, (sortedDocs) => {
                            expect(sortedDocs).to.eql($this.expectedSortedDocs);
                            done();
                        });
                    });
                });

                context('when filterStopWords is passed', () => {
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
            describe('when nothing is passed', () => {
                it('throws error', () => {
                    expect(r.tfidf).to.throw('Invalid params');
                });
            });

            describe('when documents are not passed', () => {
                it('throws error', () => {
                    expect(r.tfidf.bind(this, this.queryFilePath)).to.throw('Invalid params');
                });
            });

            describe('when query is null', () => {
                it('throws error', () => {
                    expect(r.tfidf.bind(this, null, this.documentsPath)).to.throw('Invalid params');
                });
            });
            
            describe('when documents is null', () => {
                it('throws error', () => {
                    expect(r.tfidf.bind(this, this.query, null)).to.throw('Invalid params');
                });
            });
        });


        context('when query does not exist in documents', () => {
            describe('sync', () => {
                it('returns the original docuemnts without sorting', () => {
                    let $this = this;
                    let sortedDocs = r.tfidf('invalid query', this.documents);
                    expect(sortedDocs).to.eql($this.documents);
                });
            });
            
            describe('async', () => {
                it('returns the original docuemnts without sorting', (done) => {
                    let $this = this;
                    r.tfidf('invalid query', this.documents, (sortedDocs) => {
                        expect(sortedDocs).to.eql($this.documents);
                        done();
                    });
                });
            });
        });


        context('when documents is empty array', () => {
            describe('sync', () => {
                it('returns empty array', () => {
                    let $this = this;
                    let sortedDocs = r.tfidf(this.query, []);
                    expect(sortedDocs).to.eql([]);
                });
            });

            describe('async', () => {
                it('returns empty array', (done) => {
                    let $this = this;
                    r.tfidf(this.query, [], (sortedDocs) => {
                        expect(sortedDocs).to.eql([]);
                        done();
                    });
                });
            });
        });


        context('when documents are empty', () => {
            context('sync', () => {
                it('returns the original docs', () => {
                    let $this = this;
                    let emptyDocuments = ['', '', '', '', ''];
                    let sortedDocs = r.tfidf(this.query, emptyDocuments);
                    expect(sortedDocs).to.eql(emptyDocuments);
                });
            });

            context('async', () => {
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
            describe('when the example matrix is used', () => {
                context('sync', () => {
                    it('returns correct result', () => {
                        let ratingPrediction = r.getRatingPrediction(this.ratings, this.row, this.col);
                        expect(ratingPrediction).to.eql(this.expectedRatingPrediction);
                    });
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getRatingPrediction(this.ratings, this.row, this.col, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(this.expectedRatingPrediction);
                            done();
                        });
                    });
                });
            });

            describe('sparse matrix', () => {
                beforeEach(() => {
                    this.sparseMatrix = [
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0]
                    ];
                });

                context('sync', () => {
                    it('returns correct result', () => {
                        let ratingPrediction = r.getRatingPrediction(this.sparseMatrix, this.row, this.col);
                        expect(ratingPrediction).to.eql(0);
                    });
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getRatingPrediction(this.sparseMatrix, this.row, this.col, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(0);
                            done();
                        });
                    });
                });
            });

            describe('medium matrix', () => {
                context('sync', () => {
                    it('returns correct result', () => {
                        let ratingPrediction = r.getRatingPrediction(generateMatrix(5000, 5000), this.row, this.col);
                        expect(ratingPrediction > 0).to.be.true;
                    }).timeout(LONG_TIMEOUT);
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getRatingPrediction(generateMatrix(5000, 5000), this.row, this.col, (ratingPrediction) => {
                            expect(ratingPrediction > 0).to.be.true;
                            done();
                        });
                    }).timeout(LONG_TIMEOUT);
                });
            });

            describe('large matrix', () => {
                context('sync', () => {
                    it('returns the correct result', () => {
                        let ratings = generateMatrix(300, 100000);
                        let ratingPrediction = r.getRatingPrediction(ratings, this.row, this.col);
                        expect(ratingPrediction > 0).to.be.true;
                    }).timeout(LONG_TIMEOUT);
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        let ratings = generateMatrix(300, 100000);
                        r.getRatingPrediction(ratings, this.row, this.col, (ratingPrediction) => {
                            expect(ratingPrediction > 0).to.be.true;
                            done();
                        });
                    }).timeout(LONG_TIMEOUT);
                });
            });

            describe('very large matrix', () => {
                context('sync', () => {
                    it('returns the correct result', () => {
                        let ratingPrediction = r.getRatingPrediction(largeRatingsTable, this.row, 31);
                        expect(ratingPrediction > 0).to.be.true;
                    }).timeout(LONG_TIMEOUT);
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getRatingPrediction(largeRatingsTable, this.row, 31, (ratingPrediction) => {
                            expect(ratingPrediction > 0).to.be.true;
                            done();
                        });
                    }).timeout(LONG_TIMEOUT);
                });
            });
        });


        context('when invalid params are sent', () => {
            describe('when ratings are invalid', () => {
                context('sync', () => {
                    it('return 0', () => {
                        let ratingPrediction = r.getRatingPrediction(false, this.row, this.col);
                        expect(ratingPrediction).to.eql(0);
                    });
                });

                context('async', () => {
                    it('return 0', (done) => {
                        r.getRatingPrediction(false, this.row, this.col, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(0);
                            done();
                        });
                    });
                });
            });

            describe('when row is invalid', () => {
                context('row is negative', () => {
                    context('sync', () => {
                        it('return 0', () => {
                            let ratingPrediction = r.getRatingPrediction(this.ratings, -1, this.col);
                            expect(ratingPrediction).to.eql(0);
                        });
                    });

                    context('async', () => {
                        it('return 0', (done) => {
                            r.getRatingPrediction(this.ratings, -1, this.col, (ratingPrediction) => {
                                expect(ratingPrediction).to.eql(0);
                                done();
                            });
                        });
                    });
                });
                
                context('row is outside of matrix', () => {
                    context('sync', () => {
                        it('return 0', () => {
                            let ratingPrediction = r.getRatingPrediction(this.ratings, 123123, this.col);
                            expect(ratingPrediction).to.eql(0);
                        });
                    });

                    context('async', () => {
                        it('return 0', (done) => {
                            r.getRatingPrediction(this.ratings, 123123, this.col, (ratingPrediction) => {
                                expect(ratingPrediction).to.eql(0);
                                done();
                            });
                        });
                    });
                });
                
                context('row is null', () => {
                    context('sync', () => {
                        it('return 0', () => {
                            let ratingPrediction = r.getRatingPrediction(this.ratings, null, this.col);
                            expect(ratingPrediction).to.eql(0);
                        });
                    });

                    context('async', () => {
                        it('return 0', (done) => {
                            r.getRatingPrediction(this.ratings, null, this.col, (ratingPrediction) => {
                                expect(ratingPrediction).to.eql(0);
                                done();
                            });
                        });
                    });
                });
            });

            describe('when col is invalid', () => {
                context('col is negative', () => {
                    context('sync', () => {
                        it('return 0', () => {
                            let ratingPrediction = r.getRatingPrediction(this.ratings, this.row, -1);
                            expect(ratingPrediction).to.eql(0);
                        });
                    });

                    context('async', () => {
                        it('return 0', (done) => {
                            r.getRatingPrediction(this.ratings, this.row, -1, (ratingPrediction) => {
                                expect(ratingPrediction).to.eql(0);
                                done();
                            });
                        });
                    });
                });

                context('col is outside matrix', () => {
                    context('sync', () => {
                        it('return 0', () => {
                            let ratingPrediction = r.getRatingPrediction(this.ratings, this.row, 1231231);
                            expect(ratingPrediction).to.eql(0);
                        });
                    });

                    context('async', () => {
                        it('return 0', (done) => {
                            r.getRatingPrediction(this.ratings, this.row, 1231231, (ratingPrediction) => {
                                expect(ratingPrediction).to.eql(0);
                                done();
                            });
                        });
                    });
                });

                context('col is undefined', () => {
                    context('sync', () => {
                        it('return 0', () => {
                            let ratingPrediction = r.getRatingPrediction(this.ratings, this.row);
                            expect(ratingPrediction).to.eql(0);
                        });
                    });

                    context('async', () => {
                        it('return 0', (done) => {
                            r.getRatingPrediction(this.ratings, this.row, undefined, (ratingPrediction) => {
                                expect(ratingPrediction).to.eql(0);
                                done();
                            });
                        });
                    });
                });
            });
        });
    });


    context('getGlobalBaselineRatingPrediction', () => {
        beforeEach(() => {
            this.ratings = [
                [4, 0, 0, 1, 1, 0, 0],
                [5, 5, 4, 0, 0, 0, 0],
                [0, 0, 0, 2, 4, 5, 0],
                [3, 0, 0, 0, 0, 0, 3]
            ];
            this.row = 0;
            this.col = 4;

            this.expectedRatingPrediction = 1.1363636363636362;
        });

        context('when correct params are sent', () => {
            describe('when the example matrix is used', () => {
                context('sync', () => {
                    it('returns correct result', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, this.row, this.col);
                        expect(ratingPrediction).to.eql(this.expectedRatingPrediction);
                    });
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getGlobalBaselineRatingPrediction(this.ratings, this.row, this.col, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(this.expectedRatingPrediction);
                            done();
                        });
                    });
                });
            });
            
            describe('sparse matrix', () => {
                beforeEach(() => {
                    this.sparseMatrix = [
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0]
                    ];
                });

                context('sync', () => {
                    it('returns correct result', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.sparseMatrix, this.row, this.col);
                        expect(ratingPrediction).to.eql(0);
                    });
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getGlobalBaselineRatingPrediction(this.sparseMatrix, this.row, this.col, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(0);
                            done();
                        });
                    });
                });
            });

            describe('medium matrix', () => {
                context('sync', () => {
                    it('returns correct result', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(generateMatrix(5000, 5000), this.row, this.col);
                        expect(ratingPrediction > 0).to.be.true;
                    }).timeout(LONG_TIMEOUT);
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getGlobalBaselineRatingPrediction(generateMatrix(5000, 5000), this.row, this.col, (ratingPrediction) => {
                            expect(ratingPrediction > 0).to.be.true;
                            done();
                        });
                    }).timeout(LONG_TIMEOUT);
                });
            });

            describe('large matrix', () => {
                context('sync', () => {
                    it('returns correct result', () => {
                        let ratings = generateMatrix(300, 100000);
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(ratings, this.row, this.col);
                        expect(ratingPrediction > 0).to.be.true;
                    }).timeout(LONG_TIMEOUT);
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        let ratings = generateMatrix(300, 100000);
                        r.getGlobalBaselineRatingPrediction(ratings, this.row, this.col, (ratingPrediction) => {
                            expect(ratingPrediction > 0).to.be.true;
                            done();
                        });
                    }).timeout(LONG_TIMEOUT);
                });
            });

            describe('very large matrix', () => {
                context('sync', () => {
                    it('returns correct result', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(largeRatingsTable, this.row, 31);
                        expect(ratingPrediction > 0).to.be.true;
                    }).timeout(LONG_TIMEOUT);
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getGlobalBaselineRatingPrediction(largeRatingsTable, this.row, 31, (ratingPrediction) => {
                            expect(ratingPrediction > 0).to.be.true;
                            done();
                        });
                    }).timeout(LONG_TIMEOUT);
                });
            });
        });


        context('when invalid params are sent', () => {
            describe('when ratings are invalid', () => {
                context('sync', () => {
                    it('return 0', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(false, this.row, this.col);
                        expect(ratingPrediction).to.eql(0);
                    });
                });

                context('async', () => {
                    it('return 0', (done) => {
                        r.getGlobalBaselineRatingPrediction(false, this.row, this.col, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(0);
                            done();
                        });
                    });
                });
            });

            describe('when row is negative', () => {
                context('sync', () => {
                    it('return 0', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, -1, this.col);
                        expect(ratingPrediction).to.eql(0);
                    });
                });

                context('async', () => {
                    it('return 0', (done) => {
                        r.getGlobalBaselineRatingPrediction(this.ratings, -1, this.col, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(0);
                            done();
                        });
                    });
                });
            });

            describe('when row is outside matrix', () => {
                context('sync', () => {
                    it('return 0', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, 123123, this.col);
                        expect(ratingPrediction).to.eql(0);
                    });
                });

                context('async', () => {
                    it('return 0', (done) => {
                        r.getGlobalBaselineRatingPrediction(this.ratings, 123123, this.col, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(0);
                            done();
                        });
                    });
                });
            });

            describe('when row is null', () => {
                context('sync', () => {
                    it('return 0', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, null, this.col);
                        expect(ratingPrediction).to.eql(0);
                    });
                });

                context('async', () => {
                    it('return 0', (done) => {
                        r.getGlobalBaselineRatingPrediction(this.ratings, null, this.col, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(0);
                            done();
                        });
                    });
                });
            });

            describe('when col is negative', () => {
                context('sync', () => {
                    it('return 0', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, this.row, -1);
                        expect(ratingPrediction).to.eql(0);
                    });
                });

                context('async', () => {
                    it('return 0', (done) => {
                        r.getGlobalBaselineRatingPrediction(this.ratings, this.row, -1, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(0);
                            done();
                        });
                    });
                });
            });

            describe('when col is outside matrix', () => {
                context('sync', () => {
                    it('return 0', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, this.row, 123123);
                        expect(ratingPrediction).to.eql(0);
                    });
                });

                context('async', () => {
                    it('return 0', (done) => {
                        r.getGlobalBaselineRatingPrediction(this.ratings, this.row, 123123, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(0);
                            done();
                        });
                    });
                });
            });

            describe('when col is null', () => {
                context('sync', () => {
                    it('return 0', () => {
                        let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, this.row, null);
                        expect(ratingPrediction).to.eql(0);
                    });
                });

                context('async', () => {
                    it('return 0', (done) => {
                        r.getGlobalBaselineRatingPrediction(this.ratings, this.row, null, (ratingPrediction) => {
                            expect(ratingPrediction).to.eql(0);
                            done();
                        });
                    });
                });
            });
        });
    });


    context('getTopCFRecommendations', () => {
        beforeEach(() => {
            this.ratings = [
                [4, 0, 0, 1, 1, 0, 0],
                [5, 5, 4, 0, 0, 0, 0],
                [0, 0, 0, 2, 4, 5, 0],
                [3, 0, 0, 0, 0, 0, 3]
            ];
            this.row = 0;
            this.limit = 100;

            this.expectedTopRecommendations = [
              { itemId: 1, rating: 4.4907920453550085 },
              { itemId: 2, rating: 3.5926336362840074 },
              { itemId: 5, rating: 0.5092079546449908 },
              { itemId: 6, rating: 0 }
            ];
        });

        context('when correct params are sent', () => {
            context('when example matrix is used', () => {
                context('sync', () => {
                    it('returns correct result', () => {
                        let recommendations = r.getTopCFRecommendations(this.ratings, this.row, this.limit);
                        expect(recommendations).to.eql(this.expectedTopRecommendations);
                    });
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getTopCFRecommendations(this.ratings, this.row, this.limit, (recommendations) => {
                            expect(recommendations).to.eql(this.expectedTopRecommendations);
                            done();
                        });
                    });
                });
            });
            

            context('sparse matrix', () => {
                beforeEach(() => {
                    this.sparseMatrix = [
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0]
                    ];
                });

                context('sync', () => {
                    it('returns correct result', () => {
                        let recommendations = r.getTopCFRecommendations(this.sparseMatrix, this.row, this.limit);
                        expect(recommendations).to.eql([]);
                    });
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getTopCFRecommendations(this.sparseMatrix, this.row, this.limit, (recommendations) => {
                            expect(recommendations).to.eql([]);
                            done();
                        });
                    });
                });
            });

            describe('medium matrix', () => {
                context('sync', () => {
                    it('returns correct result', () => {
                        let recommendations = r.getTopCFRecommendations(generateMatrix(1000, 1000), this.row, this.limit);
                        expect(recommendations.length > 0).to.be.true;
                    }).timeout(LONG_TIMEOUT);
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getTopCFRecommendations(generateMatrix(1000, 1000), this.row, this.limit, (recommendations) => {
                            expect(recommendations.length > 0).to.be.true;
                            done();
                        });
                    }).timeout(LONG_TIMEOUT);
                });
            });

            describe('large matrix', () => {
                context('sync', () => {
                    it('returns correct result', () => {
                        let recommendations = r.getTopCFRecommendations(generateMatrix(300, 10000), this.row, this.limit);
                        expect(recommendations.length > 0).to.be.true;
                    }).timeout(LONG_TIMEOUT);
                });
                
                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getTopCFRecommendations(generateMatrix(300, 10000), this.row, this.limit, (recommendations) => {
                            expect(recommendations.length > 0).to.be.true;
                            done();
                        });
                    }).timeout(LONG_TIMEOUT);
                });
            });

            describe('very large matrix', () => {
                context('sync', () => {
                    it('returns correct result', () => {
                        let recommendations = r.getTopCFRecommendations(largeRatingsTable, this.row, this.limit);
                        expect(recommendations.length > 0).to.be.true;
                    }).timeout(LONG_TIMEOUT);
                });

                context('async', () => {
                    it('returns correct result', (done) => {
                        r.getTopCFRecommendations(largeRatingsTable, this.row, this.limit, (recommendations) => {
                            expect(recommendations.length > 0).to.be.true;
                            done();
                        });
                    }).timeout(LONG_TIMEOUT);
                });
            });

            describe('with custom limit', () => {
                context('sync', () => {
                    it('returns correct result within limit', () => {
                        let recommendations = r.getTopCFRecommendations(this.ratings, this.row, 3);
                        expect(recommendations.length === 3).to.be.true;
                    });
                });

                context('async', () => {
                    it('returns correct result within limit', (done) => {
                        r.getTopCFRecommendations(this.ratings, this.row, 3, (recommendations) => {
                            expect(recommendations.length === 3).to.be.true;
                            done();
                        });
                    });
                });
            });
        });

        context('when invalid params are sent', () => {
            describe('when ratings are invalid', () => {
                context('sync', () => {
                    it('return 0', () => {
                        let recommendations = r.getTopCFRecommendations(false, this.row, this.limit);
                        expect(recommendations).to.eql([]);
                    });
                });

                context('async', () => {
                    it('return 0', (done) => {
                        r.getTopCFRecommendations(false, this.row, this.limit, (recommendations) => {
                            expect(recommendations).to.eql([]);
                            done();
                        });
                    });
                });
                

                it('return 0', (done) => {
                    r.getTopCFRecommendations(false, this.row, this.limit, (recommendations) => {
                        expect(recommendations).to.eql([]);
                        done();
                    });
                }).timeout(LONG_TIMEOUT);
            });

            describe('when row is invalid', () => {
                context('sync', () => {
                    it('return 0', () => {
                        let recommendations = r.getTopCFRecommendations(this.ratings, -1, this.limit);
                        expect(recommendations).to.eql([]);
                    });
                });

                context('async', () => {
                    it('return 0', (done) => {
                        r.getTopCFRecommendations(this.ratings, -1, this.limit, (recommendations) => {
                            expect(recommendations).to.eql([]);
                            done();
                        });
                    });
                });

                context('row is outside matrix', () => {
                    context('sync', () => {
                        it('return 0', () => {
                            let recommendations = r.getTopCFRecommendations(this.ratings, 123123, this.limit);
                            expect(recommendations).to.eql([]);
                        });
                    });

                    context('async', () => {
                        it('return 0', (done) => {
                            r.getTopCFRecommendations(this.ratings, 123123, this.limit, (recommendations) => {
                                expect(recommendations).to.eql([]);
                                done();
                            });
                        });
                    });
                });

                context('row is null', () => {
                    context('sync', () => {
                        it('return 0', () => {
                            let recommendations = r.getTopCFRecommendations(this.ratings, null, this.limit);
                            expect(recommendations).to.eql([]);
                        });
                    });

                    context('async', () => {
                        it('return 0', (done) => {
                            r.getTopCFRecommendations(this.ratings, null, this.limit, (recommendations) => {
                                expect(recommendations).to.eql([]);
                                done();
                            });
                        });
                    });
                });
            });
        });
    });
});