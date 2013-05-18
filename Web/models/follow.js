'use continuation'
var db = require('../lib/db');

var followKeys = {
  from: {
    type: String,
    index: true,
  },
  to: {
    type: String,
    index: true,
  },
};
followSchema = new db.Schema(followKeys);
module.exports = Follow = db.model('Follow', followSchema);

Follow.clearTo = function(login, next) {
  Follow.remove({to: login}, next);
};

Follow.add = function(from, to, next) {
  var edge = new Follow({from: from, to: to});
  edge.save(next);
};

Follow.update = function(raw, next) {
  var data = {from: raw.from, to: raw.to};
  try {
    Follow.findOne(data, obtain(existing));
    if (!existing) {
      var edge = new Follow(data);
      edge.save(obtain());
    }
    next();
  } catch (err) {
    next(err);
  }
}
