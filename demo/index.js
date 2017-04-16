var recommender = require('recommender');
var documentPath = "resources\\document.txt";
var documentsPath = "resources\\documents.txt";
var weights = recommender.tfidf(documentPath, documentsPath);
var recommendations = recommender.recommend();
var sortedDocs = recommender.getSortedDocs();

console.log(weights);
console.log(recommendations);
console.log(sortedDocs);
/*
    { learning: 0.7027325540540822, life: 0.7027325540540822 }
    [ 1, 0.7071067811865476, 0.7071067811865475 ]
    [ 'The game of life is a game of everlasting learning',
      'The unexamined life is not worth living',
      'Never stop learning' ]
*/
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
console.log('res', predictedRating);