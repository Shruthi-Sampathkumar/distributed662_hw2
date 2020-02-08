//
//  tsd.cpp
//  
//
//  Created by Shruthi Sampath Kumar on 2/2/20.
//

#include <stdio.h>
#include <iostream>
#include <memory>
#include <string>
#include <grpc++/grpc++.h>
#include <bits/stdc++.h>
#include <unordered_map>
#include <grpc++/support/string_ref.h>
//#include <mutex>
//#include <json/value.h>
//#include <jsoncpp/json/json.h>
//#include "/home/csce438/grpc/src/core/lib/json/json.h"
#include "/home/csce438/grpc/third_party/protobuf/conformance/third_party/jsoncpp/jsoncpp.cpp"
#include "/home/csce438/grpc/third_party/protobuf/conformance/third_party/jsoncpp/json.h"
//#include </usr/include/jsoncpp/json/json.h>
#include <fstream>
//using json = nlohmann::json;

#include "timeline.grpc.pb.h"
#include "timeline.pb.h"


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::StatusCode;
using grpc::ServerReaderWriter;


using timeline::social_network;
using timeline::user;
using timeline::post;
using timeline::follow_request;
using timeline::follow_response;
using timeline::unfollow_request;
using timeline::unfollow_response;
using timeline::list_request;
using timeline::list_response;
//using timeline::addTo;
//using timeline::removeFrom;
//using timeline::getFollowersUsers;


class timelineImpl final : public social_network::Service {
public:
    
    std::unordered_map<std::string, ServerReaderWriter<post, post>* > members;
    //std::mutex mutex_users, mutex_timeline;
    //std::unique_lock<std::mutex> lock_users(mutex_users, std::defer_lock);
    //std::unique_lock<std::mutex> lock_timeline(mutex_timeline, std::defer_lock);
    
    
    //add user to the database
    Status addUser(ServerContext* context,const user* user1,
    follow_response* response)
    {
        //reading the timeline json file
        std::ifstream timeline_read("timeline.json", std::ifstream::binary);
        if (!timeline_read.is_open())
        {
          std::cout << "Failed to open timeline.json " << std::endl;
            response->set_success_status(1);
            return Status::OK;
          //return Status(StatusCode::INVALID_ARGUMENT, "Cannot open users.json file!");
        }
        
        //reading the users json file
        std::ifstream ip_users_file("users.json", std::ifstream::binary);
        if (!ip_users_file.is_open())
        {
          std::cout << "Failed to open users.json " << std::endl;
            response->set_success_status(1);
            return Status::OK;
          //return Status(StatusCode::INVALID_ARGUMENT, "Cannot open users.json file!");
        }
        
        //Json::Value users;
        //Json::Reader reader;
        //reader.parse(ip_users_file, users);
        
        
        Json::Reader reader1, reader2;
        Json::Value timeline_parsed, users;
        
        reader1.parse(timeline_read, timeline_parsed);
        reader2.parse(ip_users_file, users);
        
        std::string u1 = user1->name();
        
        //check if the user already is existing in the database
        if (!users["users"].isMember(u1))
        {
            
            //adding an entry for the user in the users database
            Json::Value followers(Json::arrayValue);
            Json::Value following(Json::arrayValue);
            //adding self follower and followee
            following.append(u1);
            followers.append(u1);
            
            users["users"][u1]["name"] = u1;
            users["users"][u1]["following"] = following;
            users["users"][u1]["followers"] = followers;
            //dumping the json object in users json file
            std::ofstream op_users_file("users.json");
            op_users_file << std::setw(4) << users << std::endl;
            
            
            
            //adding an entry for the user in the timeline database
            
            Json::Value timeline_content(Json::arrayValue);
            timeline_parsed[u1] = timeline_content;
            //dumping the json object in timeline json file
            std::ofstream op_timeline_file("timeline.json");
            op_timeline_file << std::setw(4) << timeline_parsed << std::endl;
            
            
        }
        
        response->set_success_status(0);
        
        return Status::OK;
        
    }
    
    
    //if the command was "FOLLOW"
    Status addTo(ServerContext* context, const follow_request* request,
                    follow_response* response)
    {
        //reading the json file
        std::cout << "Command received from the client is FOLLOW " << std::endl;
        std::ifstream ip_users_file("users.json", std::ifstream::binary);
        if (!ip_users_file.is_open())
        {
          std::cout << "Failed to open users.json " << std::endl;
          return Status::OK;
          //return Status(StatusCode::INVALID_ARGUMENT, "Cannot open users.json file!");
        }
        
        Json::Reader reader;
        Json::Value users;
        reader.parse(ip_users_file, users);
        
        std::string u1 = request->user1();
        std::string u2 = request->user2();
        
        if (users["users"].isMember(u1) and users["users"].isMember(u2))
        {
            users["users"][u1]["following"].append(u2);
            std::cout << "Added Following " << std::endl;
            
            users["users"][u2]["followers"].append(u1);
            std::cout << "Added Followers " << std::endl;
            std::ofstream op_users_file("users.json");
            op_users_file << std::setw(4) << users << std::endl;
            response->set_success_status(0);
            return Status::OK;
            
        }
        else
        {
            response->set_success_status(1);
        }
        return Status::OK;
        
    }
    
    //if the command was "UNFOLLOW"
    Status removeFrom(ServerContext* context, const unfollow_request* request,
    unfollow_response* response)
    {
        //reading the json file
        std::ifstream ip_users_file("users.json", std::ifstream::binary);
        if (!ip_users_file.is_open())
        {
          std::cout << "Failed to open users.json " << std::endl;
          return Status::OK;
          //return Status(StatusCode::INVALID_ARGUMENT, "Cannot open users.json file!");
        }
        
        Json::Reader reader;
        Json::Value users;
        reader.parse(ip_users_file, users);
        
        std::string u1 = request->user1();
        std::string u2 = request->user2();
        
        if (users["users"].isMember(u1) and users["users"].isMember(u2) and !(u1==u2))
          {
              //removing the user2 from following list of user1
              Json::Value new_items = Json::arrayValue;
              Json::Value new_followers = Json::arrayValue;
              int c = 0;
              for(int i = 0; i<users["users"][u1]["following"].size(); i++)
              {
                  if(users["users"][u1]["following"][i].compare(u2) != 0)
                  {
                      new_items[c] = users["users"][u1]["following"][i];
                      c++;
                  }
              }
              users["users"][u1]["following"] = new_items;
              
              //removing user1 from followers list of user2
              int d = 0;
              for(int i = 0; i<users["users"][u2]["followers"].size(); i++)
              {
                  if(users["users"][u2]["followers"][i].compare(u2) != 0){
                      new_followers[d] = users["users"][u1]["followers"][i];
                      d++;
                  }
              }
              users["users"][u2]["followers"] = new_followers;
              
              std::ofstream op_users_file("users.json");
              op_users_file << std::setw(4) << users << std::endl;
              response->set_success_status(0);
              return Status::OK;
          }
          else
          {
              response->set_success_status(1);
              return Status::OK;
              //return Status(StatusCode::INVALID_ARGUMENT, "Invalid user input!");
          }
    }
    
    //if the command was "LIST"
    Status getFollowersUsers(ServerContext* context, const list_request* request,
    list_response* response)
    {
        //reading the json file
        std::ifstream ip_users_file("users.json", std::ifstream::binary);
        if (!ip_users_file.is_open())
        {
          std::cout << "Failed to open users.json " << std::endl;
            return Status::OK;
          //return Status(StatusCode::INVALID_ARGUMENT, "Cannot open users.json file!");
        
        }
        
        Json::Reader reader;
        Json::Value users;
        reader.parse(ip_users_file, users);
        std::string u1 = request->user1();
        
        if (users["users"].isMember(u1))
          {
              //setting the response variable
              response->set_success_status(0);
              
              std::cout << "Json file contains  " << u1 << std::endl;
              std::cout << "The followers are : " << std::endl;
              
              Json::Value names= users["users"][u1]["followers"];
              for (const auto& element : names)
              {
                  std::string value = element.asString();
                  std::cout << value << std::endl;
                  response->add_followers(value);
                  
              }
              
              Json::Value  v = users["users"];
              //the keys (names) in users json file is extracted
              Json::Value::Members active_users = v.getMemberNames();
              std::cout << "The active members are " << std::endl;
              for (const auto& element : active_users)
              {
                  std::string value = element;
                  std::cout << value << std::endl;
                  response->add_active_users(value);
                  
              }
              return Status::OK;
          }
          else
          {
              //set the response variable to indicate failure
              response->set_success_status(1);
              return Status::OK;
              //return Status(StatusCode::INVALID_ARGUMENT, "Invalid user argument!");
          }
          
        
    }
    
    
    Status updates(ServerContext* context, ServerReaderWriter<post, post>* stream)
    {
        //members[current_members].name
        //std::vector<post> received_posts;
        grpc::string_ref user_ref = context->client_metadata().find("user")->second;
        std::string user(user_ref.begin(), user_ref.end());
        
        //add the user stream
        members[user]=stream;
        
        post post1;
        
        while (stream->Read(&post1))
        {
            //post post1;
            std::string u1 = user;
            
            //reading the timeline json file
            std::ifstream timeline_read("timeline.json", std::ifstream::binary);
            if (!timeline_read.is_open())
            {
              std::cout << "Failed to open timeline.json " << std::endl;
                return Status::OK;
              //return Status(StatusCode::INVALID_ARGUMENT, "Cannot open users.json file!");
            }
            
            //reading the users json file
            std::ifstream ip_users_file("users.json", std::ifstream::binary);
            if (!ip_users_file.is_open())
            {
              std::cout << "Failed to open users.json " << std::endl;
              return Status::OK;
              //return Status(StatusCode::INVALID_ARGUMENT, "Cannot open timeline.json file!");
            }
            
            Json::Reader reader1, reader2;
            Json::Value timeline_parsed, users;
            
            reader1.parse(timeline_read, timeline_parsed);
            reader2.parse(ip_users_file, users);
            
            
            //check if the posting user is a part of the users database and then carry out the update actions
            if (users["users"].isMember(u1))
            {
                
                Json::Value followers = users["users"][u1]["followers"];
                
                for (const auto& element : followers)
                {
                    if (!element)
                    {
                        continue;
                    }
                    std::string value = element.asString();
                    std::cout << "Processing for the follower : " << value << std::endl;
                    Json::Value updated_timeline = Json::arrayValue;
                    
                    //updating the timeline json object of the followers
                    //std::string tmp = post->content();
                    updated_timeline[0] = post1.content();
                    
                    int c = 1;
                    for(int i = 0; i<timeline_parsed[value].size() && c<20; i++)
                    {
                        updated_timeline[c] = timeline_parsed[value][i];
                        c++;
                    }
                    
                    timeline_parsed[value] = updated_timeline;
                    
                    //if the user is currently in timeline mode, display the updated timeline
                    if (members.find(value)!=members.end())
                    {
                        for(int i = 0; i<timeline_parsed[value].size(); i++)
                        {
                            post tmp;
                            tmp.set_content(timeline_parsed[value][i].asString());
                            members[value]->Write(tmp);
                        }
                    }
                    
                }
                
                //writing the updated timeline json object to timeline database
                std::ofstream op_timeline_file("timeline.json");
                op_timeline_file << std::setw(4) << timeline_parsed << std::endl;
                return Status::OK;
            }
        }
        
        //return Status(StatusCode::INVALID_ARGUMENT, "Invalid user input!");
        return Status::OK;
    }
};




void RunServer() {
  std::string server_address("0.0.0.0:50051");
    
  timelineImpl service;
  ServerBuilder builder;
    
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
    
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}





int main(int argc, char** argv) {
  RunServer();
  return 0;
}
