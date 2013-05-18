var db = require('../lib/db');

var userMetaKeys = {
  login: {
    type: String,
    index: true,
    unique: true,
  },
  followCrawl: {
    type: Date,
    index: true,
  },
};
userMetaSchema = new db.Schema(userMetaKeys);
module.exports = UserMeta = db.model('usermeta', userMetaSchema);

UserMeta.update = db.update(UserMeta, userMetaKeys, 'login');
