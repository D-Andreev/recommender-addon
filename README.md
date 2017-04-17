# Recommender
Recommender is a node addon with implementations of [tf-idf](https://en.wikipedia.org/wiki/Tf%E2%80%93idf) and [collaborative filtering](https://en.wikipedia.org/wiki/Collaborative_filtering), which are commonly used in recommendation systems.

[![NPM](https://nodei.co/npm/recommender.png?downloads=true&downloadRank=true)](https://nodei.co/npm/recommender/) [![NPM](https://nodei.co/npm-dl/recommender.png?months=6&height=3)](https://nodei.co/npm/recommender/)

[![NPM version](https://badge.fury.io/js/badge-list.svg)](https://www.npmjs.com/package/recommender) [![Open Source Love](https://badges.frapsoft.com/os/mit/mit.svg?v=102)](https://github.com/D-Andreev/recommender-addon/blob/master/LICENSE)

 * **[Installation](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#installation)**
 * **[Usage](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#usage)**
 * **[API](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#api)**
 * **[Run examples and benchmarks](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#license)**
 * **[Contributing](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#license)**
 * **[License](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#license)**

## Installation
 - `npm i recommender`

## Usage

### TF-IDF
The input of TF-IDF is a search query and a collection of documents. It finds how important a word is to a document in a collection or corpus. Then using cosine similarity we can get the most similar documents to the search query and make recommendations.
We create two text files with the input. Save this in `search_query.txt`.
```
get current date time javascript
```
And this into `documents.txt`. Every document is seperated by a new line and every word is seprated by space.
```
get the current date and time in javascript
get the current date and time in python
something very different
what is the time now
```

```js
var recommender = require('recommender');

var searchQueryPath = "./search_query.txt";
var documentsPath = "./documents.txt";

bool filterStopWords = false;
var weights = recommender.tfidf(searchQueryPath, documentsPath, filterStopWords);
var recommendations = recommender.recommend();
var sortedDocs = recommender.getSortedDocs();
console.log(sortedDocs);
// Output:
/**
    get the current date and time in javascript
    get the current date and time in python
    something very different
    what is the time now
*/
```
We can also pass `filterStopWords` which is optional and `false` by default. If `filterStopWords` is `true` those words will be filtered out and not considered when calculating similarity. Stop-words are those words that appear very commonly across the documents, therefore loosing their representativeness and don't contribute to the meaning of the text. i.e (`a`, `about`, `the`, `if`, `it`, `is`...). The full stop words list can be viewed [here](https://github.com/D-Andreev/recommender-addon/blob/master/include/Constants.h#L8).

### Collaborative filtering
The input for collaborative filtering is a table with user ratings. Each row is an item and each column is a user. Consider the following table with ratings of movies. `U01,U02,U03...U13` are users and `M01,M02,M03...M6` are movies. A rating of `0` means that the user has not rated the movie. Ratings range from `1` to `5`.
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
var predictedRating = recommender.getRatingPrediction(userRatings, movieIndex, userIndex);
console.log(predictedRating);
// Output: 2.586406866934817
```
### API
* **[recommender.tfidf(`searchQueryFilePath`, `documentsFilePath`, `useStopWords`)](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#recommendertfidfsearchqueryfilepath-documentsfilepath-usestopwords)**
* **[recommender.recommend()](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#recommenderrecommend)**
* **[recommender.getSortedDocs()](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#recommendergetsorteddocs)**
* **[recommender.getRatingPrediction(`ratings`, `rowIndex`, `colIndex`)](https://github.com/D-Andreev/recommender-addon/blob/0b61872cdfb58074110ab703464c45a22d0ce9ca/README.md#recommendergetratingpredictionratings-rowindex-colindex)**

##### recommender.tfidf(`searchQueryFilePath`, `documentsFilePath`, `useStopWords`)
###### Arguments
* `searchQueryFilePath` - A string with the file path of the search query text file. *(Required)*
* `documentsFilePath` - A string with the file path of the documents text file. *(Required)*
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

var searchQueryPath = "./search_query.txt";
var documentsPath = "./documents.txt";
bool filterStopWords = true;
var weights = recommender.tfidf(searchQueryPath, documentsPath, filterStopWords);
```
##### recommender.recommend()
###### Returns
An array with float point numbers representing the similarities. Every index corresponds to the index of the document in `documents.txt`.
```js
[1, 0.801901630090658, 0, 0.3223967271549685]
```
###### Examples
```js
var recommender = require('recommender');

var searchQueryPath = "./search_query.txt";
var documentsPath = "./documents.txt";

var weights = recommender.tfidf(searchQueryPath, documentsPath);
var recommendations = recommender.recommend();
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
### Run examples and benchmarks
- Clone the repo.
- `npm i` in the main folder.
- `npm i` in `/demo` folder.
- `node index.js` to run the examples.
- `node benchmarks.js` to run the benchmarks.

The benchmarks run tfidf and rating prediction with relatively large inputs. Can be viewed [here](https://github.com/D-Andreev/recommender-addon/blob/master/demo/benchmarks.js). 
```
tfidf*10: 1843.378ms
ratingPrediction*10: 3445.166ms
tfidf*10: 1834.131ms
ratingPrediction*10: 3540.355ms
```

### Contributing
Pull requests are welcome.

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
