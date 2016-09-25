/*
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include <ctime>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <algorithm>

#include <grpc++/grpc++.h>
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>

#include "helloworld.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;


#define SERVER "localhost"
#define BILLION 1000000000L
#define NUM_ITERATIONS 10
#define BUFFER_SIZE 64000

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // Assambles the client's payload, sends it and presents the response back
  // from the server.
  char SayHello(int *arr, int size, int times) {

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The client streaming actual RPC.
    std::unique_ptr<ClientWriter<HelloRequest> > writer(stub_->SayHello(&context, &reply));
    for (int i = 0; i < times; ++i) {
	HelloRequest request;
	for (int j = 0; j < size; ++j) {
		request.add_data(arr[j]);
	}
	if (!writer->Write(request)) {
		break; // Broken stream
	}
    }
    writer->WritesDone();
    Status status = writer->Finish();

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return '0';
    }
  }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

double findMedian(std::vector<uint64_t> &vec) {
  std::sort(vec.begin(), vec.end());
  int size = vec.size();
  if (size % 2 == 0) {
    double val = (double)(vec[size/2] + vec[size/2 + 1]) / (double)2.0f;
    return val;
  }
  else {
    return vec[size/2];
  }
}

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  std::string connection = std::string(SERVER) + ":50051";
  GreeterClient greeter(grpc::CreateChannel(connection, grpc::InsecureChannelCredentials()));

  uint64_t total_time = 0;
  struct timespec start, end;

  for (int i = 1; i <= 512; i = i << 1) {
          int file_size = i * 1024 * 1024;
          int packet_size_in_int = BUFFER_SIZE / 4;
	  int times = (file_size / BUFFER_SIZE); 
          int *data = new int[packet_size_in_int];
	  std::vector<uint64_t> results(NUM_ITERATIONS, 0);
	  for (int itr = 0; itr < NUM_ITERATIONS; ++itr) {
	      clock_gettime(CLOCK_REALTIME, &start);/* mark start time */
	      char reply = greeter.SayHello(data, packet_size_in_int, times);
	      clock_gettime(CLOCK_REALTIME, &end);/* mark end time */
	      results[itr] = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
	  }
          delete data;
	  printf("Median time taken = %f nanoseconds for %d MB data.\n", findMedian(results), i);
  }
  return 0;
}
