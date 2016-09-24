/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <iostream>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "gen-cpp/Calculator.h"

#define SERVER "localhost"
#define BILLION 1000000000L
#define NUM_ITERATIONS 100 

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace tutorial;
using namespace shared;

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

int main() {
  boost::shared_ptr<TTransport> socket(new TSocket(SERVER, 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  CalculatorClient client(protocol);

  try {
    transport->open();

    uint64_t total_time = 0;
    struct timespec start, end;
  
    for(int size = 1; size <= 8192; size = size << 1) {
      std::vector<uint64_t> results(NUM_ITERATIONS, 0);
      for (int itr = 0; itr < NUM_ITERATIONS; ++itr) {
	vector<int32_t> data(size, 0x5555555);
        clock_gettime(CLOCK_REALTIME, &start);	/* mark start time */
        int reply = client.add(data);
        clock_gettime(CLOCK_REALTIME, &end);	/* mark end time */
        results[itr] = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
      }
      std::cout << "Bytes = " << (size * 4) << " RTT = " << findMedian(results) << " ns" << std::endl;
    }
    
    transport->close();
  } catch (TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
}
