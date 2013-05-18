mongoose = require 'mongoose'
config = require '../config'

mongoose.connect config.mongodb

module.exports = mongoose
