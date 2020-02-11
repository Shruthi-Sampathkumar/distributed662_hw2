#include <iostream>
#include <memory>
//#include <thread>
//#include <vector>
#include <string>
#include <unistd.h>
#include <grpc++/grpc++.h>
#include "client.h"
#include <thread>
#include <chrono>
#include <ctime>

#include "timeline.grpc.pb.h"
#include "timeline.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using grpc::StatusCode;

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
    std::string username = "xyz";
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
    stub_ = social_network::NewStub(grpc::CreateChannel("localhost:50051",
                                                        grpc::InsecureChannelCredentials()));
    
    //create_user
    IReply ire;
    ClientContext context;
    user user1;
    user1.set_name(username);
    follow_response f1_response;
    
    Status status = stub_->addUser(&context, user1, &f1_response);
    
    if (!status.ok())
    {
        ire.comm_status = FAILURE_ALREADY_EXISTS;
        std::cout << "addUser rpc failed." << std::endl;
        return -1;
    }
    else
    {
        ire.comm_status = SUCCESS;
        std::cout << "User added to database successfully " << std::endl;
    }
    std::cout << "connectTo() is successful at the client" << std::endl;
    return 1; // return 1 if success, otherwise return -1
}

IReply Client::processCommand(std::string& input)
{
    //parsing the input. 'newString1' will contain the parsed result
    int j=0, ctr=0;
    char newString1 [2][20];
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
    char* command = newString1[0];
    
    if (strcasecmp(command, "FOLLOW")==0)
    {
        std::string u2 = newString1[1];
        
        follow_request f1_request;
        f1_request.set_user1(username);
        f1_request.set_user2(u2);
        
        ClientContext context;
        follow_response f1_response;
        
        Status status = stub_->addTo(&context, f1_request, &f1_response);
        ire.grpc_status = status;
        if (!status.ok())
        {
            if (f1_response.success_status()==1)
            {
                ire.grpc_status=Status::OK;
                ire.comm_status = FAILURE_ALREADY_EXISTS;
            }
            else if (f1_response.success_status()==2)
            {
                ire.grpc_status=Status::OK;
                ire.comm_status = FAILURE_INVALID_USERNAME;
            }
        }
        else
        {
            ire.comm_status = SUCCESS;
        }
        
    }
    else if (strcasecmp(command, "UNFOLLOW")==0)
    {
        std::string u2 = newString1[1];
        
        
        unfollow_request f2_request;
        f2_request.set_user1(username);
        f2_request.set_user2(u2);
        
        ClientContext context;
        unfollow_response f2_response;
        
        Status status = stub_->removeFrom(&context, f2_request, &f2_response);
        ire.grpc_status = status;
        if (!status.ok())
        {
            ire.comm_status = FAILURE_UNKNOWN;
            std::cout << "removeFrom rpc failed : " << std::endl;
        }
        else
        {
            if (f2_response.success_status()==1)
            {
                ire.comm_status = FAILURE_INVALID_USERNAME;
            }
            else if (f2_response.success_status()==0)
            {
                ire.comm_status = SUCCESS;
            }
        }
    }
    
    else if (strcasecmp(command, "LIST")==0)
    {
        
        list_request l_request;
        l_request.set_user1(username);
        
        ClientContext context;
        list_response l_response;
        
        Status status = stub_->getFollowersUsers(&context, l_request, &l_response);
        ire.grpc_status = status;
        if (!status.ok() or l_response.success_status()!=0)
        {
            ire.comm_status = FAILURE_UNKNOWN;
            std::cout << "getFollowersUsers rpc failed." << std::endl;
        }
        else
        {
            if (!l_response.success_status())
            {
                ire.comm_status = SUCCESS;
                std::vector<std::string> following_users(l_response.followers().begin(), l_response.followers().end());
                
                std::vector<std::string> all_users(l_response.active_users().begin(), l_response.active_users().end());
                
                ire.following_users = following_users;
                ire.all_users = all_users;
            }
            else
            {
                ire.comm_status = FAILURE_UNKNOWN;
            }
        }
        
    }
    
    //if the command is timeline
    else if (strcasecmp(command,"TIMELINE")==0)
    {
        
        ire.comm_status = SUCCESS;
        ire.grpc_status = Status::OK;
        
        
    }
    
    //if the command is invalid
    else
    {
        ire.comm_status = FAILURE_INVALID;
        ire.grpc_status =  Status(StatusCode::INVALID_ARGUMENT, "Invalid Command!");
    }
    
    return ire;
}

void Client::processTimeline()
{
    ClientContext context;
    
    //creating metadata with the name of the user
    context.AddMetadata("user", username.c_str());
    
    //creating a stream object
    std::shared_ptr<ClientReaderWriter<post, post> > stream(
        stub_->updates(&context));
    
    std::string u = username.c_str();
    
    //getting an input from the user (post) and sending it to the server
    std::thread writer([stream, u]()
    {
        while (1)
        {
            post post1;
            //getting an input post from user
            std::string new_post = getPostMessage();
            
            //setting the post message type with data
            new_post.erase(std::remove(new_post.begin(), new_post.end(), '\n'),
            new_post.end());
            post1.set_content(new_post);
            post1.set_owner(u);
            auto t = std::chrono::system_clock::now();
            std::time_t t1 = std::chrono::system_clock::to_time_t(t);
            post1.set_timestamp(std::ctime(&t1));
            
            //sending data to server
            stream->Write(post1);
        }
        stream->WritesDone();
    });
    
    
    //reading new posts from the following users
    std::thread reader([stream]()
    {
            post p;
            while(stream->Read(&p))
            {
                //struct tm tm;
                //strptime(p.timestamp().c_str(), "%a %b %d %OH:%M:%OS %Y", &tm);
                //time_t t = mktime(&tm);
                auto t = std::chrono::system_clock::now();
                std::time_t t1 = std::chrono::system_clock::to_time_t(t);
                //displaying the post to the user in timeline mode
                displayPostMessage(p.owner(), p.content(), t1);
                
            }
    });
    
    //Wait for the threads to finish
    writer.join();
    reader.join();
    
    Status status = stream->Finish();
    if (!status.ok())
    {
      std::cout << "RouteChat rpc failed." << std::endl;
    }
     
}
