db = require '../lib/db'

keys =
	name:
		type: String
		index: true
		unique: true
	crawlStatus:
		type: Number
		default: 0
		index: true
	descrition: String
	category: [String]

projectSchema = new db.Schema keys

module.exports = Project = db.model 'Project', projectSchema

#Project.update = db.update Project, keys, 'name'
