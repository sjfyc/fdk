# -*- coding: utf8 -*-
import os

# tab字符填充
def tabStr(tabnum,format,*args):
	i=0
	f=""
	while i< tabnum:
		f += "\t"
		i += 1
	f += format
	return f % args

# 首字母小写
def lower1stChar(s):
	return s[0:1].lower() + s[1:]
	
# 从路径中获得文件名
def getFileNameFromPath(path, hasExt=True):    
	index1 = path.rfind("/")
	index2 = path.rfind("\\")
	index = index1
	if index2 > index:
		index = index2
	if hasExt:
		return path[index+1:]
	else:
		return path[index+1:-4]

_XML_ENCODES = \
{
	"<"	:"&lt;",
	">"	:"&gt;",
	"&"	:"&amp;",
	"'"	:"&apos;",
	"\"":"&quot;",
}
def xmlEncode(text):
	res = ""
	length=len(text)
	index = 0
	while index < length:
		substr = text[index:index+1]
		if _XML_ENCODES.has_key(substr):
			res += _XML_ENCODES[substr]
		else:
			res += substr
		index += 1
	return res

# 一般的文件写入标记
def _getFileWriteFlag():
	o_flag = os.O_WRONLY|os.O_CREAT|os.O_TRUNC
	try:
		o_flag |= os.O_BINARY
	except AttributeError:
		pass
	return o_flag

# 把字符串写入文件（覆盖式）
def writeFile(filePath, s):	
	fd = os.open(filePath, _getFileWriteFlag())
	os.write(fd, s)
	os.close(fd)

# 遍历文件夹下所有文件
def forEachFile(dir, handler, recursive=True):
	for fileName in os.listdir(dir):
		filePath = dir + '\\' + fileName
		if os.path.isdir(filePath):
			if recursive:
				forEachFile(filePath, handler, recursive)
		else:
			handler.handle(fileName, filePath)
