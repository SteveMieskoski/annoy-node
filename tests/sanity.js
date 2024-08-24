/* global __dirname */

var test = require('tape');
var Annoy = require("../index");


test('Sanity test', sanityTest);


function sanityTest(t) {
  var obj = new Annoy(10, 'Angular');

  t.end();
}
