'use continuation'
var mongoose = require('mongoose');
var util = require('util');
var config = require('./config');

var host = config.mongodb.host;
var port = config.mongodb.port;
var db = config.mongodb.db;
var address = util.format('mongodb://%s:%s/%s', host, port, db);

module.exports = mongoose;

mongoose.connect(address);
mongoose.connected = false;
mongoose.connection.on('open', function() {
  mongoose.connected = true;
});

mongoose.update = function(collection, keys, primary) {
  return function(raw, next) {
    try {
      // Find existing object
      var key = {};
      key[primary] = raw[primary];
      collection.findOne(key, obtain(existing));
      var updated = false;
      if (existing) {
        // Update existing
        Object.keys(raw).forEach(function(key) {
          if (key === '_id') {
            return;
          }
          if (raw.timestamp >= existing.timestamp || existing[key] === undefined) {
            existing[key] = raw[key];
            updated = true;
          }
        });
        var instance = existing;
      } else {
        // Create new
        var instance = new collection(raw);
        updated = true;
      }
      if (updated) {
        instance.save(obtain());
      }
      next(null, updated);
    } catch (err) {
      next(err);
    }
  };
};
