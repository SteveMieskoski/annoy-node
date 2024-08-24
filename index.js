// var annoyAddon = require('bindings')('addon');
const annoyAddon = require('./build/Release/addon');
console.log(annoyAddon)
module.exports = annoyAddon.Annoy;
