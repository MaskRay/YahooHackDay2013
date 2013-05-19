_ = require 'underscore'
search = require '../lib/search'

module.exports.index = (req, res) ->
  res.render 'index'

module.exports.search = (req, res) ->
  console.log req.body
  search req.body, (err, users) ->
    if err?
      console.log err
      res.render 'index', {err}
    else
      console.log users
      res.render 'index', {users}
