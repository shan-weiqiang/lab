#include <fstream>
#include <iostream>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Set up the importer and factory
    google::protobuf::compiler::DiskSourceTree sourceTree;
    sourceTree.MapPath("", ".");
    google::protobuf::compiler::Importer importer(&sourceTree, nullptr);
    google::protobuf::DynamicMessageFactory factory;

    // Import the example.proto descriptor
    const google::protobuf::FileDescriptor* fileDescriptor = importer.Import("example.proto");

    // Get the descriptor for the Person message
    const google::protobuf::Descriptor* descriptor = importer.pool()->FindMessageTypeByName("example.Person");

    // Create a dynamic message based on the Person descriptor
    const google::protobuf::Message* prototype = factory.GetPrototype(descriptor);
    google::protobuf::Message* message = prototype->New();

    // Use reflection to populate the message
    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::FieldDescriptor* nameField = descriptor->FindFieldByName("name");
    const google::protobuf::FieldDescriptor* ageField = descriptor->FindFieldByName("age");
    reflection->SetString(message, nameField, "John Doe");
    reflection->SetInt32(message, ageField, 30);

    // Serialize the message to a file
    std::fstream output("person_out.pb", std::ios::out | std::ios::trunc | std::ios::binary);
    if (!message->SerializeToOstream(&output)) {
        std::cerr << "Failed to write message." << std::endl;
        return -1;
    }

    delete message;
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}