# Recommender
`recommender` is a node addon with utility functions, which can help when building a recommender system. It contains implementations of [`tf-idf`](https://en.wikipedia.org/wiki/Tf%E2%80%93idf), [`Collaborative Filtering`](https://en.wikipedia.org/wiki/Collaborative_filtering) and `Global Baseline Approach` which are commonly used in recommendation systems.

[![NPM](https://nodei.co/npm/recommender.png?downloads=true&downloadRank=true)](https://nodei.co/npm/recommender/)

[![Build Status](https://travis-ci.org/D-Andreev/recommender-addon.svg?branch=master)](https://travis-ci.org/D-Andreev/recommender-addon) [![NPM version](https://badge.fury.io/js/badge-list.svg)](https://www.npmjs.com/package/recommender) [![Open Source Love](https://badges.frapsoft.com/os/mit/mit.svg?v=102)](https://github.com/D-Andreev/recommender-addon/blob/master/LICENSE)

 * **[Installation](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#installation)**
 * **[Usage](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#usage)**
 * **[API](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#api)**
 * **[Run examples and benchmarks](https://github.com/D-Andreev/recommender-addon/blob/28ba4c3f5fdb3b66215b6f3420d568c592e8c9a3/README.md#run-examples-and-benchmarks)**
 * **[Contributing](https://github.com/D-Andreev/recommender-addon/blob/28ba4c3f5fdb3b66215b6f3420d568c592e8c9a3/README.md#contributing)**
 * **[License](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#license)**

## Installation
 - `npm i recommender`

## Usage

### TF-IDF
The input of TF-IDF is a search query and a collection of documents. It finds how important a word is to a document in a collection or corpus. Then using cosine similarity we can get the most similar documents to the search query and make recommendations.
```js
var recommender = require('recommender');

var query = 'get current date time javascript';
var documents = [
    'get the current date and time in javascript',
    'get the current date and time in python',
    'something very different',
    'what is the time now'
];
var weights = recommender.tfidf(query, documents);
var recommendations = recommender.recommend();
var sortedDocs = recommender.getSortedDocs();
console.log(sortedDocs);
// Output:
/**
    get the current date and time in javascript
    get the current date and time in python
	what is the time now
    something very different
*/
```
The `tfidf` method also accepts paths to the files where our query and documents are. We can create a text file for the query - `search_query.txt` with content:
```
get current date time javascript
```
And this into `documents.txt`. Every document is seperated by a new line and every word is separated by space.
```
get the current date and time in javascript
get the current date and time in python
something very different
what is the time now
```
```js
var recommender = require('recommender');

var queryPath = './search_query.txt';
var documentsPath = './documents.txt';

var weights = recommender.tfidf(queryPath, documentsPath);
var recommendations = recommender.recommend();
var sortedDocs = recommender.getSortedDocs();
console.log(sortedDocs);
// Output:
/**
    get the current date and time in javascript
    get the current date and time in python
	what is the time now
    something very different
*/
```

We can also pass `filterStopWords` which is optional and `false` by default. If `filterStopWords` is `true` those words will be filtered out and not considered when calculating similarity. Stop-words are those words that appear very commonly across the documents, therefore loosing their representativeness and don't contribute to the meaning of the text. i.e (`a`, `about`, `the`, `if`, `it`, `is`...). The full stop words list can be viewed [here](https://github.com/D-Andreev/recommender-addon/blob/master/include/Constants.h#L8).
```js
bool filterStopWords = true;
var weights = recommender.tfidf(queryPath, documentsPath, filterStopWords);
```

### Collaborative filtering
The input for collaborative filtering is a table with user ratings. Each row is an item and each column is a user. Consider the following table with ratings of movies. `U01,U02,U03...U13` are users and `M01,M02,M03...M6` are movies. A rating of `0` means that the user has not rated the movie. In this example ratings range from `1` to `5`, but they can be in any system (i.e. 1-10).
```
        U01   U02   U03   U04   U05   U06   U07   U08   U10   U11   U12   U13
   M1   1     0     3     0     0     5     0     0     5     0     4     0
   M2   0     0     5     4     0     0     4     0     0     2     1     3
   M3   2     4     0     1     2     0     3     0     4     3     5     0
   M4   0     2     4     0     5     0     0     4     0     0     2     0
   M5   0     0     4     3     4     2     0     0     0     0     2     5
   M6   1     0     3     0     3     0     0     2     0     0     4     0
```
Aside from the ratings table we need to pass the row index and column index for the rating we wish to predict.
```js
var recommender = require('recommender');
var ratings = [
    [ 1, 0, 3, 0, 0, 5, 0, 0, 5, 0, 4, 0 ],
	[ 0, 0, 5, 4, 0, 0, 4, 0, 0, 2, 1, 3 ],
	[ 2, 4, 0, 1, 2, 0, 3, 0, 4, 3, 5, 0 ],
	[ 0, 2, 4, 0, 5, 0, 0, 4, 0, 0, 2, 0 ],
	[ 0, 0, 4, 3, 4, 2, 0, 0, 0, 0, 2, 5 ],
	[ 1, 0, 3, 0, 3, 0, 0, 2, 0, 0, 4, 0 ]
];
var movieIndex = 0;
var userIndex = 4;
// We are predicting the rating of U05 for M1.
var predictedRating = recommender.getRatingPrediction(ratings, movieIndex, userIndex);
console.log(predictedRating);
// Output: 2.586406866934817
```
There are pros and cons of using only the collaborative filtering method to predict ratings.
* Pros
    * Works for any kind of items (i.e. movies, songs, books etc..). No need for feature selection, you just need a matrix with ratings.
* Cons
    * [`Cold start problem`](https://en.wikipedia.org/wiki/Cold_start). At the beginning of your recommendation system no users have rated anything yet, your utility matrix will be empty, so no ratings can be predicted.
    * `Sparsity`. Most of the times your user/ratings matrix is very sparse. There are many items and many users, but users have only rated several items, so it's hard to find users that have rated the same items.
    * `First rater problem`. When a new item is added, there are no ratings for it yet, so it can't be recommended to anyone.
    * `Popularity bias`. Also known as [The Harry Potter Effect.](http://recsyswiki.com/wiki/Harry_Potter_effect) The most popular items are recommended the most. So for example a movie like `Harry Potter` is a very popular item and it is recommended to a lot of people, but it is not that interesting and clouds some of the unique recommendations which could be shown to a user.
Genrally when building recommender systems, for more exact results it is best to use a [Hybrid recommender system](https://en.wikipedia.org/wiki/Recommender_system#Hybrid_recommender_systems), instead off just using only collaborative filtering or only content based filtering.
### Global Baseline Approach
This approach is quite useful when your ratings table is sparse, and there aren't users who rated the same item. Typically with collaborative filtering you would need other users, that rated the same item.
Consider the following utility matrix with ratings:
```
       HP1   HP2   HP3   TW   SW1   SW2   SW3
   A   4     0     0     1     1     0     0
   B   5     5     4     0     0     0     0
   C   0     0     0     2     4     5     0
   D   3     0     0     0     0     0     3
```
`A`, `B`, `C` and `D` are users. `HP1` (Harry Potter 1), `TW` (Twilight), `SW1` (Star Wars 1) are movies. The predicted rating of user `A` for `HP2`, using the global baseline approach is `3.0909090909090913`. Results using this method tend to give predicted ratings a bit lower than collaborative filtering.
```js
var recommender = require('recommender');
var ratings = [
    [ 4, 0, 0, 1, 1, 0, 0 ],
	[ 5, 5, 4, 0, 0, 0, 0 ],
	[ 0, 0, 0, 2, 4, 5, 0 ],
	[ 3, 0, 0, 0, 0, 0, 3 ]
];
var userIndex = 0;
var movieIndex = 1;
// We are predicting the rating of A for HP2.
var predictedRating = recommender.getGlobalBaselineRatingPrediction(ratings, userIndex, movieIndex);
console.log(predictedRating);
// Output: 3.0909090909090913
```

### API
* **[recommender.tfidf(`query`, `documents`, `useStopWords`)](https://github.com/D-Andreev/recommender-addon/blob/2a17c6b0f95023381710854c1544242362cd7868/README.md#recommendertfidfquery-documents-usestopwords)**
* **[recommender.tfidf(`searchQueryFilePath`, `documentsFilePath`, `useStopWords`)](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#recommendertfidfsearchqueryfilepath-documentsfilepath-usestopwords)**
* **[recommender.recommend()](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#recommenderrecommend)**
* **[recommender.getSortedDocs()](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#recommendergetsorteddocs)**
* **[recommender.getRatingPrediction(`ratings`, `rowIndex`, `colIndex`)](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#recommendergetratingpredictionratings-rowindex-colindex)**
* **[recommender.getGlobalBaselineRatingPrediction(`ratings`, `rowIndex`, `colIndex`)](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#recommendergetglobalbaselineratingpredictionratings-rowindex-colindex)**

##### recommender.tfidf(`query`, `documents`, `useStopWords`)
###### Arguments
* `query` - A string with the query. *(Required)*
* `documents` - An array of strings with the documents. *(Required)*
* `filterStopWords` - A boolean to filter out the stop words or not. *(Optional)* *(Default: `false`)*
###### Returns
An object with each of the terms in the search query as keys and a float number with the weight of the term.
```js
{
    current: 0.3386294361119891,
    date: 0.3386294361119891,
    get: 0.3386294361119891,
    javascript: 0.47725887222397817,
    time: 0.2575364144903562
}
```
###### Examples
```js
var recommender = require('recommender');

var query = "get current date time javascript";
var documents = [
    'get the current date and time in javascript',
    'get the current date and time in python',
    'something very different',
    'what is the time now'
];
bool filterStopWords = true;
var weights = recommender.tfidf(query, documents, filterStopWords);
```
##### recommender.tfidf(`queryFilePath`, `documentsFilePath`, `useStopWords`)
###### Arguments
* `queryFilePath` - A string with the file path to the search query text file. *(Required)*
* `documentsFilePath` - A string with the file path to the documents text file. *(Required)*
* `filterStopWords` - A boolean to filter out the stop words or not. *(Optional)* *(Default: `false`)*
###### Returns
An object with each of the terms in the search query as keys and a float number with the weight of the term.
```js
{
    current: 0.3386294361119891,
    date: 0.3386294361119891,
    get: 0.3386294361119891,
    javascript: 0.47725887222397817,
    time: 0.2575364144903562
}
```
###### Examples
```js
var recommender = require('recommender');

var queryFilePath = './search_query.txt';
var documentsFilePath = './documents.txt';
bool filterStopWords = true;
var weights = recommender.tfidf(queryFilePath, documentsFilePath, filterStopWords);
```
##### recommender.recommend()
###### Returns
An array with float point numbers representing the similarities. Every index corresponds to the index of the document in `documents.txt`.
```js
[1, 0.80190163009065796, 0, 0.32239672715496848]
```
##### recommender.getSortedDocs()
###### Returns
An array of strings with the sorted by similarity documents.
```js
[
    'get the current date and time in javascript',
    'get the current date and time in python',
    'what is the time now',
    'something very different'
]
```
###### Examples
```js
var recommender = require('recommender');

var searchQueryPath = "./search_query.txt";
var documentsPath = "./documents.txt";

var weights = recommender.tfidf(searchQueryPath, documentsPath);
var recommendations = recommender.recommend();
var sortedDocs = recommender.getSortedDocs();
```
##### recommender.getRatingPrediction(`ratings`, `rowIndex`, `colIndex`)
###### Arguments
* `ratings` - A two dimensional array with numbers representing the ratings. *(Required)*
* `rowIndex` - An integer with the index of the target row for prediction. *(Required)*
* `colIndex` - An integer with the index of the target column for prediction. *(Required)*
###### Returns
A float number with the predicted rating.
###### Examples
```js
var recommender = require('recommender');

var ratings = [
    [ 1, 0, 3, 0, 0, 5, 0, 0, 5, 0, 4, 0 ],
	[ 0, 0, 5, 4, 0, 0, 4, 0, 0, 2, 1, 3 ],
	[ 2, 4, 0, 1, 2, 0, 3, 0, 4, 3, 5, 0 ],
	[ 0, 2, 4, 0, 5, 0, 0, 4, 0, 0, 2, 0 ],
	[ 0, 0, 4, 3, 4, 2, 0, 0, 0, 0, 2, 5 ],
	[ 1, 0, 3, 0, 3, 0, 0, 2, 0, 0, 4, 0 ]
];
var rowIndex = 0;
var colIndex = 4;
var predictedRating = recommender.getRatingPrediction(ratings, rowIndex, colIndex); // 2.586406866934817
```
##### recommender.getGlobalBaselineRatingPrediction(`ratings`, `rowIndex`, `colIndex`)
###### Arguments
* `ratings` - A two dimensional array with numbers representing the ratings. *(Required)*
* `rowIndex` - An integer with the index of the target row for prediction. *(Required)*
* `colIndex` - An integer with the index of the target column for prediction. *(Required)*
###### Returns
A float number with the predicted rating.
###### Examples
```js
var recommender = require('recommender');
var ratings = [
    [ 4, 0, 0, 1, 1, 0, 0 ],
    [ 5, 5, 4, 0, 0, 0, 0 ],
    [ 0, 0, 0, 2, 4, 5, 0 ],
    [ 3, 0, 0, 0, 0, 0, 3 ]
];
var userIndex = 0;
var movieIndex = 1;
var predictedRating = recommender.getGlobalBaselineRatingPrediction(ratings, userIndex, movieIndex);
// Output: 3.0909090909090913
```
### Run examples and benchmarks
- Clone the repo.
- `npm i` in the main folder.
- `npm i` in `/demo` folder.
- `node index.js` to run the examples.
- `node benchmarks.js` to run the benchmarks.

Can be viewed [here](https://github.com/D-Andreev/recommender-addon/blob/master/demo/benchmarks.js). 
```
tfidf*10: 10598.283ms
ratingPrediction*10: 0.179ms
tfidf*10: 11217.339ms
ratingPrediction*10: 0.095ms
```

### Contributing
Pull requests are welcome.

### Changelog
For complete changelog [click here](https://github.com/D-Andreev/recommender-addon/blob/master/CHANGELOG.md).

### License
MIT License

Copyright (c) 2017 Dimitar Andreev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

[![forthebadge](http://forthebadge.com/images/badges/contains-technical-debt.svg)](http://forthebadge.com) [![forthebadge](http://forthebadge.com/images/badges/check-it-out.svg)](http://forthebadge.com) [![forthebadge](http://forthebadge.com/images/badges/no-ragrets.svg)](http://forthebadge.com) [![forthebadge](http://forthebadge.com/images/badges/winter-is-coming.svg)](http://forthebadge.com) [![forthebadge](http://forthebadge.com/images/badges/powered-by-jeffs-keyboard.svg)](http://forthebadge.com)
[![forthebadge](http://forthebadge.com/images/badges/compatibility-ie-6.svg)](http://forthebadge.com)
