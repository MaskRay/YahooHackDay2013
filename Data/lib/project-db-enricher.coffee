'use continuation'

request = require 'request'
cheerio = require 'cheerio'
async = require 'async'

Project = require '../models/project'
Category = require '../models/category'

PUSH_TO_DB_CONCURRENCY = 10
CRAWL_PROJECTS_CONCURRENCY = 10
N_PAGE_MAX = 17481

host = 'http://sourceforge.net'

get = (url, callback) ->
	request url, cont(err, res, body)
	if !err and res.statusCode != 200
		err = 404 if res.statusCode = 404
	if err
		console.log err
	return callback err if err
	return callback null, body


pushToDB = (projname) ->
	Project.findOne name: projname, obtain proj
	if not proj
		proj = new Project name: projname
		proj.save obtain()

projectPushToDBQueue = async.queue pushToDB, PUSH_TO_DB_CONCURRENCY

parseProjects = ($) ->
	projects = $("#search_results ul.projects li a.project-icon")
	ret = []
	for proj in projects
		projname = proj.attribs.href.replace /^\/projects\/([^\/]*)\/.*$/, '$1'
		ret.push projname
	return ret


crawlProjects = (page, callback) ->
	path = "/directory/?sort=popular&page=#{page}"
	console.log "fetching #{host + path} ..."
	get host + path, obtain body
	console.log "fected #{host + path}"
	#console.log body
	$ = cheerio.load body
	projects = parseProjects($)
	for projname in projects
		#projectPushToDBQueue.push proj
		Project.findOne name: projname, obtain proj
		if not proj
			proj = new Project name: projname
			proj.save obtain()
	callback()

projectsCrawlQueue = async.queue crawlProjects, CRAWL_PROJECTS_CONCURRENCY

for page in [600..N_PAGE_MAX]
	projectsCrawlQueue.push page
