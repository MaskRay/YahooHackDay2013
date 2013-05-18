express = require 'express'
http = require 'http'
path = require 'path'
config = require './config'

app = express()
app.configure ->
  app.set 'port', process.env.PORT or 3000
  app.set 'views', __dirname + '/views'
  app.set 'view engine', 'jade'
  app.use express.favicon()
  app.use express.logger('dev')
  app.use express.bodyParser()
  app.use express.methodOverride()
  app.use app.router

  app.use express.static(path.join(__dirname, 'public'))

app.configure 'development', ->
  app.use express.errorHandler()

require('./routes') app

http.createServer(app).listen app.get('port'), ->
  console.log 'Express server listening on port ' + app.get('port')
