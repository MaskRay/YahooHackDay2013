'use continuation'
mongoose = require 'mongoose'
util = require 'util'
config = require '../config'

host = config.mongodb.host
port = config.mongodb.port
db = config.mongodb.db
address = util.format 'mongodb://%s:%s/%s', host, port, db

module.exports = mongoose

mongoose.connect address
mongoose.connected = false
mongoose.connection.on 'open', () ->
	mongoose.connected = true
	console.log 'MongoDB connected.'

mongoose.update = (collection, keys, primary) ->
	(raw, next) ->
		try
			key = {}
			key[primary] = raw[primary]
			collection.findOne key, obtain existing
			updated = false
			instance = null

			if existing
				Object.keys(raw).forEach (key) ->
					if key is '_id'
						return
					if raw.timestamp >= existing.timestamp || existing[key] is undefined
						existing[key] = raw[key]
						updated = true
				instance = existing
			else # create new
				instance = new collection raw
				updated = true

			if updated
				instance.save obtain()

			next null, updated
		catch err
			next err
