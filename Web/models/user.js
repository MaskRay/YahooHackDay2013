var db = require('../lib/db');

var userKeys = {
  login: {
    type: String,
    index: true,
    unique: true,
  },
  id: {
    type: Number,
    index: true,
  },
  name: String,
  company: String,
  location: String,
  gravatar_id: String,
  blog: String,
  email: {
    type: String,
    index: true,
  },
  bio: String,
  hireable: Boolean,
  created_at: Date,
  updated_at: Date,
  timestamp: Date,
  pagerank: Number,
  followCrawl: {
    type: Date,
    index: true,
  },
};
userSchema = new db.Schema(userKeys);
module.exports = User = db.model('User', userSchema);

User.update = db.update(User, userKeys, 'login');
