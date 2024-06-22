#include "msg/HelloWorldPubSubTypes.h"
#include <chrono>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <thread>

using namespace eprosima::fastdds::dds;
using namespace HelloWorldModule;

int main() {
  // Create a DomainParticipant
  DomainParticipantQos participant_qos;
  DomainParticipant *participant =
      DomainParticipantFactory::get_instance()->create_participant(
          0, participant_qos);

  // Register the type
  TypeSupport type(new HelloWorldPubSubType());
  type.register_type(participant);

  // Create a Topic
  Topic *topic = participant->create_topic("HelloWorldTopic", type->getName(),
                                           TOPIC_QOS_DEFAULT);

  // Create a Publisher
  PublisherQos publisher_qos;
  Publisher *publisher = participant->create_publisher(publisher_qos);

  // Create a DataWriter
  DataWriterQos writer_qos;
  DataWriter *writer = publisher->create_datawriter(topic, writer_qos);

  // Create a sample data
  HelloWorld hello;
  hello.message("Hello Fast DDS!");

  // Write the data
  for (;;) {
    writer->write(&hello);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // Cleanup
  participant->delete_contained_entities();
  DomainParticipantFactory::get_instance()->delete_participant(participant);

  return 0;
}