'use continuation'

CONCURRENCY = 20

request = require 'request'
cheerio = require 'cheerio'
async = require 'async'

Project = require '../models/project'
Category = require '../models/category'

host = 'http://sourceforge.net'

projectCrawler = (callback) ->
	while true
		Project.findOne crawlStatus: 0, obtain(pendingProj)
		if not pendingProj
			break
		# FIXME: WARNING: may cause inconsistency
		pendingProj.crawlStatus = 1
		pendingProj.save obtain()

		projname = pendingProj.name
		try
			# assert proj is in db and crawled is false
			# description
			console.log "fetching #{host}/projects/#{projname} ..."
			get "#{host}/projects/#{projname}", obtain(projpage)

			$ = cheerio.load projpage
			desc = parseDescription($)
			cates = parseCategories($)

			Project.update({name: projname},{
				description: desc
				category: cates
				crawlStatus: 2
			}, obtain())
			###
			proj =
				name: projname
				description: desc
				category: cates
				crawlStatus: 2
			###

			#Project.update proj, obtain()

			console.log "------------------"
			console.log "project: #{projname}"
			console.log desc
			console.log cates
			console.log "------------------"
		catch err
			Project.update({name: projname}, {crawlStatus: 3}, obtain())

	console.log 'no uncrawed project, pending, crawler sleep for 1s'
	setTimeout(projectCrawler, 1000)
	
	# never ends
	# callback()

get = (url, callback) ->
	request url, cont(err, res, body)
	if !err and res.statusCode != 200
		err = 404 if res.statusCode = 404
	if err
		console.log err
	return callback err if err
	return callback null, body

parseDescription = ($) ->
	$("[itemprop='description']").text()

parseCategories = ($) ->
	elimiteUnneedStr = (strArr) ->
		ret = []
		for str in strArr
			if str.length > 1 and str != 'directory'
				ret.push str
		return ret

	catesIds = []
	extract = ($$) ->
		for item in $$
			cateId = elimiteUnneedStr item.attribs['href'].split('/')
			catesIds = catesIds.concat cateId

			cateDesc = $(item).text()
	#console.log $.html().match /itemprop="[^"]*"/
	extract $("[itemprop='applicationCategory']")
	extract $("[itemprop='softwareApplicationSubCategory']")

	return catesIds

async.parallel (projectCrawler for i in [0..CONCURRENCY-1]), () ->
