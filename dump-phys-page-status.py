#! /usr/bin/python

'''
The script is used to dump the physical page frame status in the Linux system by reading 
interfaces /proc/kpageflags and /proc/kpagecount. See /Documentation/vm/pagemap.txt for
more details.

Some of these code are borrowed from RAMpage project by Horst Schirmeier, thanks.

Author: Wang Xiaoqiang <wang_xiaoq@126.com> Distributed & Embeded System Lab
License: GPL v2


Usage: sudo ./dump-phys-page-status.py
'''

import struct

LOCKED		=	1 << 0
ERROR		=	1 << 1
REFERENCED	=	1 << 2
UPTODATE	=	1 << 3
DIRTY		=	1 << 4
LRU		=	1 << 5
ACTIVE		=	1 << 6
SLAB		=	1 << 7
WRITEBACK	=	1 << 8
RECLAIM		=	1 << 9
BUDDY		=	1 << 10
MMAP		=	1 << 11
ANON		=	1 << 12
SWAPCACHE	=	1 << 13
SWAPBACKED	=	1 << 14
COMPOUND_HEAD	=	1 << 15
COMPOUND_TAIL	=	1 << 16
HUGE		=	1 << 17
UNEVICTABLE	=	1 << 18
HWPOISON	=	1 << 19
NOPAGE		=	1 << 20
KSM             =       1 << 21

RESERVED        =       1 << 32

ALL_FLAGS = {
    LOCKED : 'LOCKED',
    ERROR : 'ERROR',
    REFERENCED : 'REFERENCED',
    UPTODATE : 'UPTODATE',
    DIRTY : 'DIRTY',
    LRU : 'LRU',
    ACTIVE : 'ACTIVE',
    SLAB : 'SLAB',
    WRITEBACK : 'WRITEBACK',
    RECLAIM : 'RECLAIM',
    BUDDY : 'BUDDY',
    MMAP : 'MMAP',
    ANON : 'ANON',
    SWAPCACHE : 'SWAPCACHE',
    SWAPBACKED : 'SWAPBACKED',
    COMPOUND_HEAD : 'COMPOUND_HEAD',
    COMPOUND_TAIL : 'COMPOUND_TAIL',
    HUGE : 'HUGE',
    UNEVICTABLE : 'UNEVICTABLE',
    HWPOISON : 'HWPOISON',
    NOPAGE : 'NOPAGE',
    KSM : 'KSM' ,
    RESERVED : 'RESERVED'
}

class PhysPageStatus:
	def __init__(self):
		self.flag_file_name = "/proc/kpageflags"
		self.count_file_name = "/proc/kpagecount"
		self.flag_file = open(self.flag_file_name, "r")
		self.count_file = open(self.count_file_name, "r")
		''' Physical address to be examined '''
		self.physical_address = 0
		''' Physical page frame corresponding to the physical_address '''
		self.physical_frame = 0
		''' Current page frame status '''
		self.current_status = 0
		self.current_count = 0

	def setCurPageFrame(self, phy_addr):
		self.physical_address = phy_addr
		self.physical_frame = phy_addr>>12

	def setCurPageStatus(self):
		offset = self.physical_frame * 8
		self.flag_file.seek(offset, 0)
		self.current_status = self.flag_file.read(8)
		self.count_file.seek(offset, 0)
		self.current_count = self.count_file.read(8)

	def getCurPageStatus(self):
		return self.current_status

	def dumpInfo(self):
		print "physical address %d" % self.physical_address
		print "physical frame %d" % self.physical_frame
		count = struct.unpack('L', self.current_count)
		print "current page count %d" % count
		status = struct.unpack('L', self.current_status)
		status = status[0]
		print "current page status %d" % status
		for (k, v) in ALL_FLAGS.items():
			if status & k:
				print v,
		print ""

if __name__ == '__main__':
	page_status = PhysPageStatus()

	while 1:
		phy_addr = int(input("Physical address: "))

		page_status.setCurPageFrame(phy_addr)
		page_status.setCurPageStatus()
		page_status.dumpInfo()
		print ""
