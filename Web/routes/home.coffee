_ = require 'underscore'

module.exports.index = (req, res) ->
  search req.query, (err, users) ->
    if err?
      res.render 'home', {err}
    else
      res.render 'home', {users}

module.exports.search = (req, res) ->
  location = req.param('location')
