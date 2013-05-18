search = require '../lib/search'

exports.index = (req, res) ->
  search req.query, (err, users) ->
    if err?
      res.send {err}
    else
      res.send {users}
