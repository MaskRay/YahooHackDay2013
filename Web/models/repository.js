var db = require('../lib/db');

var repositoryKeys = {
  name: {
    type: String,
    index: true,
  },
  full_name: {
    type: String,
    index: true,
    unique: true,
  },
  owner: {
    type: String,
    index: true,
  },
  description: String,
  homepage: String,
  master_branch: String,
  parent: String,
  forks: [String],
  language: String,
  size: Number,
  watchers: {
    type: Number,
    index: true,
  },
  created_at: Date,
  updated_at: Date,
  pushed_at: Date,
  timestamp: Date,
  contributeCrawl: {
    type: Date,
    index: true,
  }
};
repositorySchema = new db.Schema(repositoryKeys);
module.exports = Repository = db.model('Repository', repositorySchema);

Repository.update = db.update(Repository, repositoryKeys, 'full_name');
