
TARGET		= ib2slurm
OBJECTS		= ib2slurm.o

#
##
#

CC		= cc
CPPFLAGS	= -I/usr/include/infiniband
CFLAGS		= -Wextra -Wall -Wno-unused-parameter -g -O1
LDFLAGS		=
LIBS		= -libnetdisc

#
##
#

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $+ $(LDFLAGS) $(LIBS)

clean:
	$(RM) $(OBJECTS) $(TARGET)

