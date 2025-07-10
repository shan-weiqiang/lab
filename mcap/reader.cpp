#define MCAP_IMPLEMENTATION  // Define this in exactly one .cpp file

#include <string_view>
#include <mcap/reader.hpp>
#include "mcap/types.hpp"
#include "route_guide.pb.h"

#include <cstring>
#include <iostream>
#include <unordered_map>

int main() {
  mcap::McapReader reader;
  auto status = reader.open("output.mcap");
  if (!status.ok()) {
    std::cerr << "Failed to open MCAP file: " << status.message << "\n";
    return 1;
  }

  // Callback to process each message
  auto onMessage = [&](const mcap::MessageView& messageView) {
    // Retrieve the channel associated with the message
    // auto channelIt = channels.find(messageView.channel->id);
    // if (channelIt == channels.end()) {
    //   std::cerr << "Unknown channel ID: " << messageView.channel->id << "\n";
    //   return;
    // }
    // const mcap::ChannelPtr& channel = channelIt->second;

    // // Retrieve the schema for the channel
    // auto schemaIt = schemas.find(messageView.schema->id);
    // if (schemaIt == schemas.end()) {
    //   std::cerr << "Unknown schema ID: " << messageView.schema->id << "\n";
    //   return;
    // }
    // const mcap::SchemaPtr& schema = schemaIt->second;

    const uint8_t* data =
        reinterpret_cast<const uint8_t*>(messageView.message.data);
    size_t dataSize = messageView.message.dataSize;

    routeguide::RouteSummary sum;
    sum.ParseFromArray(data, dataSize);
    std::cout << sum.distance() << std::endl;
    std::cout << sum.elapsed_time() << std::endl;
  };

  // Read all messages, invoking the callback for each
  auto msg_views = reader.readMessages();
  for (auto& msg : msg_views) {
    if ((msg.schema->encoding != "protobuf") ||
        msg.schema->name != "routeguide.RouteSummary") {
      continue;
    }
    onMessage(msg);
  }

  // reader.schemas()/reader.channels() should be called after this
  auto messageView = reader.readMessages();

  // statistics only available after calling of readSummary
  mcap::ReadSummaryMethod method;
  reader.readSummary(method);

  if (!reader.statistics()) {
    std::cout << "Statistics is not available" << std::endl;
  } else {
    std::cout << " Channel count: " << reader.statistics()->channelCount
              << std::endl;
    std::cout << " Msg count: " << reader.statistics()->messageCount
              << std::endl;
    std::cout << " Msg start time: " << reader.statistics()->messageStartTime
              << std::endl;
    std::cout << " Msg end time: " << reader.statistics()->messageEndTime
              << std::endl;
    std::cout << " Msg count at channel 1: "
              << reader.statistics()->channelMessageCounts.at(1) << std::endl;
  }

  // Store schemas and channels for quick lookup
  std::unordered_map<mcap::SchemaId, mcap::SchemaPtr> schemas;
  for (const auto& schema : reader.schemas()) {
    schemas[schema.second->id] = schema.second;
  }

  std::cout << "Total schemas: " << schemas.size() << std::endl;

  std::unordered_map<mcap::ChannelId, mcap::ChannelPtr> channels;
  for (const auto& channel : reader.channels()) {
    channels[channel.second->id] = channel.second;
  }
  std::cout << "Total channels: " << channels.size() << std::endl;

  for (auto it = messageView.begin(); it != messageView.end(); it++) {
    // skip messages that we can't use
    if ((it->schema->encoding != "protobuf") ||
        it->schema->name != "routeguide.RouteSummary") {
      continue;
    }
    std::cout << "Topic name: " << it->channel->topic << std::endl;
    routeguide::RouteSummary path;
    if (!path.ParseFromArray(static_cast<const void*>(it->message.data),
                             it->message.dataSize)) {
      std::cerr << "could not parse routeguide.RouteSummary" << std::endl;
      return 1;
    }
    std::cout << "Found message: " << path.ShortDebugString() << std::endl;
    std::cout << "Channel ID: " << it->channel->id << std::endl;
    // print out the message
  }

  reader.close();
  return 0;
}
