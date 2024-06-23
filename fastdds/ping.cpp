#include "msg/HelloWorld.hpp"
#include "msg/HelloWorldPubSubTypes.h"
#include <chrono>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <thread>

using namespace eprosima::fastdds::dds;
using namespace HelloWorldModule;

class echoHelloWorldListener : public DataReaderListener {
public:
  void on_data_available(DataReader *reader) override {
    HelloWorld data;
    SampleInfo info;

    if (reader->take_next_sample(&data, &info) == RETCODE_OK) {
      if (info.valid_data) {
        std::cout << "Received message: " << data.message()<< std::endl;
      }
    }
  }
};

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

  Topic *echo_topic = participant->create_topic("echoHelloWorldTopic", type->getName(),
                                           TOPIC_QOS_DEFAULT);

  // Create a Publisher
  PublisherQos publisher_qos;
  Publisher *publisher = participant->create_publisher(publisher_qos);

  SubscriberQos subscriber_qos;
  Subscriber * subscriber = participant->create_subscriber(subscriber_qos);

  // Create a DataWriter
  DataWriterQos writer_qos;
  DataWriter *writer = publisher->create_datawriter(topic, writer_qos);

  DataReaderQos reader_qos;
  echoHelloWorldListener listener;
  DataReader * reader = subscriber->create_datareader(echo_topic, reader_qos, &listener);

  // Create a sample data
  HelloWorld hello;
  hello.message("Hello Fast DDS!");

  // Write the data
  for (;;) {
    writer->write(&hello);
    std::cout << "Send message: " << hello.message() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // Cleanup
  participant->delete_contained_entities();
  DomainParticipantFactory::get_instance()->delete_participant(participant);

  return 0;
}