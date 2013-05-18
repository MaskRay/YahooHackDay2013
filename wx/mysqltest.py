import MySQLdb
import MySQLdb.cursors
db=MySQLdb.connect(host="geetest2013.mysql.rds.aliyuncs.com",user="dbc1i7c05mm3w308",passwd="gt2012wuhan",db="dbc1i7c05mm3w308",cursorclass =MySQLdb.cursors.DictCursor,use_unicode=True,charset="utf8")
cursor=db.cursor()
cursor.execute("insert into test values("ä½æ")

