!/usr/bin/env python

from collections import defaultdict
from errno import ENOENT, EROFS
from stat import S_IFMT, S_IMODE, S_IFDIR, S_IFREG
from sys import argv, exit
from time import time

from fusell import FUSELL


class Memory(FUSELL):
    def create_ino(self):
        self.ino += 1
        return self.ino
    
    def init(self, userdata, conn):
        self.ino = 1
        self.attr = defaultdict(dict)
        self.data = defaultdict(str)
        self.parent = {}
        self.children = defaultdict(dict)
        
        self.attr[1] = {'st_ino': 1, 'st_mode': S_IFDIR | 0777, 'st_nlink': 2}
        self.parent[1] = 1
    
        #now make a new file log
        ino = self.create_ino()
        now = time()
        attr = {
            'st_ino': ino,
            'st_mode': 33204,
            'st_nlink': 1,
            'st_atime': now,
            'st_mtime': now,
            'st_ctime': now}
        self.attr[ino] = attr
        self.attr[1]['st_nlink'] += 1
        self.children[1]['log'] = ino
        
    forget = None

    #define a method to write to log, note: ino for log is 2
    def pwLog(self, ino, message):
        self.data[2] = self.data[2][:len(self.data[2])] + message + '\n'
        self.attr[2]['st_size'] = len(self.data[2])
    
    def getattr(self, req, ino, fi):
        # dont log or print log related operations 
        if ino != 2:
            print 'getattr:', ino
            #log as well
            self.pwLog(ino, '{0} {1}'.format('getattr:', ino))
        
        attr = self.attr[ino]
        if attr:
            self.reply_attr(req, attr, 1.0)
        else:
            self.reply_err(req, ENOENT)
    
    def lookup(self, req, parent, name):
        children = self.children[parent]
        ino = children.get(name, 0)
        
        # dont log or print log related operations 
        if ino != 2:
            print 'lookup:', parent, name        
            #log as well
            self.pwLog(ino, '{0} {1} {2}'.format('lookup:', parent, name))
        
        attr = self.attr[ino]
        
        if attr:
            entry = {'ino': ino, 'attr': attr, 'atttr_timeout': 1.0, 'entry_timeout': 1.0}
            self.reply_entry(req, entry)
        else:
            self.reply_err(req, ENOENT)
    
    def mkdir(self, req, parent, name, mode):
        ino = self.create_ino()
        
        # dont log or print log related operations 
        if ino != 2:
            print 'mkdir:', parent, name        
            #log as well
            self.pwLog(ino, '{0} {1} {2}'.format('mkdir:', parent, name))
        
        ctx = self.req_ctx(req)
        now = time()
        attr = {
            'st_ino': ino,
            'st_mode': S_IFDIR | mode,
            'st_nlink': 2,
            'st_uid': ctx['uid'],
            'st_gid': ctx['gid'],
            'st_atime': now,
            'st_mtime': now,
            'st_ctime': now}
        
        self.attr[ino] = attr
        self.attr[parent]['st_nlink'] += 1
        self.parent[ino] = parent
        self.children[parent][name] = ino
        
        entry = {'ino': ino, 'attr': attr, 'atttr_timeout': 1.0, 'entry_timeout': 1.0}
        self.reply_entry(req, entry)
    
    def mknod(self, req, parent, name, mode, rdev):
        ino = self.create_ino()
        
        # dont log or print log related operations 
        if ino != 2:        
            print 'mknod:', parent, name
            #log as well
            self.pwLog(ino, '{0} {1} {2}'.format('mknod:', parent, name))
        
        ctx = self.req_ctx(req)
        now = time()
        attr = {
            'st_ino': ino,
            'st_mode': mode,
            'st_nlink': 1,
            'st_uid': ctx['uid'],
            'st_gid': ctx['gid'],
            'st_rdev': rdev,
            'st_atime': now,
            'st_mtime': now,
            'st_ctime': now}
        
        self.attr[ino] = attr
        self.attr[parent]['st_nlink'] += 1
        self.children[parent][name] = ino
        
        entry = {'ino': ino, 'attr': attr, 'atttr_timeout': 1.0, 'entry_timeout': 1.0}
        self.reply_entry(req, entry)
    
    def open(self, req, ino, fi):
        # dont log or print log related operations 
        if ino != 2:
            print 'open:', ino
            #log as well
            self.pwLog(ino, '{0} {1}'.format('open:', ino))
        
        self.reply_open(req, fi)

    def read(self, req, ino, size, off, fi):
        # dont log or print log related operations 
        if ino != 2:
            print 'read:', ino, size, off
            #log as well
            self.pwLog(ino, '{0} {1} {2} {3}'.format('read:', ino, size, off))
        
        buf = self.data[ino][off:(off + size)]
        self.reply_buf(req, buf)
    
    def readdir(self, req, ino, size, off, fi):
        # dont log or print log related operations 
        if ino != 2:
            print 'readdir:', ino
            #log as well
            self.pwLog(ino, '{0} {1}'.format('readdir:', ino))
            
        parent = self.parent[ino]
        entries = [('.', {'st_ino': ino, 'st_mode': S_IFDIR}),
            ('..', {'st_ino': parent, 'st_mode': S_IFDIR})]
        for name, child in self.children[ino].items():
            entries.append((name, self.attr[child]))
        self.reply_readdir(req, size, off, entries)        
    
    def rename(self, req, parent, name, newparent, newname):
        ino = self.children[parent].pop(name)
        
        # dont log or print log related operations 
        if ino != 2:
            print 'rename:', parent, name, newparent, newname
            #log as well
            self.pwLog(ino, '{0} {1} {2} {3} {4}'.format('rename:', parent, name, newparent, newname))
            
        self.children[newparent][newname] = ino
        self.parent[ino] = newparent
        self.reply_err(req, 0)
    
    def setattr(self, req, ino, attr, to_set, fi):
        # dont log or print log related operations 
        if ino != 2:
            print 'setattr:', ino, to_set
            #log as well
            self.pwLog(ino, '{0} {1} {2}'.format('setattr:', ino, to_set))
            
        a = self.attr[ino]
        for key in to_set:
            if key == 'st_mode':
                # Keep the old file type bit fields
                a['st_mode'] = S_IFMT(a['st_mode']) | S_IMODE(attr['st_mode'])
            else:
                a[key] = attr[key]
        self.attr[ino] = a
        self.reply_attr(req, a, 1.0)
    
    def write(self, req, ino, buf, off, fi):
        # dont log or print log related operations 
        if ino != 2:
            print 'write:', ino, off, len(buf)
            #log as well
            self.pwLog(ino, '{0} {1} {2} {3}'.format('write:', ino, off, len(buf)))
            
        self.data[ino] = self.data[ino][:off] + buf
        self.attr[ino]['st_size'] = len(self.data[ino])
        self.reply_write(req, len(buf))

if __name__ == '__main__':
    if len(argv) != 2:
        print 'usage: %s <mountpoint>' % argv[0]
        exit(1)   
    fuse = Memory(argv[1])
