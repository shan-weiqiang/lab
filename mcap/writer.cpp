#define MCAP_IMPLEMENTATION  // Define this in exactly one .cpp file

#include <cstddef>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/dynamic_message.h>
#include <mcap/writer.hpp>
#include "route_guide.pb.h"

#include <chrono>
#include <cstring>
#include <iostream>

// Recursively adds all `fd` dependencies to `fd_set`.
void fdSetInternal(google::protobuf::FileDescriptorSet& fd_set,
                   std::unordered_set<std::string>& files,
                   const google::protobuf::FileDescriptor* fd) {
  for (int i = 0; i < fd->dependency_count(); ++i) {
    const auto* dep = fd->dependency(i);
    auto [_, inserted] = files.insert(dep->name());
    if (!inserted) continue;
    fdSetInternal(fd_set, files, fd->dependency(i));
  }
  fd->CopyTo(fd_set.add_file());
}

// Returns a serialized google::protobuf::FileDescriptorSet containing
// the necessary google::protobuf::FileDescriptor's to describe d.
std::string fdSet(const google::protobuf::Descriptor* d) {
  std::string res;
  std::unordered_set<std::string> files;
  google::protobuf::FileDescriptorSet fd_set;
  fdSetInternal(fd_set, files, d->file());
  return fd_set.SerializeAsString();
}

mcap::Schema createSchema(const google::protobuf::Descriptor* d) {
  mcap::Schema schema(d->full_name(), "protobuf", fdSet(d));
  return schema;
}

// Returns the system time in nanoseconds. std::chrono is used here, but any
// high resolution clock API (such as clock_gettime) can be used.
mcap::Timestamp now() {
  return mcap::Timestamp(
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count());
}

int main() {
  // Initialize an MCAP writer with the "protobuf" profile and write the file
  // header
  mcap::McapWriter writer;
  auto status = writer.open("output.mcap", mcap::McapWriterOptions("protobuf"));
  if (!status.ok()) {
    std::cerr << "Failed to open MCAP file for writing: " << status.message
              << "\n";
    return 1;
  }

  // Register a Schema;
  // we now do not care about dynamic types
  mcap::Schema sche = createSchema(routeguide::RouteSummary::descriptor());
  writer.addSchema(sche);

  // Register a Channel
  mcap::Channel chatterPublisher("/chatter", "protobuf", sche.id);
  // Channel ID is created after addChannel
  writer.addChannel(chatterPublisher);

  // Create a message payload
  routeguide::RouteSummary sum;
  sum.set_distance(23);
  sum.set_elapsed_time(345345);

  // Write our message
  mcap::Message msg;

  msg.channelId = chatterPublisher.id;

  msg.sequence = 1;               // Optional
  msg.logTime = now();            // Required nanosecond timestamp
  msg.publishTime = msg.logTime;  // Set to logTime if not available
  auto payload = sum.SerializeAsString();
  msg.data = reinterpret_cast<std::byte*>(payload.data());

  msg.dataSize = payload.size();
  auto stat = writer.write(msg);
  std::cout << "One msg written: " << std::endl;
  std::cout << "Channel ID: " << msg.channelId << std::endl;

  // Finish writing the file
  writer.close();
}
