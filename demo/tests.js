"use strict";

var assert = require('assert');
var mocha = require('mocha');
var chai = require('chai');
var expect = chai.expect;
var r = require('recommender');

const LONG_TIMEOUT = 30000;
function generateMatrix(rows, cols) {
    let matrix = [];
    for (let i = 0; i < rows; i++) {
        let row = [];
        for (let j = 0; j < cols; j++) {
            row.push(Math.floor(Math.random() * (5 - 1 + 1)) + 1);
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
            it('returns correct result', () => {
                let ratingPrediction = r.getRatingPrediction(this.ratings, this.row, this.col);
                expect(ratingPrediction).to.eql(this.expectedRatingPrediction);
            });

            describe('sparse matrix', () => {
                it('returns correct result', () => {
                    let ratingPrediction = r.getRatingPrediction([
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0]
                    ], this.row, this.col);
                    expect(ratingPrediction).to.eql(0);
                });
            });

            describe('medium matrix', () => {
                it('returns correct result', (done) => {
                    let ratingPrediction = r.getRatingPrediction(generateMatrix(5000, 5000), this.row, this.col);
                    expect(ratingPrediction >= 0).to.be.true;
                    done();
                }).timeout(LONG_TIMEOUT);
            });

            describe('large matrix', () => {
                it('returns correct result', (done) => {
                    let ratings = generateMatrix(300, 100000);
                    let ratingPrediction = r.getRatingPrediction(ratings, this.row, this.col);
                    expect(ratingPrediction >= 0).to.be.true;
                    done();
                }).timeout(LONG_TIMEOUT);
            });

            describe('async', () => {
                it('returns correct result', (done) => {
                    let $this = this;
                    r.getRatingPrediction(this.ratings, this.row, this.col, (ratingPrediction) => {
                        expect(ratingPrediction).to.eql($this.expectedRatingPrediction);
                        done();
                    });
                }).timeout(LONG_TIMEOUT);
            });
        });


        context('when invalid params are sent', () => {
            describe('when ratings are invalid', () => {
                it('return 0', () => {
                    let ratingPrediction = r.getRatingPrediction(false, this.row, this.col);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', (done) => {
                    r.getRatingPrediction(false, this.row, this.col, (ratingPrediction) => {
                        expect(ratingPrediction).to.eql(0);
                        done();
                    });
                }).timeout(LONG_TIMEOUT);
            });

            describe('when row is invalid', () => {
                it('return 0', () => {
                    let ratingPrediction = r.getRatingPrediction(this.ratings, -1, this.col);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', () => {
                    let ratingPrediction = r.getRatingPrediction(this.ratings, 123123, this.col);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', () => {
                    let ratingPrediction = r.getRatingPrediction(this.ratings, null, this.col);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', (done) => {
                    r.getRatingPrediction(this.ratings, null, this.col, (ratingPrediction) => {
                        expect(ratingPrediction).to.eql(0);
                        done();
                    });
                });
            });

            describe('when col is invalid', () => {
                it('return 0', () => {
                    let ratingPrediction = r.getRatingPrediction(this.ratings, this.row, -1);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', () => {
                    let ratingPrediction = r.getRatingPrediction(this.ratings, this.row, 1231231);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', () => {
                    let ratingPrediction = r.getRatingPrediction(this.ratings, this.row);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', (done) => {
                    r.getRatingPrediction(this.ratings, this.row, null, (ratingPrediction) => {
                        expect(ratingPrediction).to.eql(0);
                        done();
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
            it('returns correct result', () => {
                let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, this.row, this.col);
                expect(ratingPrediction).to.eql(this.expectedRatingPrediction);
            });

            describe('sparse matrix', () => {
                it('returns correct result', () => {
                    let ratingPrediction = r.getGlobalBaselineRatingPrediction([
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0],
                        [0, 0, 0, 0, 0, 0, 0]
                    ], this.row, this.col);
                    
                    expect(ratingPrediction).to.eql(0);
                });
            });

            describe('medium matrix', () => {
                it('returns correct result', (done) => {
                    let ratingPrediction = r.getGlobalBaselineRatingPrediction(generateMatrix(5000, 5000), this.row, this.col);
                    expect(ratingPrediction >= 0).to.be.true;
                    done();
                }).timeout(LONG_TIMEOUT);
            });

            describe('large matrix', () => {
                it('returns correct result', (done) => {
                    let ratings = generateMatrix(300, 100000);
                    let ratingPrediction = r.getGlobalBaselineRatingPrediction(ratings, this.row, this.col);
                    expect(ratingPrediction >= 0).to.be.true;
                    done();
                }).timeout(LONG_TIMEOUT);
            });

            describe('async', () => {
                it('returns correct result', (done) => {
                    let $this = this;
                    r.getGlobalBaselineRatingPrediction(this.ratings, this.row, this.col, (ratingPrediction) => {
                        expect(ratingPrediction).to.eql($this.expectedRatingPrediction);
                        done();
                    });
                }).timeout(LONG_TIMEOUT);
            });
        });


        context('when invalid params are sent', () => {
            describe('when ratings are invalid', () => {
                it('return 0', () => {
                    let ratingPrediction = r.getGlobalBaselineRatingPrediction(false, this.row, this.col);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', (done) => {
                    r.getGlobalBaselineRatingPrediction(false, this.row, this.col, (ratingPrediction) => {
                        expect(ratingPrediction).to.eql(0);
                        done();
                    });
                }).timeout(LONG_TIMEOUT);
            });

            describe('when row is invalid', () => {
                it('return 0', () => {
                    let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, -1, this.col);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', () => {
                    let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, 123123, this.col);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', () => {
                    let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, null, this.col);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', (done) => {
                    r.getGlobalBaselineRatingPrediction(this.ratings, null, this.col, (ratingPrediction) => {
                        expect(ratingPrediction).to.eql(0);
                        done();
                    });
                });
            });

            describe('when col is invalid', () => {
                it('return 0', () => {
                    let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, this.row, -1);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', () => {
                    let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, this.row, 1231231);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', () => {
                    let ratingPrediction = r.getGlobalBaselineRatingPrediction(this.ratings, this.row);
                    expect(ratingPrediction).to.eql(0);
                });

                it('return 0', (done) => {
                    r.getGlobalBaselineRatingPrediction(this.ratings, this.row, null, (ratingPrediction) => {
                        expect(ratingPrediction).to.eql(0);
                        done();
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
              { itemId: 1, rating: 5 },
              { itemId: 5, rating: 5 },
              { itemId: 2, rating: 4 }
            ];
        });

        context('when correct params are sent', () => {
            it('returns correct result', () => {
                let recommendations = r.getTopCFRecommendations(this.ratings, this.row, this.limit);
                expect(recommendations).to.eql(this.expectedTopRecommendations);
            });

            context('sparse matrix', () => {
                let ratings = [
                    [0, 0, 0, 0, 0, 0, 0],
                    [0, 0, 0, 0, 0, 0, 0],
                    [0, 0, 0, 0, 0, 0, 0],
                    [0, 0, 0, 0, 0, 0, 0]
                ];
                let recommendations = r.getTopCFRecommendations(this.ratings, this.row, this.limit);
                expect(recommendations).to.eql([]);
            });

            describe('medium matrix', () => {
                it('returns correct result', (done) => {
                    let recommendations = r.getTopCFRecommendations(generateMatrix(5000, 5000), this.row, this.limit);
                    expect(recommendations.length >= 0).to.be.true;
                    done();
                }).timeout(LONG_TIMEOUT);
            });

            describe('large matrix', () => {
                it('returns correct result', (done) => {
                    let recommendations = r.getTopCFRecommendations(generateMatrix(300, 10000), this.row, this.limit);
                    expect(recommendations.length >= 0).to.be.true;
                    done();
                }).timeout(LONG_TIMEOUT);
            });

            describe('async', () => {
                it('returns correct result', (done) => {
                    let $this = this;
                    r.getTopCFRecommendations(this.ratings, this.row, this.limit, (recommendations) => {
                        expect(recommendations).to.eql($this.expectedTopRecommendations);
                        done();
                    });
                }).timeout(LONG_TIMEOUT);
            });

            describe('with custom limit', () => {
                it('returns correct result within limit', (done) => {
                    let $this = this;
                    r.getTopCFRecommendations(this.ratings, this.row, 3, (recommendations) => {
                        expect(recommendations.length === 3).to.be.true;
                        done();
                    });
                }).timeout(LONG_TIMEOUT);
            });
        });

        context('when invalid params are sent', () => {
            describe('when ratings are invalid', () => {
                it('return 0', () => {
                    let recommendations = r.getTopCFRecommendations(false, this.row, this.limit);
                    expect(recommendations).to.eql([]);
                });

                it('return 0', (done) => {
                    r.getTopCFRecommendations(false, this.row, this.limit, (recommendations) => {
                        expect(recommendations).to.eql([]);
                        done();
                    });
                }).timeout(LONG_TIMEOUT);
            });

            describe('when row is invalid', () => {
                it('return 0', () => {
                    let recommendations = r.getTopCFRecommendations(this.ratings, -1, this.limit);
                    expect(recommendations).to.eql([]);
                });

                it('return 0', () => {
                    let recommendations = r.getTopCFRecommendations(this.ratings, 123123, this.limit);
                    expect(recommendations).to.eql([]);
                });

                it('return 0', () => {
                    let recommendations = r.getTopCFRecommendations(this.ratings, null, this.limit);
                    expect(recommendations).to.eql([]);
                });

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