
TARGET		= ib2slurm
OBJECTS		= ib2slurm.o

#
# Where is SLURM installed?
#
SLURM_PREFIX	= /usr/local

ib2slurm_VERSION_MAJOR	= 0
ib2slurm_VERSION_MINOR	= 2

#
##
#

CC		= cc

CPPFLAGS	= -I/usr/include/infiniband -I$(SLURM_PREFIX)/include \
		  -Dib2slurm_VERSION_MAJOR=$(ib2slurm_VERSION_MAJOR) \
		  -Dib2slurm_VERSION_MINOR=$(ib2slurm_VERSION_MINOR) \
		  -DUSE_SLURM_HOSTLISTS

CFLAGS		= -Wextra -Wall -Wno-unused-parameter -g -O1

LDFLAGS		= -L$(SLURM_PREFIX)/lib -Wl,-rpath,$(SLURM_PREFIX)/lib

LIBS		= -libnetdisc -losmcomp -libmad -lslurm

#
##
#

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $+ $(LDFLAGS) $(LIBS)

clean:
	$(RM) $(OBJECTS) $(TARGET) ib2slurm-config.h

#
##
#

ib2slurm.o: ib2slurm.c ib2slurm-config.h

ib2slurm-config.h: ib2slurm-config.h.in
	touch ib2slurm-config.h

