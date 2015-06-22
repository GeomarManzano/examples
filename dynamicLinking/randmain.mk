# Copyright 2015 Geomar Manzano

# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC = gcc
OPTIMIZE = -O2
CFLAGS = $(OPTIMIZE) -g3 -Wall -Wextra -march=native -mtune=native -mrdrnd

all: randmain

randcpuid.o: randcpuid.c
	$(CC) $(CFLAGS) -c randcpuid.c

randlibhw.o: randlibhw.c
	$(CC) $(CFLAGS) -c -Wall -fPIC randlibhw.c

randlibhw.so: randlibhw.o
	$(CC) $(CFLAGS) -shared randlibhw.o -o randlibhw.so

randlibsw.o: randlibsw.c
	$(CC) $(CFLAGS) -c -Wall -fPIC randlibsw.c

randlibsw.so: randlibsw.o
	$(CC) $(CFLAGS) -shared randlibsw.o -o randlibsw.so

randmain.o: randmain.c
	$(CC) $(CFLAGS) -c randmain.c

randmain: randmain.o randcpuid.o randlibsw.so randlibhw.so
	$(CC) $(CFLAGS) randmain.o randcpuid.o -o randmain -ldl -Wl,-rpath=$(PWD)
