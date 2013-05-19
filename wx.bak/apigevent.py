#coding:utf8
import time
import redis
import jieba
import chardet
import re
import xml.dom.minidom  
from gevent import monkey; monkey.patch_all()
from gevent.pywsgi import WSGIServer
import MySQLdb
import MySQLdb.cursors
import socket
db=MySQLdb.connect(host="localhost",user="root",passwd="yahoohackbeijing",db="yahoohack", cursorclass =MySQLdb.cursors.DictCursor,use_unicode=True,charset="utf8")
cursor=db.cursor()
r=redis.StrictRedis(host="127.0.0.1", port=6379, db=0)
sendmessage="""
 <xml>
 <ToUserName><![CDATA[%s]]></ToUserName>
 <FromUserName><![CDATA[gh_12a1412f3492]]></FromUserName>
 <CreateTime>%s</CreateTime>
 <MsgType><![CDATA[text]]></MsgType>
 <Content><![CDATA[%s]]></Content>
 <FuncFlag>0</FuncFlag>
 </xml>
"""
def message_info(info):
  try:
	temp=re.findall(u"CDATA\[(.*)\]",info) 
        userid=temp[1][:-1]
        msgtype=temp[2][:-1]
        content=temp[3][:-1]
	msgid=re.findall(u"<MsgId>(\d*)</MsgId>",info)
	createtime=re.findall(u"<CreateTime>(\d*)</CreateTime>",info)
        print userid,content
	return userid,content
  except Exception,e:
	print e
	return "",""
	
def message_send(content,userid):
	s=sendmessage%(userid,"1368867920",content)
	print s
	return s
def search_hack(content,userid):
        timenow=int(time.time())
	cursor.execute("insert into mq values(%s,%s,%s)",(userid,timenow,content))
        db.commit()
        result=jieba.cut_for_search(content)
	result="|".join(result).split("|")
        
	returninfo=""
        u=''
        m=-1
        city=''
        lovers=''
        p=''
	for item in result:
            try:
                if item.lower() in langlower:
                    lovers=item
	    	    returninfo+=" "+item+"(lang)"
                elif item.lower() in cityen:
                    city=item
                    returninfo+=" "+item+"(city)"
                elif item in citycn:
                    city=item
                    returninfo+=" "+item+"(city)"
                elif item in univer:
                    u=item
                    returninfo+=" "+item+"(university)"
                else:
                    returninfo+=" "+item
            except:
                returninfo+=" "+item
            cursor.execute("select * from userinfo where id='%s'"%userid)
            backinfo=cursor.fetchall()
            if not len(backinfo):
                cursor.execute("insert into userinfo values(%s,%s,%s,%s,%s,%s)",(userid,city,u,m,p,lovers))
            else:
                if city:
                    sql="update userinfo set city='%s' where id='%s'"%(city,userid)
                    print sql
                    cursor.execute(sql)
                if u:
                    sql="update userinfo set university='%s' where id='%s'"%(u,userid)
                    print sql
                    cursor.execute(sql)
            db.commit()
        if len(u) or len(city) :
            return returninfo.encode("utf8")
        else:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
            sock.connect(('127.0.0.1',5000))  
            sock.send(content)
            info=sock.recv(1024)  
            sock.close()  
            return info

	

def start_server(environ, start_response):
    #print master_list,slave_list,hash_ring_master
    status = '200 OK'
    headers = [
        ('Content-Type', 'application/xml') ]#put the http header here
    start_response(status,headers)
    info=environ['wsgi.input'].read()
    print info
    print chardet.detect(info)
    userid,content=message_info(info) 
    content=search_hack(content,userid)
    return message_send(content,userid)

def init_server():
    global citycn
    global cityen
    global langlower
    global univer
    lang=open("language.list")
    city=open("chengshi.list")
    university=open("university.list")
    lang=lang.read()
    city=city.read()
    university=university.read()
    langlower=[]
    citycn=[]
    cityen=[]
    univer=[]
    lang=lang.split("\n")
    for item in lang:
        langlower.append(item.lower())
    city=city.split("\n")
    print city
    for item in city:
        print item
        try:
            cityinfo=item.split(" ")
            citycn.append(cityinfo[0].decode("utf8"))
            cityen.append(cityinfo[1].lower())
        except:
            pass
    print citycn,cityen
    university=university.split(" ")
    for item in university:
        univer.append(item.decode("utf8"))
        print item



    print lang



def main():
    init_server()
    WSGIServer(('', 80), start_server).serve_forever() 
if __name__ == '__main__':
    main() # if this is the main .then it will start here
########################################################################

