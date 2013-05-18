home = require './home'

routes =
  '/':
    get: home.index

module.exports = (app) ->
  for path, methods of routes
    for method, handler of methods
      app[method] path, handler
