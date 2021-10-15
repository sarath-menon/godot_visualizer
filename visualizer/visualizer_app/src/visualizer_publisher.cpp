#include "visualizer_publisher.h"



using namespace eprosima::fastdds::dds;

DDSPublisher::~DDSPublisher() {
  if (writer_ != nullptr) {
    publisher_->delete_datawriter(writer_);
  }
  if (publisher_ != nullptr) {
    participant_->delete_publisher(publisher_);
  }
  if (topic_ != nullptr) {
    participant_->delete_topic(topic_);
  }
  DomainParticipantFactory::get_instance()->delete_participant(participant_);
}

bool DDSPublisher::init() {
  /* Initialize data_ here */

  // CREATE THE PARTICIPANT
  DomainParticipantQos pqos;
  pqos.name("Participant_pub");
  participant_ =
      DomainParticipantFactory::get_instance()->create_participant(0, pqos);
  if (participant_ == nullptr) {
    return false;
  }

  // REGISTER THE TYPE
  type_.register_type(participant_);

  // CREATE THE PUBLISHER
  publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
  if (publisher_ == nullptr) {
    return false;
  }

  // CREATE THE TOPIC
  topic_ = participant_->create_topic("sim_img", type_.get_type_name(),
                                      TOPIC_QOS_DEFAULT);
  if (topic_ == nullptr) {
    return false;
  }

  // CREATE THE WRITER
  writer_ =
      publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT, &listener);
  if (writer_ == nullptr) {
    return false;
  }

  std::cout << "Position DataWriter created." << std::endl;
  return true;
}

void DDSPublisher::PubListener::on_publication_matched(
    eprosima::fastdds::dds::DataWriter *,
    const eprosima::fastdds::dds::PublicationMatchedStatus &info) {
  if (info.current_count_change == 1) {
    matched_ = info.total_count;
    std::cout << "DataWriter matched." << std::endl;
  } else if (info.current_count_change == -1) {
    matched_ = info.total_count;
    std::cout << "DataWriter unmatched." << std::endl;
  } else {
    std::cout << info.current_count_change
              << " is not a valid value for PublicationMatchedStatus current "
                 "count change"
              << std::endl;
  }
}
