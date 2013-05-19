#coding=utf-8  
import time
import tornado.httpserver
import tornado.ioloop
import tornado.web
import os
import string
#import MySQLdb
#import MySQLdb.cursors
from tornado.options import define, options
#db=MySQLdb.connect(host="localhost",user="root",passwd="912619",db="weibo", cursorclass = MySQLdb.cursors.DictCursor,charset="utf8")
#cursor=db.cursor()

define("port", default=80, help="Run server on a specific port",
        type=int)
class MainHandler(tornado.web.RequestHandler):
    def get(self):
	signature=self.get_argument("signature")
	timestamp=self.get_argument("timestamp")
	nonce=self.get_argument("nonce")
	echostr=self.get_argument("echostr")
	self.write(echostr)
     	
settings = {
    "debug": True,
    "static_path": os.path.join(os.path.dirname(__file__), "static"),
    "cookie_secret":"iloveluoweiyi",
}


application = tornado.web.Application([
    (r"/", MainHandler),
  ], **settings)

if __name__ == "__main__":
    http_server = tornado.httpserver.HTTPServer(application)
    tornado.options.parse_command_line()
    http_server.listen(80)
    tornado.ioloop.IOLoop.instance().start()


