
#include "PayloadPubSubType.hpp"
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/qos/WriterQos.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/rtps/history/IPayloadPool.h>
#include <memory>
#include <thread>

using namespace eprosima::fastdds::dds;

class HelloWorldListener : public DataReaderListener {
public:
  HelloWorldListener(DataWriter *echo_writer) : writer_{echo_writer} {}
  void on_data_available(DataReader *reader) override {
    eprosima::fastdds::rtps::SerializedPayload_t data;
    SampleInfo info;

    if (reader->take_next_sample(&data, &info) == RETCODE_OK) {
      if (info.valid_data) {
        std::cout << "Received message: " << data.length << std::endl;
        // echo back payload data
        writer_->write(&data);
      }
    }
  }
  DataWriter *writer_;
};

int main() {
  // Create a DomainParticipant
  DomainParticipantQos participant_qos;
  DomainParticipant *participant =
      DomainParticipantFactory::get_instance()->create_participant(
          0, participant_qos);

  // Register the type
  TypeSupport type(new utility::PayloadPubSubType());
  type->setName("HelloWorldModule::HelloWorld");
  type.register_type(participant);

  // Create a Topic
  Topic *topic = participant->create_topic("HelloWorldTopic", type->getName(),
                                           TOPIC_QOS_DEFAULT);
  Topic *echo_topic = participant->create_topic(
      "echoHelloWorldTopic", type->getName(), TOPIC_QOS_DEFAULT);
  // Create a Subscriber
  SubscriberQos subscriber_qos;
  Subscriber *subscriber = participant->create_subscriber(subscriber_qos);

  PublisherQos pubscriber_qos;
  Publisher *publisher = participant->create_publisher(pubscriber_qos);
  DataWriterQos writer_qos;
  DataWriter *writer = publisher->create_datawriter(echo_topic, writer_qos);

  // Create a DataReader
  DataReaderQos reader_qos;
  HelloWorldListener listener{writer};
  DataReader *reader =
      subscriber->create_datareader(topic, reader_qos, &listener);

  // Wait for data
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Cleanup
  participant->delete_subscriber(subscriber);
  participant->delete_contained_entities();
  DomainParticipantFactory::get_instance()->delete_participant(participant);

  return 0;
}