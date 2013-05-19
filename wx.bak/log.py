#coding:utf8
import redis
import json
import re
from StringIO import StringIO
import requests
import base64
from BeautifulSoup import BeautifulSoup as bs
from hashlib import md5
r = redis.StrictRedis(host='localhost', port=6379, db=0)
def md5value(values):
    m=md5()
    m.update(values)
    return m.hexdigest()
token=''
cookies=dict()
musicurl="https://mp.weixin.qq.com/cgi-bin/getvoicedata?token=%s&msgid=%s&fileid=0 "
allmessage="https://mp.weixin.qq.com/cgi-bin/getmessage?t=wxm-message&token=%s&lang=zh_CN&count=50"
cookieskey=['slave_user','cert','slave_sid','remember_acct','sig']
def update_cookies(backinfo):
    for item in cookieskey:
        try:
            cookies[item]=backinfo.cookies[item]
        except Exception,e:
            print e
def logwx(uname,passwd):
        global token
        payload = {'username': uname, 'pwd': md5value(passwd),'imgcode':'','f':'json'}
        print uname,md5value(passwd)
        a=requests.post("https://mp.weixin.qq.com/cgi-bin/login?lang=zh_CN",data=payload,cookies={"cert":"spWYnLrkKHSQ_Rm5nTX503Qn9tX6vBJV"})
        jinfo=json.loads(a.text)
        print jinfo
        newurl="https://mp.weixin.qq.com"+jinfo["ErrMsg"]
        token=re.findall("token=(.*)",jinfo["ErrMsg"])
        for item in a.cookies:
            print item
        update_cookies(a)
        print cookies
        a=requests.post(newurl,cookies=cookies)
        url=allmessage%token[0]
        print url
        a=requests.post(url,cookies=cookies)
        print a.text
        info=re.findall('"id":"(\d*)"',a.text)
        typeinfo=re.findall('"type":"(\d*)"',a.text)
        print info,typeinfo
        for item in xrange(len(typeinfo)):
            if typeinfo[item]=='3':
                if r.hexists(info[item],"handled"):
                     pass
                else :
                    url=musicurl%(token[0],info[item])
                    print url
                    temp=requests.post(url,cookies=cookies)
                    f=open("music%s.mp3"%info[item],"wb+")
                    f.write(temp.content)
if __name__=="__main__":
        #name=raw_input("name:")
        #passwd=raw_input("password:")
        logwx("1550287285","xredcn123")
