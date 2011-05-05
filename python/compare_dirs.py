#! /usr/bin/env python

# This script compares two directories (source and backup) and also does take backup of neccessary files.
# TODO : 1. Compare the files for conflicts (DONE) and merging.
#        2. Ask for user confirmation (DONE)
#        3. Log
#        4. Add list compare 

# Owner            : Roopesh S
# Source file      : $Source: /usr/local/cvsroot/src/code/python/compare_dirs.py,v $ 
# Current Revision : $Revision: 1.6 $
# Branch           : $Name:  $
# Latest check-in  : $Author: roopesh $ on $Date: 2010-03-18 10:43:53 $ 

import os
import sys
import getopt
from sys import stderr 
from os import path 
from datetime import datetime

#import pdb # this has to be removed after debugging
#from pdb import set_trace

def print_st():
    from traceback import print_tb
    print_tb(sys.exc_info()[2]) # Print stack trace (for debugging)

def get_cur_time():
    return datetime.now().strftime('%d/%m/%Y %H:%M:%S')
 
class cfile_print:
    def __init__(self, fn, mode='w'):
        try:
            self.fp = open(path.abspath(path.expanduser(fn)), mode)
        except Exception, details:
            self.fp = None
            stderr.write('\nException : %s' % str(details))

    def write(self, s):
        try:
            self.fp.write(s)
        except:
            pass

    def __del__(self):
        if self.fp:
            try:
                self.fp.close()
            except:
                pass

class clog_file(cfile_print):
    def __init__(self, fn='~/.cd_log_file', verbose=False):
        cfile_print.__init__(self, fn, 'a')
        self.verbose = verbose 
    def write(self, s):
        cfile_print.write(self, '%s : %s\n'% (get_cur_time(),s))
        if self.verbose:
            print s

class cdiff_file(cfile_print):
    def __init__(self, fn='~/.cd_diff_file'):
        cfile_print.__init__(self, fn, 'a')
    def log_diff(self, src, dest):
        cfile_print.write(self, '%s : %s(%d) <> %s(%d)\n'% (get_cur_time(),path.abspath(src), path.getsize(src), path.abspath(dest), path.getsize(dest)))

class cmaps:
    DEFAULT_DEMLIMITER=" <##@@!@@##> "
    def __init__(self, map_file=None, log_fp = None):
        self.dict={}
        try:
            self.map_file = path.abspath(path.expanduser(map_file))
        except:
            self.map_file = None
        self.log_fp = log_fp # Log file pointer
        if self.map_file: 
            self.__scan_map_file__()

    def __scan_map_file__(self):
        try:
            fp = open(self.map_file, "r")
            for line in fp.readlines():
                try:
                    elems = line.strip().split(cmaps.DEFAULT_DEMLIMITER)
                    src_dir = elems[0].strip() # ASSUMPTION : both src and backup dirs as absolue path
                    back_dir = elems[1].strip()
                    self.dict[src_dir] = back_dir 
                except:
                    continue
            fp.close()
        except Exception, details:
            stderr.write("\nException :" + str(details))
    def iter(self):
        return self.dict.iteritems()
    def add_map(self, src, dest):
        try:
            s = path.abspath(src)
            d = path.abspath(dest)
            if s != d:
                if not (s in self.dict and path.samefile(d,self.dict[s])):
                    fp = open(self.map_file, "a+")
                    fp.write('%s\n' % cmaps.DEFAULT_DEMLIMITER.join((s,d,get_cur_time())))
                    fp.close()
                    return True
        except:
            pass 
        return False

def compare_wrapper(backup_obj, dirname, files):
    backup_obj._compare(dirname, files)

class cbackup:
    def __init__(self, src, dest, opts):
        self.ask_user_confirmation = True
        self.verbose = opts['verbose']
        try:
            self.log = clog_file(opts['log-file'], verbose=self.verbose)
        except:
            self.log = clog_file(verbose = self.verbose)
        try:
            self.diff = cdiff_file(opts['diff-file'])
        except:
            self.diff = cdiff_file()
        self.__add_src_backup__(src, dest)
        self.map = cmaps(opts['map-file'], self.log)
        self.log.write('*'*20+' RUN '+'*'*20)

    def __del__(self):
        self.map.add_map(self.src, self.dest)

    def __add_src_backup__(self, src, dest):
        ret = True
        if path.exists(src):
            self.src = path.normpath(src)
        else:
            ret = False
            stderr.write('Source Dir : %s does not exist' % src)
        if path.exists(dest):
            self.dest = path.normpath(dest)
        else:
            ret = False
            stderr.write('Backup Dir : %s does not exist' % dest)
        return ret

    def __user_confirm__(self):
        ret = 'n'
        while(1):
            stderr.write('[Yes/No/All] ? ')
            try:
                i = sys.stdin.readline().strip().lower()
            except:
                i = ''
            if i in ['y','yes']:
                ret = 'y'
                break
            elif i in ['n','no']:
                ret = 'n'
                break
            elif i in ['a', 'all']:
                ret = 'a'
                break
        return ret

    def itermap(self):
        return self.map.iter()

    def compare_dir(self, bidir=False, diffFile=False, backup=False, src=None, dest=None):
        if src and dest:
            if not self.__add_src_backup__(src, dest):
                return 
        self.log.write('-'*20)
        self.log.write('Comparing ')
        self.log.write("\tSource Directory : " + self.src)
        self.log.write("\tBackup Directory : " + self.dest)
        self.log.write('-'*20)

        self.ask_user_confirmation = True
        self.opts = {}
        self.opts['bidir'] = bidir
        self.opts['diffFile'] = diffFile
        self.opts['backup'] = backup

        path.walk(self.src, compare_wrapper, self)
        self.opts = {} # Clearing the opts

    def _compare(self, dirname, files):
        bidirection = self.opts['bidir']
        backup_files = self.opts['backup']
        consistency = self.opts['diffFile']
        cur_path = dirname[len(self.src):]
        if cur_path and cur_path[0] == '/':
            cur_path = cur_path[1:] # Remove leading slash. Otherwise, path.join() would treat this as absolute path !!! 
        dest_cur_path = path.join(self.dest, cur_path)
        prune_list = []
        try:
            dest_files = os.listdir(dest_cur_path)
            for search_file in files:
                if search_file in dest_files:
                    if consistency:
                        try:
                            src_fn = path.join(dirname, search_file)
                            if path.isfile(src_fn): # FIXME : Should add "os.stat(file)" for time-stamp check before checking HASH !! 
                                dest_fn = path.join(dest_cur_path, search_file)
                                if os.stat(src_fn)[-2] != os.stat(dest_fn)[-2]: # If modification time-stamp differs
                                    pipe_output = os.popen('sha1sum "%s" "%s"' % (src_fn, dest_fn))
                                    hash_output = pipe_output.read().split('\n')
                                    pipe_output.close()
                                    hash_1  = hash_output[0].split()[0]
                                    hash_2  = hash_output[1].split()[0]
                                    if hash_1 != hash_2:
                                        self.diff.log_diff(src_fn, dest_fn)
                                        self.log.write('DiffFile : %s %d %d' % (src_fn, path.getsize(src_fn), path.getsize(dest_fn)))
                        except Exception, details:
                            self.log.write('Exception : %s' % str(details))
                else:
                    missing_filename = path.join(dirname, search_file)
                    try:
                        if path.isdir(missing_filename):
                            prune_list.append(search_file) 
                    except Exception, details:
                        self.log.write('Exception : %s' % str(details))
                    is_copied = False 
                    if backup_files:
                        uc = 'y'
                        if self.ask_user_confirmation == True:
                            sys.stderr.write('COPY %s ==> %s ' % (missing_filename, dest_cur_path))
                            uc = self.__user_confirm__()
                            if uc == 'a':
                                self.ask_user_confirmation = False
                                uc = 'y'
                        if uc == 'y':
                            try:
                                self.log.write('%s ==> %s [COPYING] ... ' % (missing_filename, dest_cur_path))
                                ret = os.system('cp -pru "%s" "%s"' % (missing_filename, path.join(dest_cur_path, '.'))) 
                                if ret == 0:
                                    is_copied = True
                                    self.log.write('%s ==> %s [COPIED]' % (missing_filename, dest_cur_path))
                            except Exception, details:
                                self.log.write('Exception : %s' % str(details))
                                is_copied = False
                        if uc == 'n':
                            self.log.write('%s --> %s [MISSING]' % (missing_filename, dest_cur_path))
                    else:
                        self.log.write('%s --> %s [MISSING]' % (missing_filename, dest_cur_path))

            if bidirection:
                for search_file in dest_files:
                    if search_file in files:
                        pass
                    else:
                        missing_filename = path.join(dest_cur_path, search_file)
                        self.log.write('%s <-- %s [MISSING]' % (cur_path, missing_filename))
        except:
            self.log.write('%s --> %s [MISSING DIR]' % (dirname, dest_cur_path))
        for prune_file in prune_list:
            files.remove(prune_file)    # Pruning directories which are not in DEST, to avoid unneccessary traversals

def usage(script_file):
    print 'Usage : python '+ script_file + ' [-hvb --help --bidirection --verbose --backup-files ] <source dir> <backup dir>'
    print '-h | --help \t\t For Help'
    print '-v | --verbose \t\t For verbose output'
    print '-b | --bidirection \t List differences in both direction (src and backup dirs) '
    print '-c | --consistency \t List files which are not consistent in "src" and "backup" dirs '
    print '-m | --map-file=<MapFileName>  \t use the  map-file specified'
    print '--scan-mapfile \t Perfrom operation on map-list (src-->backup) contained in map-file'

def main(argv):
    try:
        opts, args = getopt.getopt(argv[1:], 'hvbcm', ["help", "verbose", "bidirection", "backup-files", "consistency", "map-file", "scan-mapfile"])
    except:
        usage(argv[0])
        sys.exit(2)
     
    if len(args) < 2:
        usage(argv[0])
        sys.exit(2)
    src = args[0].strip()
    dest = args[1].strip()

    maps_dict = {}

    opts_dict = {}
    opts_dict['verbose'] = False
    opts_dict['bidirection'] = False
    opts_dict['backup-files'] = False
    opts_dict['consistency'] = False
    opts_dict['user_confirmation'] = True 
    opts_dict['map-file'] = '~/.cd_map_file'
    opts_dict['scan-mapfile'] = False
   
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage(argv[0])
            sys.exit(2)
        elif opt in ("-v", "--verbose"):
            opts_dict['verbose'] = True
        elif opt in ("-b", "--bidirection"):
            opts_dict['bidirection'] = True
        elif opt in ("--backup-files"):
            opts_dict['backup-files'] = True
        elif opt in ("-c", "--consistency"):
            opts_dict['consistency'] = True
        elif opt in ("-m", "--map-file"):
            opts_dict['map_file'] = arg 
        elif opt in ("--scan-mapfile"):
            opts_dict['scan-mapfile'] = True 

    if not path.exists(src):
        stderr.write('\nError : Source Path (%s) does not exist' % src) 
    elif not path.exists(src):
        stderr.write('\nError : Dest Path (%s) does not exist' % dest)
    elif path.abspath(src) == path.abspath(dest):
        stderr.write('\nError : Source and Destination cannot be same!')
    else:
        try:
            backup_obj = cbackup(src, dest, opts_dict)
            backup_obj.compare_dir(bidir=opts_dict["bidirection"], diffFile = opts_dict["consistency"],backup = opts_dict["backup-files"])
            if opts_dict["scan-mapfile"]:
                for s,d in backup_obj.itermap():
                    if not path.samefile(s, src):
                        backup_obj.compare_dir(src=s, dest=d, bidir=opts_dict["bidirection"], diffFile = opts_dict["consistency"],backup = opts_dict["backup-files"])
        except Exception, details:
            stderr.write("\nException : %s\n" % str(details))

if __name__ == '__main__':
    #debug = pdb.Pdb()
    #debug.runcall(main, sys.argv)
    main(sys.argv)
