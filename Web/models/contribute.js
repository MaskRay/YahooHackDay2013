'use continuation'
var db = require('../lib/db');

var contributeKeys = {
  login: {
    type: String,
    index: true,
  },
  repository: {
    type: String,
    index: true,
  },
  commits: Number,
};
contributeSchema = new db.Schema(contributeKeys);
module.exports = Contribute = db.model('contribute', contributeSchema);

Contribute.clearRepo = function(repository, next) {
  Contribute.remove({repository: repository}, next);
};

Contribute.add = function(raw, next) {
  var edge = new Contribute(raw);
  edge.save(next);
};

