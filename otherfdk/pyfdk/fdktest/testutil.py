from fdk.util import *

class ForEachFileHandler:
	def handle(self, fileName, filePath):
		print "fileName: %s"%fileName
		print "\t%s"%filePath

def test():	
	assert tabStr(2, "abc") == "\t\tabc"
	
	assert lower1stChar("AbC") == "abC"
	assert lower1stChar("abc") == "abc"
	assert lower1stChar("") == ""
	
	assert getBaseName("123\\abc.txt") == "abc"
	assert getBaseName("abc.txt") == "abc"
	assert getBaseName("abc") == "abc"
	
	assert xmlEncode("<node>\"1&&2 message\"</node>") == "&lt;node&gt;&quot;1&amp;&amp;2 message&quot;&lt;/node&gt;"
	
	dir = r"C:\Windows\System32\drivers\etc"
	print "--------forEachFile in dir %s" % dir
	forEachFileHandler = ForEachFileHandler()
	forEachFile(dir, forEachFileHandler, False)
	
	