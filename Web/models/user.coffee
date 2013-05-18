db = require '../lib/db'

userKeys =
  login:
    type: String
    index: true
    unique: true
  id:
    type: Number
    index: true
  name: String
  company: String
  location:
    type: String
    index: true
  gravatar_id: String
  blog: String
  email:
    type: String
    index: true
  bio: String
  hireable: Boolean
  created_at: Date
  updated_at: Date
  timestamp: Date
  pagerank: Number
  followCrawl:
    type: Date
    index: true
  languages:
    actionscript: Number
    c: Number
    csharp: Number
    cpp: Number
    commonlisp: Number
    css: Number
    diff: Number
    emacslisp: Number
    erlang: Number
    haskell: Number
    html: Number
    java: Number
    javascript: Number
    lua: Number
    objectivec: Number
    perl: Number
    php: Number
    python: Number
    ruby: Number
    scala: Number
    scheme: Number
  hits_rank:
    type: Number
    index: true

userSchema = new db.Schema(userKeys)
module.exports = User = db.model 'User', userSchema

User.update = db.update User, userKeys, 'login'
