Recommender
===============================

`recommender` is a node addon with utility functions, which can help when building a recommender system. It contains implementations of [`tf-idf`](https://en.wikipedia.org/wiki/Tf%E2%80%93idf), [`Collaborative Filtering`](https://en.wikipedia.org/wiki/Collaborative_filtering) and `Global Baseline Approach` which are commonly used in recommendation systems. Each of the API methods have a **sync** and an **async** variation. **Using the async methods is highly recommended**, because they work in new threads and do not block the event loop. 

[![NPM](https://nodei.co/npm/recommender.png?downloads=true&downloadRank=true)](https://nodei.co/npm/recommender/)

[![Build Status](https://travis-ci.org/D-Andreev/recommender-addon.svg?branch=master)](https://travis-ci.org/D-Andreev/recommender-addon) [![NPM version](https://badge.fury.io/js/badge-list.svg)](https://www.npmjs.com/package/recommender) [![Open Source Love](https://badges.frapsoft.com/os/mit/mit.svg?v=102)](https://github.com/D-Andreev/recommender-addon/blob/master/LICENSE)

 * **[Installation](#Installation)**
 * **[Usage](#Usage)**
 * **[API](#API)**
 * **[Run examples and benchmarks](#Run-examples)**
 * **[Changelog](#Changelog)**
 * **[Contributing](#Contributing)**
 * **[License](#License)**

<a name="Installation"></a>
# Installation
 - `npm i recommender`

<a name="Usage"></a>

## Usage

### TF-IDF
The input of TF-IDF is a search query and a collection of documents. It finds how important a word is to a document in a collection. Then using cosine similarity we can get the most similar documents to the search query and make recommendations.
```js
var recommender = require('recommender');

var query = 'get current date time javascript';
var documents = [
    'get the current date and time in javascript',
    'get the current date and time in python',
    'something very different',
    'what is the time now'
];
recommender.tfidf(query, documents, (sortedDocs) => {
    console.log(sortedDocs);
    // Output:
    /**
        get the current date and time in javascript
        get the current date and time in python
    	what is the time now
        something very different
    */     
});
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

recommender.tfidf(queryPath, documentsPath, (sortedDocs) => {
    console.log(sortedDocs);
    // Output:
    /**
        get the current date and time in javascript
        get the current date and time in python
    	what is the time now
        something very different
    */   
});
```

We can also pass `filterStopWords` which is optional and `false` by default. If `filterStopWords` is `true` those words will be filtered out and not considered when calculating similarity. Stop-words are those words that appear very commonly across the documents, therefore loosing their representativeness and don't contribute to the meaning of the text. i.e (`a`, `about`, `the`, `if`, `it`, `is`...). The full stop words list can be viewed [here](https://github.com/D-Andreev/recommender-addon/blob/master/include/Constants.h#L8).
```js
bool filterStopWords = true;
var sortedDocs = recommender.tfidf(queryPath, documentsPath, filterStopWords, calback);
```

### Collaborative filtering
The input for collaborative filtering is a table with user ratings. Consider the following example.
```
       HP1   HP2   HP3   TW   SW1   SW2   SW3
   A   4     0     0     1     1     0     0
   B   5     5     4     0     0     0     0
   C   0     0     0     2     4     5     0
   D   3     0     0     0     0     0     3
```
`A`, `B`, `C` and `D` are users. `HP1` (Harry Potter 1), `TW` (Twilight), `SW1` (Star Wars 1) are movies.  A rating of `0` means that the user has not rated the movie. In this example ratings range from `1` to `5`, but they can be in any system (i.e. 1-10). The predicted rating of user `A` for `HP2`, using collaborative filtering is `4`. Aside from the ratings table we need to pass the row index and column index for the rating we wish to predict.
```js
var recommender = require('recommender');
var ratings = [
    [ 4, 0, 0, 1, 1, 0, 0 ],
	[ 5, 5, 4, 0, 0, 0, 0 ],
	[ 0, 0, 0, 2, 4, 5, 0 ],
	[ 3, 0, 0, 0, 0, 0, 3 ]
];
var movieIndex = 0;
var userIndex = 4;
// We are predicting the rating of U05 for M1.
var predictedRating = recommender.getRatingPrediction(ratings, movieIndex, userIndex, (predictedRating) => {
    console.log(predictedRating);
// Output: 4
});
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
`A`, `B`, `C` and `D` are users. `HP1` (Harry Potter 1), `TW` (Twilight), `SW1` (Star Wars 1) are movies. The predicted rating of user `A` for `HP2`, using the global baseline approach is `3.6363636363636362`.
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
var predictedRating = recommender.getGlobalBaselineRatingPrediction(ratings, userIndex, movieIndex, (predictedRating) => {
    console.log(predictedRating);
// Output: 3.6363636363636362
});
```
<a name="API"></a>
### API
* **[recommender.tfidf(`query`, `documents`, `useStopWords`, [`callback`])](#tfidf-arrays)**
* **[recommender.tfidf(`searchQueryFilePath`, `documentsFilePath`, `useStopWords`, [`callback`])](#tfidf-files)**
* **[recommender.getRatingPrediction(`ratings`, `rowIndex`, `colIndex`, [`callback`])](#get-r-p)**
* **[recommender.getGlobalBaselineRatingPrediction(`ratings`, `rowIndex`, `colIndex`, [`callback`])](#get-g-b)**
* **[recommender.getTopCFRecommendations(`ratings`, `rowIndex`, `limit`, [`callback`])](#get-top-cf)**
<a name="tfidf-arrays"></a>
##### recommender.tfidf(`query`, `documents`, `useStopWords`, [`callback`])
###### Arguments
* `query` - A string with the query. *(Required)*
* `documents` - An array of strings with the documents. *(Required)*
* `filterStopWords` - A boolean to filter out the stop words or not. *(Optional)* *(Default: `false`)*
* `callback` - A function with callback. *(Optional)*
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

var query = "get current date time javascript";
var documents = [
    'get the current date and time in javascript',
    'get the current date and time in python',
    'something very different',
    'what is the time now'
];
bool filterStopWords = true;
recommender.tfidf(query, documents, filterStopWords, (sortedDocs) => {
    // use sorted docs here....
});
```
<a name="tfidf-files"></a>
##### recommender.tfidf(`queryFilePath`, `documentsFilePath`, `useStopWords`, [`callback`])
###### Arguments
* `queryFilePath` - A string with the file path to the search query text file. *(Required)*
* `documentsFilePath` - A string with the file path to the documents text file. *(Required)*
* `filterStopWords` - A boolean to filter out the stop words or not. *(Optional)* *(Default: `false`)*
* `callback` - A function with callback. *(Optional)*
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

var queryFilePath = './search_query.txt';
var documentsFilePath = './documents.txt';
bool filterStopWords = true;
var weights = recommender.tfidf(queryFilePath, documentsFilePath, filterStopWords, (sortedDocs) => {
    // use sorted docs here...
});
```
<a name="get-r-p"></a>
##### recommender.getRatingPrediction(`ratings`, `rowIndex`, `colIndex`, [`callback`])
###### Arguments
* `ratings` - A two dimensional array with numbers representing the ratings. *(Required)*
* `rowIndex` - An integer with the index of the target row for prediction. *(Required)*
* `colIndex` - An integer with the index of the target column for prediction. *(Required)*
* `callback` - A function with callback. *(optional)*
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
recommender.getRatingPrediction(ratings, rowIndex, colIndex, (predictedRating) => {
    // predictedRating is 2.586406866934817
});
```
<a name="get-g-b"></a>
##### recommender.getGlobalBaselineRatingPrediction(`ratings`, `rowIndex`, `colIndex`, [`callback`])
###### Arguments
* `ratings` - A two dimensional array with numbers representing the ratings. *(Required)*
* `rowIndex` - An integer with the index of the target row for prediction. *(Required)*
* `colIndex` - An integer with the index of the target column for prediction. *(Required)*
* `callback` - A function with callback. *(optional)*
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
recommender.getGlobalBaselineRatingPrediction(ratings, userIndex, movieIndex, (predictedRating) => {
    // predictedRating is 3.6363636363636362
});
```
<a name="get-top-cf"></a>
##### recommender.getTopCFRecommendations(`ratings`, `rowIndex`, `limit`, [`callback`])
###### Arguments
* `ratings` - A two dimensional array with numbers representing the ratings. *(Required)*
* `rowIndex` - An integer with the index of the target row for prediction. *(Required)*
* `limit` - An integer with the max number of recommendations to be returned. *(Required)*
* `callback` - A function with callback. *(optional)*
###### Returns
An array of objects. Each object contains the item id and the predicted rating. The array is sorted by rating.
###### Examples
```js
var recommender = require('recommender');
var ratings = [
    [ 4, 0, 0, 1, 1, 0, 0 ],
    [ 5, 5, 4, 0, 0, 0, 0 ],
    [ 0, 0, 0, 2, 4, 5, 0 ],
    [ 3, 0, 0, 0, 0, 0, 3 ]
];
// We are getting the top recommendations for the first user.
recommender.getTopCFRecommendations(ratings, 0, 100, (recommendations) => {
    console.log(recommendations);
    /*
    [
      { itemId: 1, rating: 5 },
      { itemId: 5, rating: 5 },
      { itemId: 2, rating: 4 }
    ]
    */
});
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
