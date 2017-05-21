# Change Log

## 1.0.1
- Add readme file
- Add LICENSE
- Add CHANGELOG.md

## 1.1.0
- Add support for passing string and array to `recommender.tfidf`

## 1.1.1
- Fix readme
- Add basic js tests

## 1.1.2
- Separate some methods in Utils class

## 1.2.0
- Add global baseline to API.

## 1.3.0
- Add top collaborative filtering recommendations to API.

## 1.3.1
- Fix Recommender header file.
- Don't return NaN for GlobalBaseline

## 1.4.0
- Add async variants of all methods.

## 1.4.1
- Pass recommender instance to workers.

## 1.4.2
- Fix recommend function.

## 2.0.0
- Deprecate `recommend` and `getSortedDocs` and use only `tfidf`

## 2.0.1
- Add tests for `tfidf`.

## 2.0.2
- Add more tests.
- Separate methods for parsing parameters and convert between v8 types and c++ types.

## 2.0.3
- Refactor recommener_node, separate methods.
- Add tests.

## 2.0.4
- Fix build warnings.