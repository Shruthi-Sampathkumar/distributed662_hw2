#include <iostream>
#include <memory>
//#include <thread>
//#include <vector>
#include <string>
#include <unistd.h>
#include <grpc++/grpc++.h>
#include "client.h"

#include "timeline.grpc.pb.h"
#include "timeline.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using timeline::social_network;
using timeline::user;
using timeline::post;
using timeline::follow_request;
using timeline::follow_response;
using timeline::unfollow_request;
using timeline::unfollow_response;
using timeline::list_request;
using timeline::list_response;

class Client : public IClient
{
    public:
        Client(const std::string& hname,
               const std::string& uname,
               const std::string& p)
            :hostname(hname), username(uname), port(p)
            {}
    //create a stub for the service social_network
    
    
    //Client(std::shared_ptr<Channel> channel)
        //: stub_(social_network::NewStub(channel)) {}
    

    protected:
        virtual int connectTo();
        virtual IReply processCommand(std::string& input);
        virtual void processTimeline();
    private:
        std::string hostname;
        std::string username;
        std::string port;
        
        // You can have an instance of the client stub
        // as a member variable.
        //std::unique_ptr<NameOfYourStubClass::Stub> stub_;
        std::unique_ptr<social_network::Stub> stub_;
};

int main(int argc, char** argv) {

    std::string hostname = "localhost";
    std::string username = "bbb";
    std::string port = "3010";
    int opt = 0;
    while ((opt = getopt(argc, argv, "h:u:p:")) != -1){
        switch(opt) {
            case 'h':
                hostname = optarg;break;
            case 'u':
                username = optarg;break;
            case 'p':
                port = optarg;break;
            default:
                std::cerr << "Invalid Command Line Argument\n";
        }
    }

    Client myc(hostname, username, port);
    // You MUST invoke "run_client" function to start business logic
    //this run() function in the interface runs the logic
    myc.run_client();

    return 0;
}

int Client::connectTo()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to create a stub so that
    // you call service methods in the processCommand/porcessTimeline
    // functions. That is, the stub should be accessible when you want
    // to call any service methods in those functions.
    // I recommend you to have the stub as
    // a member variable in your own Client class.
    // Please refer to gRpc tutorial how to create a stub.
	// ------------------------------------------------------------
    stub_ = social_network::NewStub(grpc::CreateChannel("localhost:50051",
                                                        grpc::InsecureChannelCredentials()));
    
    //create_user
    IReply ire;
    ClientContext context;
    user user1;
    user1.set_name(this->username);
    follow_response f1_response;
    
    Status status = stub_->addUser(&context, user1, &f1_response);
    
    if (!status.ok())
    {
        ire.comm_status = FAILURE_INVALID;
        std::cout << "addUser rpc failed." << std::endl;
        //return false;
    }
    else
    {
        ire.comm_status = SUCCESS;
        std::cout << "User added to database successfully " << std::endl;
    //return true;
    }
    std::cout << "connectTo() is successful at the client" << std::endl;
    return 1; // return 1 if success, otherwise return -1
}

IReply Client::processCommand(std::string& input)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In this function, you are supposed to parse the given input
    // command and create your own message so that you call an 
    // appropriate service method. The input command will be one
    // of the followings:
	//
	// FOLLOW <username>
	// UNFOLLOW <username>
	// LIST
    // TIMELINE
	//
	// - JOIN/LEAVE and "<username>" are separated by one space.
	// ------------------------------------------------------------
    
    //parsing the input. 'newString1' will contain the parsed result
    std::cout << "The input command is : " << input << std::endl;
    int j=0, ctr=0;
    char newString1 [3][20];
    for(int i=0;i<=(input.length());i++)
    {
        // if space or NULL found, assign NULL into newString[ctr]
        if(input[i]==' '||input[i]=='\0')
        {
            newString1[ctr][j]='\0';
            ctr++;  //for next word
            j=0;    //for next word, init index to 0
        }
        else
        {
            newString1[ctr][j]=input[i];
            j++;
        }
    }
    
    IReply ire;
    //command = newString1[0];
    char* command = newString1[0];
    
    if (strcmp(command, "FOLLOW")==0)
    {
        //std::vector<std::string> u1 = username;
        //std::vector<std::string> u2 = newString1[1];
        std::string u1 = username;
        std::string u2 = newString1[1];

        user user1, user2;
	    user1.set_name(u1);
        user2.set_name(u2);
        
        follow_request f1_request;
        f1_request.set_allocated_user1(&user1);
        f1_request.set_allocated_user2(&user2);
        
        ClientContext context;
        follow_response f1_response;
        
        Status status = stub_->addTo(&context, f1_request, &f1_response);
        ire.grpc_status = status;
        if (!status.ok())
        {
            ire.comm_status = FAILURE_INVALID;
            std::cout << "addTo rpc failed." << std::endl;
            //return false;
        }
        else
        {
            ire.comm_status = SUCCESS;
            std::cout << "Follow request successful : " << f1_response.success_status()  << std::endl;
	    //return true;
        }
        
        }
    else if (strcmp(command, "UNFOLLOW")==0)
    {
        //std::vector<std::string> u1 = username;
        //std::vector<std::string> u2 = newString1[1];
        std::string u1 = username;
        std::string u2 = newString1[1];
        
        user user1, user2;
        user1.set_name(u1);
        user2.set_name(u2);
        
        unfollow_request f2_request;
        f2_request.set_allocated_user1(&user1);
        f2_request.set_allocated_user2(&user2);
        
        ClientContext context;
        unfollow_response f2_response;
        
        Status status = stub_->removeFrom(&context, f2_request, &f2_response);
        ire.grpc_status = status;
        if (!status.ok())
        {
            ire.comm_status = FAILURE_INVALID;
            std::cout << "removeFrom rpc failed : " << std::endl;
            //return false;
        }
        else
        {
            ire.comm_status = SUCCESS;
            std::cout << "Unfollow request successful : " << f2_response.success_status() << std::endl;
	    //return true;
        }
    }
    
    else if (strcmp(command, "LIST")==0)
    {
        //std::vector<std::string> u1 = username;
        std::string u1 = username;
        
        user user1;
        user1.set_name(u1);
        
        //user user1 = set_allocated_user1(u1);
        
        list_request l_request;
        l_request.set_allocated_user1(&user1);
        
        ClientContext context;
        list_response l_response;
        
        Status status = stub_->getFollowersUsers(&context, l_request, &l_response);
        ire.grpc_status = status;
        if (!status.ok() or l_response.success_status()!=0)
        {
            ire.comm_status = FAILURE_INVALID;
            std::cout << "getFollowersUsers rpc failed." << std::endl;
            //return false;
        }
        else
        {
            //setting the iReply fields
            ire.comm_status = SUCCESS;
            std::cout << "List request successful " << std::endl;
            std::vector<std::string> following_users(l_response.followers().begin(), l_response.followers().end());
            
            std::vector<std::string> all_users(l_response.active_users().begin(), l_response.active_users().end());
            
            ire.following_users = following_users;
            ire.all_users = all_users;
            std::cout << "List request successful " << std::endl;
	    //return true;
        }
    }
    // ------------------------------------------------------------
	// GUIDE 2:
	// Then, you should create a variable of IReply structure
	// provided by the client.h and initialize it according to
	// the result. Finally you can finish this function by returning
    // the IReply.
	// ------------------------------------------------------------
    
	// ------------------------------------------------------------
    // HINT: How to set the IReply?
    // Suppose you have "Join" service method for JOIN command,
    // IReply can be set as follow:
    // 
    //     // some codes for creating/initializing parameters for
    //     // service method
    //     IReply ire;
    //     grpc::Status status = stub_->Join(&context, /* some parameters */);
    //     ire.grpc_status = status;
    //     if (status.ok()) {
    //         ire.comm_status = SUCCESS;
    //     } else {
    //         ire.comm_status = FAILURE_NOT_EXISTS;
    //     }
    //      
    //      return ire;
    // 
    // IMPORTANT: 
    // For the command "LIST", you should set both "all_users" and 
    // "following_users" member variable of IReply.
	// ------------------------------------------------------------
    return ire;
}

void Client::processTimeline()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to get into timeline mode.
    // You may need to call a service method to communicate with
    // the server. Use getPostMessage/displayPostMessage functions
    // for both getting and displaying messages in timeline mode.
    // You should use them as you did in hw1.
	// ------------------------------------------------------------

    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    //
    // Once a user enter to timeline mode , there is no way
    // to command mode. You don't have to worry about this situation,
    // and you can terminate the client program by pressing
    // CTRL-C (SIGINT)
	// ------------------------------------------------------------
    
}
