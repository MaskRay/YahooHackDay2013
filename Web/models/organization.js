var db = require('../lib/db');

var organizationKeys = {
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
  location: String,
  gravatar_id: String,
  blog: String,
  email: {
    type: String,
    index: true,
  },
  created_at: Date,
  updated_at: Date,
  timestamp: Date,
};
organizationSchema = new db.Schema(organizationKeys);
module.exports = Organization = db.model('Organization', organizationSchema);

Organization.update = db.update(Organization, organizationKeys, 'login');
