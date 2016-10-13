
#include <string>
#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>


#include <fastrtps/Domain.h>
#include <fastrtps/participant/Participant.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/publisher/Publisher.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastrtps/publisher/PublisherListener.h>
#include <fastrtps/subscriber/Subscriber.h>
#include <fastrtps/subscriber/SubscriberListener.h>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastrtps/rtps/RTPSDomain.h>
#include <fastrtps/rtps/builtin/data/WriterProxyData.h>
#include <fastrtps/rtps/common/CDRMessage_t.h>
#include <fastrtps/rtps/reader/RTPSReader.h>
#include <fastrtps/rtps/reader/StatefulReader.h>
#include <fastrtps/rtps/reader/ReaderListener.h>
#include <fastrtps/rtps/builtin/discovery/endpoint/EDPSimple.h>


//Definition of the probe listener -> Capture topic names and types

class probeReaderListener : public ReaderListener 
{
	public:
	probeReaderListener(){}

    void onNewCacheChangeAdded(RTPSReader* reader, const CacheChange_t* const change){
        (void)reader;
        if(change->kind == ALIVE){
			WriterProxyData proxyData;
			CDRMessage_t tempMsg;
			tempMsg.msg_endian = change->serializedPayload.encapsulation == PL_CDR_BE ? BIGEND:LITTLEEND;
			tempMsg.length = change->serializedPayload.length;
			memcpy(tempMsg.buffer,change->serializedPayload.data,tempMsg.length);
			if(proxyData.readFromCDRMessage(&tempMsg)){
				mapmutex.lock();
				topicNtypes[proxyData.topicName()].insert(proxyData.typeName());
                mapmutex.unlock();
			}
		}
	}

	std::map<std::string,std::set<std::string>> topicNtypes;
	std::mutex mapmutex;
};

int main(int argc, char *argv[])
{

int target_domainId;

    if(argc < 2){
        std::cout << "Usage: ddsprobe <domainId>" << std::endl;
        return 1;
    }else{
        try{
            target_domainId = std::stoi(argv[2]);       
        }catch(std::invalid_argument){
            std::cout << "Invalid argument. domainId must be a number" << std::endl;
            return 1;
        }
    }
    std::cout << "Target DomainId: " << std::to_string(target_domainId) << std::endl;
    //Create participant
    ParticipantAttributes Pparam;
    Pparam.rtps.builtin.domainId = target_domainId; //Temporarily set to 80
    
    Participant *observer = Domain::createParticipant(Pparam);
    if(!observer){
        std::cout << "Error while creating the observer participant." << std::endl;
        return 1;
    }
    //Attach probeReaderListener to the built-in protocols
    probeReaderListener* probe_1 = new probeReaderListener();
    probeReaderListener* probe_2 = new probeReaderListener();

    std::pair<StatefulReader*, StatefulReader*> EDPReaders = observer->getEDPReaders();

    if( !(EDPReaders.first->setListener(probe_1) & EDPReaders.second->setListener(probe_2)) ){
        std::cout << "Failed to attach probes to the observer participant" << std::endl;
        delete(probe_1);
        delete(probe_2);
        return 1;
    }
    //Give time for the discovery message exchanges to happen
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    //Access built-in discovery readers
    std::map<std::string, std::set<std::string>> unfiltered_topics;
    probe_1->mapmutex.lock();
    for(auto it: probe_1->topicNtypes){
        for(auto & itt: it.second){
            unfiltered_topics[it.first].insert(itt);
        }
    }
    probe_1->mapmutex.unlock();
    probe_2->mapmutex.lock();
    for(auto it: probe_2->topicNtypes){
        for(auto & itt: it.second){
            unfiltered_topics[it.first].insert(itt);
        }
    }
    probe_2->mapmutex.unlock();
    //Extract results
    std::map<std::string, std::string> topics;
    for(auto & it : unfiltered_topics){
        if(it.second.size() >= 1) topics[it.first] = *it.second.begin();
    }
    //Output
    std::cout << "Topic names and types report. Found "<< std::to_string(topics.size()) << "topics:" << std::endl;
    for(auto it : topics){
        std::cout << "  Name: '" << it.first << "', Type: '" << it.second << "'" << std::endl;
    }
    //Destruction
    EDPReaders.first->setListener(nullptr);
    EDPReaders.second->setListener(nullptr);
    delete(probe_1);
    delete(probe_2);

	return 0;
}
