#!/usr/bin/python3

#fast create a new file and add some info

import sys
import os
import time

if __name__ == '__main__':
	argc = len(sys.argv)

	if (argc <= 2):
		print("lack of filename")
		sys.exit(0)
	filename = sys.argv[1]
	if os.path.exists(filename):
		print("file exist", filename)
		sys.exit(0)

	with open(filename, 'w') as f:
		f.write("/**\n")
		f.write(" * \\file\n")
		f.write(" * \\version  $Id: {0} 0 {1} xxx $\n".format(filename,time.strftime("%Y-%m-%d %H:%M:%S")))
		f.write(" * \\author  liuqing,liuqing1@ztgame.com\n")
		f.write(" * \\date {0}\n".format(time.asctime()))
		f.write(" * \\brief {0}\n".format(sys.argv[2]))
		f.write(" */\n\n")

	print("create ok")
