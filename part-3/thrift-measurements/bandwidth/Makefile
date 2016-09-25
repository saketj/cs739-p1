#
# Copyright 2015, Google Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#     * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

CXX = g++
CPPFLAGS += `pkg-config --cflags thrift`
CXXFLAGS += -std=c++11
LDFLAGS += `pkg-config --libs thrift` -I/unsup/boost-1.58.0/include
THRIFT = thrift
THRIFTFLAGS = -r --gen cpp

all: stub-gen client.out server.out

client.out: tutorial-cpp-CppClient.cpp gen-cpp/Calculator.cpp gen-cpp/shared_constants.cpp gen-cpp/SharedService.cpp gen-cpp/shared_types.cpp gen-cpp/tutorial_constants.cpp gen-cpp/tutorial_types.cpp
	$(CXX) -o $@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS)

server.out: tutorial-cpp-CppServer.cpp gen-cpp/Calculator.cpp gen-cpp/shared_constants.cpp gen-cpp/SharedService.cpp gen-cpp/shared_types.cpp gen-cpp/tutorial_constants.cpp gen-cpp/tutorial_types.cpp
	$(CXX) -o $@ $^ $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS)

stub-gen:
	$(THRIFT) $(THRIFTFLAGS) tutorial.thrift

clean:
	rm -rf *.o *.out gen-cpp/* gen-cpp
