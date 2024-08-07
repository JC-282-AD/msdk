###############################################################################
 #
 # Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by
 # Analog Devices, Inc.),
 # Copyright (C) 2023-2024 Analog Devices, Inc.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #     http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
 #
 ##############################################################################

#
# Makefile for the FCL, Free Cryptographic Library
#

CC=    gcc.exe
CFLAGS= -O2 -W -Wall -DASYMMETRIC -DWORD32 -DX86 -D__x86

IFLAGS= -I ./ -I ./include -I ../include
LFLAGS= -L ./

LIBFLAGS=
OBJ  = main.o ecdsatest.o shatest.o ucl_sha256.o sha256_stone.o ucl_sha384.o ucl_sha512.o sha512_stone.o ucl_sia256.o ucl_sha3.o bignum.o ecdsa_generic_api.o ecdsa_high.o ucl_rng.o ucl_sys.o
TRACE_OBJ=

#
# Rules
#

all:    fcl_testing.exe

#
# Dependencies
#

%.o:    %.c
    $(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $ <

fcl_testing.exe:    $(OBJ)
    $(CC) $(CFLAGS) $(IFLAGS)  $(LFLAGS) -o fcl_testing.exe $(OBJ) $(LIBFLAGS)


# End Of Makefile
