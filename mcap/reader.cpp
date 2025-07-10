#define MCAP_IMPLEMENTATION  // Define this in exactly one .cpp file

#include <string_view>
#include <mcap/reader.hpp>
#include "mcap/types.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/message.h>
#include <google/protobuf/reflection.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <unordered_map>

class GenericProtobufReader {
private:
  std::unique_ptr<google::protobuf::DescriptorPool> descriptor_pool_;
  std::unique_ptr<google::protobuf::DynamicMessageFactory> message_factory_;
  std::unordered_map<std::string, const google::protobuf::Descriptor*>
      descriptors_;
  std::unordered_map<mcap::SchemaId, std::string> schema_id_to_type_name_;
  std::unordered_set<std::string> registered_files_;  // Track registered file descriptors

 public:
  GenericProtobufReader()
      : descriptor_pool_(new google::protobuf::DescriptorPool(
            google::protobuf::DescriptorPool::generated_pool())),
        message_factory_(new google::protobuf::DynamicMessageFactory()) {}

  // Register all protobuf schemas from the MCAP file
  bool registerSchemas(const mcap::McapReader& reader) {
    std::cout << "Registering protobuf schemas..." << std::endl;

    auto schemas = reader.schemas();
    std::cout << "Found " << schemas.size() << " schemas in MCAP file"
              << std::endl;

    for (const auto& schema_pair : schemas) {
      const auto& schema = schema_pair.second;

      std::cout << "Found schema: " << schema->name << " (ID: " << schema->id
                << ", encoding: " << schema->encoding << ")" << std::endl;

      if (schema->encoding != "protobuf") {
        std::cout << "Skipping non-protobuf schema: " << schema->name
                  << std::endl;
        continue;
      }

      std::cout << "Processing schema: " << schema->name
                << " (ID: " << schema->id << ")" << std::endl;

      // Parse the FileDescriptorSet from the schema data
      google::protobuf::FileDescriptorSet file_descriptor_set;
      std::string schema_data(
          reinterpret_cast<const char*>(schema->data.data()),
          schema->data.size());
      std::cout << "  Schema data size: " << schema_data.size() << " bytes"
                << std::endl;
      if (!file_descriptor_set.ParseFromString(schema_data)) {
        std::cerr << "Failed to parse FileDescriptorSet for schema: "
                  << schema->name << std::endl;
        continue;
      }
      std::cout << "  FileDescriptorSet contains "
                << file_descriptor_set.file_size() << " files" << std::endl;

            // Add all file descriptors to the pool (avoiding duplicates)
      for (int i = 0; i < file_descriptor_set.file_size(); ++i) {
        const auto& file_proto = file_descriptor_set.file(i);
        const std::string& file_name = file_proto.name();
        
        // Check if this file descriptor has already been registered
        if (registered_files_.find(file_name) != registered_files_.end()) {
          std::cout << "  Skipping already registered file descriptor: " << file_name << std::endl;
          continue;
        }
        
        google::protobuf::FileDescriptorProto file_desc_proto;
        file_desc_proto.CopyFrom(file_proto);
        
        const google::protobuf::FileDescriptor* file_desc = 
            descriptor_pool_->BuildFile(file_desc_proto);
        if (!file_desc) {
          std::cerr << "Failed to build file descriptor: " << file_name << std::endl;
          continue;
        }
        
        // Mark this file as registered
        registered_files_.insert(file_name);
        
        std::cout << "  Added file descriptor: " << file_name << std::endl;
        std::cout << "  File contains " << file_desc->message_type_count()
                  << " message types" << std::endl;
      }

            // Find the main message descriptor for this schema
      std::cout << "  Looking for message type: " << schema->name << std::endl;
      
      // Check if this type exists in the generated pool (for conflict detection)
      const google::protobuf::Descriptor* generated_descriptor = 
          google::protobuf::DescriptorPool::generated_pool()
              ->FindMessageTypeByName(schema->name);
      
      const google::protobuf::Descriptor* descriptor = 
          descriptor_pool_->FindMessageTypeByName(schema->name);
      
      if (descriptor) {
        // Check for conflicts
        if (generated_descriptor && generated_descriptor != descriptor) {
          std::cout << "  ⚠️  WARNING: Type '" << schema->name 
                    << "' exists in both generated and dynamic pools!" << std::endl;
          std::cout << "     Using dynamic pool version (from MCAP schema)" << std::endl;
          
          // Compare field counts to detect schema evolution
          int generated_fields = generated_descriptor->field_count();
          int dynamic_fields = descriptor->field_count();
          if (generated_fields != dynamic_fields) {
            std::cout << "     Schema evolution detected: " 
                      << generated_fields << " fields (generated) vs "
                      << dynamic_fields << " fields (dynamic)" << std::endl;
          }
        }
        
        descriptors_[schema->name] = descriptor;
        schema_id_to_type_name_[schema->id] = schema->name;
        std::cout << "  Registered message type: " << schema->name << std::endl;
      } else {
        std::cerr << "  Failed to find descriptor for: " << schema->name
                  << std::endl;
        // Try to list all available message types in the pool
        std::cout << "  Available message types in pool:" << std::endl;
        // Note: This is a simplified approach - in practice you'd need to
        // iterate through all files
      }
    }

    std::cout << "Schema registration complete. Total message types: "
              << descriptors_.size() << std::endl;
    std::cout << "Total registered file descriptors: " << registered_files_.size() << std::endl;
    std::cout << "Registered files: ";
    for (const auto& file : registered_files_) {
      std::cout << file << " ";
    }
    std::cout << std::endl;
    return true;
  }

  // Print a protobuf message using reflection
  void printMessage(const google::protobuf::Message& message,
                    const std::string& indent = "") {
    const google::protobuf::Reflection* reflection = message.GetReflection();
    const google::protobuf::Descriptor* descriptor = message.GetDescriptor();

    std::cout << indent << "Message: " << descriptor->full_name() << std::endl;

    for (int i = 0; i < descriptor->field_count(); ++i) {
      const google::protobuf::FieldDescriptor* field = descriptor->field(i);

      if (field->is_repeated()) {
        int size = reflection->FieldSize(message, field);
        if (size > 0) {
          std::cout << indent << "  " << field->name()
                    << " (repeated, size=" << size << "):" << std::endl;
          for (int j = 0; j < size; ++j) {
            printFieldValue(message, field, j, indent + "    ");
          }
        }
      } else if (reflection->HasField(message, field)) {
        std::cout << indent << "  " << field->name() << ": ";
        printFieldValue(message, field, -1, indent + "    ");
      }
    }
  }

 private:
  void printFieldValue(const google::protobuf::Message& message,
                       const google::protobuf::FieldDescriptor* field,
                       int index, const std::string& indent) {
    const google::protobuf::Reflection* reflection = message.GetReflection();

    switch (field->cpp_type()) {
      case google::protobuf::FieldDescriptor::CPPTYPE_INT32: {
        int32_t value =
            index >= 0 ? reflection->GetRepeatedInt32(message, field, index)
                       : reflection->GetInt32(message, field);
        std::cout << value << std::endl;
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_INT64: {
        int64_t value =
            index >= 0 ? reflection->GetRepeatedInt64(message, field, index)
                       : reflection->GetInt64(message, field);
        std::cout << value << std::endl;
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT32: {
        uint32_t value =
            index >= 0 ? reflection->GetRepeatedUInt32(message, field, index)
                       : reflection->GetUInt32(message, field);
        std::cout << value << std::endl;
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT64: {
        uint64_t value =
            index >= 0 ? reflection->GetRepeatedUInt64(message, field, index)
                       : reflection->GetUInt64(message, field);
        std::cout << value << std::endl;
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE: {
        double value =
            index >= 0 ? reflection->GetRepeatedDouble(message, field, index)
                       : reflection->GetDouble(message, field);
        std::cout << value << std::endl;
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT: {
        float value = index >= 0
                          ? reflection->GetRepeatedFloat(message, field, index)
                          : reflection->GetFloat(message, field);
        std::cout << value << std::endl;
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_BOOL: {
        bool value = index >= 0
                         ? reflection->GetRepeatedBool(message, field, index)
                         : reflection->GetBool(message, field);
        std::cout << (value ? "true" : "false") << std::endl;
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_ENUM: {
        const google::protobuf::EnumValueDescriptor* enum_value =
            index >= 0 ? reflection->GetRepeatedEnum(message, field, index)
                       : reflection->GetEnum(message, field);
        std::cout << enum_value->name() << " (" << enum_value->number() << ")"
                  << std::endl;
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
        std::string value =
            index >= 0 ? reflection->GetRepeatedString(message, field, index)
                       : reflection->GetString(message, field);
        std::cout << "\"" << value << "\"" << std::endl;
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
        const google::protobuf::Message& sub_message =
            index >= 0 ? reflection->GetRepeatedMessage(message, field, index)
                       : reflection->GetMessage(message, field);
        printMessage(sub_message, indent);
        break;
      }
      default:
        std::cout << "<unknown type>" << std::endl;
        break;
    }
  }

 public:
  // Process a message using dynamic protobuf
  bool processMessage(const mcap::MessageView& message_view) {
    // Find the schema for this message
    auto schema_it = schema_id_to_type_name_.find(message_view.schema->id);
    if (schema_it == schema_id_to_type_name_.end()) {
      std::cerr << "Unknown schema ID: " << message_view.schema->id
                << std::endl;
      return false;
    }

    const std::string& type_name = schema_it->second;

    // Find the descriptor for this message type
    auto desc_it = descriptors_.find(type_name);
    if (desc_it == descriptors_.end()) {
      std::cerr << "Unknown message type: " << type_name << std::endl;
      return false;
    }

    const google::protobuf::Descriptor* descriptor = desc_it->second;

    // Create a dynamic message
    std::unique_ptr<google::protobuf::Message> message(
        message_factory_->GetPrototype(descriptor)->New());

    // Parse the message data
    if (!message->ParseFromArray(
            message_view.message.data,
            static_cast<int>(message_view.message.dataSize))) {
      std::cerr << "Failed to parse message of type: " << type_name
                << std::endl;
      return false;
    }

    // Print message information
    std::cout << "\n=== Message Details ===" << std::endl;
    std::cout << "Channel: " << message_view.channel->topic << std::endl;
    std::cout << "Channel ID: " << message_view.channel->id << std::endl;
    std::cout << "Schema: " << message_view.schema->name << std::endl;
    std::cout << "Log Time: " << message_view.message.logTime << std::endl;
    std::cout << "Publish Time: " << message_view.message.publishTime
              << std::endl;
    std::cout << "Sequence: " << message_view.message.sequence << std::endl;
    std::cout << "Data Size: " << message_view.message.dataSize << " bytes"
              << std::endl;

    // Print the message content using reflection
    std::cout << "\n=== Message Content ===" << std::endl;
    printMessage(*message);
    std::cout << "========================\n" << std::endl;

    return true;
  }
};

int main() {
  mcap::McapReader reader;
  auto status = reader.open("output.mcap");
  if (!status.ok()) {
    std::cerr << "Failed to open MCAP file: " << status.message << "\n";
    return 1;
  }

  // Read summary first (required before accessing schemas)
  mcap::ReadSummaryMethod method;
  auto summary_status = reader.readSummary(method);
  if (!summary_status.ok()) {
    std::cerr << "Failed to read summary: " << summary_status.message
              << std::endl;
    return 1;
  }

  // Create our generic protobuf reader
  GenericProtobufReader protobuf_reader;

  // Register all schemas after reading summary
  if (!protobuf_reader.registerSchemas(reader)) {
    std::cerr << "Failed to register schemas" << std::endl;
    return 1;
  }

  if (reader.statistics()) {
    std::cout << "\n=== MCAP Statistics ===" << std::endl;
    std::cout << "Channel count: " << reader.statistics()->channelCount
              << std::endl;
    std::cout << "Message count: " << reader.statistics()->messageCount
              << std::endl;
    std::cout << "Message start time: " << reader.statistics()->messageStartTime
              << std::endl;
    std::cout << "Message end time: " << reader.statistics()->messageEndTime
              << std::endl;

    for (const auto& channel_count :
         reader.statistics()->channelMessageCounts) {
      std::cout << "Messages in channel " << channel_count.first << ": "
                << channel_count.second << std::endl;
    }
  }

  // Process all messages
  std::cout << "\n=== Processing Messages ===" << std::endl;
  auto message_views = reader.readMessages();

  int processed_count = 0;
  for (const auto& message_view : message_views) {
    std::cout << "Processing message with schema ID: "
              << message_view.schema->id
              << ", schema name: " << message_view.schema->name
              << ", encoding: " << message_view.schema->encoding << std::endl;
    if (message_view.schema->encoding == "protobuf") {
      if (protobuf_reader.processMessage(message_view)) {
        processed_count++;
      }
    } else {
      std::cout << "Skipping non-protobuf message on channel: "
                << message_view.channel->topic << std::endl;
    }
  }

  std::cout << "Successfully processed " << processed_count
            << " protobuf messages" << std::endl;

  reader.close();
  return 0;
}
