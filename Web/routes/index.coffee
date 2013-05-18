home = require './home'
api = require './api'

routes =
  '/':
    get: home.index
    post: home.search
  '/api':
    get: api.index

module.exports = (app) ->
  for path, methods of routes
    for method, handler of methods
      app[method] path, handler
