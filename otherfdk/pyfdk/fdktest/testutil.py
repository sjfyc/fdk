from fdk.util import *

class ForEachFileHandler:
	def handle(self, fileName, filePath):
		print "fileName: %s"%fileName
		print "filePath: %s"%filePath

def test():
	print tabStr(2, "2 tab in front")
	print "lower1stChar(AbC)=%s"%lower1stChar("AbC")
	print "lower1stChar(abc)=%s"%lower1stChar("abc")
	print "lower1stChar('')=%s|EOS"%lower1stChar('')
	print "getFileNameFromPath(abc.txt)=%s"%getFileNameFromPath("abc.txt")
	print "getFileNameFromPath(abc.txt,False)=%s"%getFileNameFromPath("abc.txt",False)
	print "getFileNameFromPath(123\\abc.txt)=%s"%getFileNameFromPath("abc.txt")
	print "getFileNameFromPath(123/abc.txt)=%s"%getFileNameFromPath("abc.txt")
	s = "<node>\"1&&2 message\"</node>"
	print "xmlEncode(%s)=%s"%(s,xmlEncode(s))
	print "++++++++forEachFile++++++++"
	forEachFileHandler = ForEachFileHandler()
	forEachFile("C:\\Windows\\System32\\drivers\\etc", forEachFileHandler, False)
	print "--------forEachFile--------"