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
#include "complex_types.pb.h"
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

  // Register schemas for both simple and complex messages
  // Note: ComplexRouteSummary depends on complex_types.proto, so there will be overlapping FileDescriptorSet data
  mcap::Schema simple_schema = createSchema(routeguide::RouteSummary::descriptor());
  mcap::Schema complex_schema = createSchema(routeguide::ComplexRouteSummary::descriptor());
  
  std::cout << "Adding schemas:" << std::endl;
  std::cout << "  Simple schema: " << simple_schema.name << " (ID: " << simple_schema.id << ")" << std::endl;
  std::cout << "  Complex schema: " << complex_schema.name << " (ID: " << complex_schema.id << ")" << std::endl;
  
  writer.addSchema(simple_schema);
  writer.addSchema(complex_schema);

  // Register channels for both message types
  mcap::Channel simpleChannel("/simple_chatter", "protobuf", simple_schema.id);
  mcap::Channel complexChannel("/complex_chatter", "protobuf", complex_schema.id);
  
  writer.addChannel(simpleChannel);
  writer.addChannel(complexChannel);

  // Create a simple message payload
  routeguide::RouteSummary simple_sum;
  simple_sum.set_distance(23);
  simple_sum.set_elapsed_time(345345);

  // Create a complex message payload
  routeguide::ComplexRouteSummary complex_sum;
  
  // Set basic summary
  auto* basic = complex_sum.mutable_basic_summary();
  basic->set_distance(100);
  basic->set_elapsed_time(500);
  basic->set_point_count(50);
  basic->set_feature_count(10);
  
  // Set scene info
  auto* scene = complex_sum.mutable_scene_info();
  scene->set_timestamp(1234567890);
  scene->set_scene_id("test_scene_001");
  
  // Add ego position
  auto* ego_pos = scene->mutable_ego_position();
  ego_pos->set_x(10.5);
  ego_pos->set_y(20.3);
  ego_pos->set_z(0.0);
  
  // Add ego bounding box
  auto* ego_bbox = scene->mutable_ego_bbox();
  auto* min_point = ego_bbox->mutable_min_point();
  min_point->set_x(9.0);
  min_point->set_y(19.0);
  min_point->set_z(-1.0);
  auto* max_point = ego_bbox->mutable_max_point();
  max_point->set_x(12.0);
  max_point->set_y(22.0);
  max_point->set_z(2.0);
  ego_bbox->set_confidence(0.95);
  
  // Add a vehicle
  auto* vehicle = scene->add_vehicles();
  vehicle->set_id("vehicle_001");
  vehicle->set_type(complex::VehicleType::CAR);
  
  auto* vehicle_pos = vehicle->mutable_position();
  vehicle_pos->set_x(15.0);
  vehicle_pos->set_y(25.0);
  vehicle_pos->set_z(0.0);
  
  auto* vehicle_vel = vehicle->mutable_velocity();
  vehicle_vel->set_x(5.0);
  vehicle_vel->set_y(0.0);
  vehicle_vel->set_z(0.0);
  
  // Add vehicle trajectory points
  for (int i = 0; i < 3; ++i) {
    auto* traj_point = vehicle->add_trajectory();
    traj_point->set_x(15.0 + i * 2.0);
    traj_point->set_y(25.0 + i * 1.0);
    traj_point->set_z(0.0);
  }
  
  // Add vehicle attributes
  (*vehicle->mutable_attributes())["color"] = "red";
  (*vehicle->mutable_attributes())["model"] = "sedan";
  
  // Add a traffic light
  auto* traffic_light = scene->add_traffic_lights();
  traffic_light->set_id("tl_001");
  traffic_light->set_state(complex::TrafficLightState::GREEN);
  traffic_light->set_remaining_time(15.5);
  
  auto* tl_pos = traffic_light->mutable_position();
  tl_pos->set_x(30.0);
  tl_pos->set_y(40.0);
  tl_pos->set_z(5.0);
  
  // Add scene metrics
  (*scene->mutable_metrics())["avg_speed"] = 25.5;
  (*scene->mutable_metrics())["density"] = 0.3;
  
  // Add metadata
  (*complex_sum.mutable_metadata())["weather"] = "sunny";
  (*complex_sum.mutable_metadata())["road_type"] = "highway";
  
  // Add features
  for (int i = 0; i < 2; ++i) {
    auto* feature = complex_sum.add_features();
    feature->set_name("landmark_" + std::to_string(i));
    
    auto* feature_loc = feature->mutable_location();
    feature_loc->set_latitude(37 + i);
    feature_loc->set_longitude(-122 + i);
  }
  
  // Add distance segments
  complex_sum.add_distance_segments(10.5);
  complex_sum.add_distance_segments(15.2);
  complex_sum.add_distance_segments(8.7);

  // Write simple message
  mcap::Message simple_msg;
  simple_msg.channelId = simpleChannel.id;
  simple_msg.sequence = 1;
  simple_msg.logTime = now();
  simple_msg.publishTime = simple_msg.logTime;
  auto simple_payload = simple_sum.SerializeAsString();
  simple_msg.data = reinterpret_cast<std::byte*>(simple_payload.data());
  simple_msg.dataSize = simple_payload.size();
  auto simple_stat = writer.write(simple_msg);
  
  // Write complex message
  mcap::Message complex_msg;
  complex_msg.channelId = complexChannel.id;
  complex_msg.sequence = 1;
  complex_msg.logTime = now();
  complex_msg.publishTime = complex_msg.logTime;
  auto complex_payload = complex_sum.SerializeAsString();
  complex_msg.data = reinterpret_cast<std::byte*>(complex_payload.data());
  complex_msg.dataSize = complex_payload.size();
  auto complex_stat = writer.write(complex_msg);
  
  std::cout << "Two messages written: " << std::endl;
  std::cout << "Simple message - Channel ID: " << simple_msg.channelId << std::endl;
  std::cout << "Complex message - Channel ID: " << complex_msg.channelId << std::endl;

  // Finish writing the file
  writer.close();
}
