db = require '../lib/db'

keys =
	id:
		type: String
		index: true
		unique: true
	name: String
	parent:
		type: String
		index: true

categorySchema = new db.Schema keys

module.exports = Category = db.model 'Category', categorySchema

#Category.update = db.update Category, keys, 'id'
