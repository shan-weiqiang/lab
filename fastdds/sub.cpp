#include "PayloadPubSubType.hpp"
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>
#include <fastdds/dds/topic/Topic.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/rtps/history/IPayloadPool.h>
#include <thread>

using namespace eprosima::fastdds::dds;

class HelloWorldListener : public DataReaderListener {
public:
  void on_data_available(DataReader *reader) override {
    eprosima::fastdds::rtps::SerializedPayload_t data;
    SampleInfo info;

    if (reader->take_next_sample(&data, &info) == RETCODE_OK) {
      if (info.valid_data) {
        std::cout << "Received message: " << data.length << std::endl;
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
  TypeSupport type(new utility::PayloadPubSubType());
  type->setName("HelloWorldModule::HelloWorld");
  type.register_type(participant);

  // Create a Topic
  Topic *topic = participant->create_topic("HelloWorldTopic", type->getName(),
                                           TOPIC_QOS_DEFAULT);

  // Create a Subscriber
  SubscriberQos subscriber_qos;
  Subscriber *subscriber = participant->create_subscriber(subscriber_qos);

  // Create a DataReader
  DataReaderQos reader_qos;
  HelloWorldListener listener;
  DataReader *reader =
      subscriber->create_datareader(topic, reader_qos, &listener);

  // Wait for data
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Cleanup
  participant->delete_contained_entities();
  DomainParticipantFactory::get_instance()->delete_participant(participant);

  return 0;
}