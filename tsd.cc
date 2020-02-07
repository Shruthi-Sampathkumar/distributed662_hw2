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
    //add user to the database
    Status addUser(ServerContext* context,const user* user1,
    follow_response* response)
    {
        std::ifstream ip_users_file("users.json", std::ifstream::binary);
        if (!ip_users_file.is_open())
        {
          std::cout << "Failed to open users.json " << std::endl;
          return Status::OK;
        }
        
        Json::Value users;
        Json::Reader reader;
        reader.parse(ip_users_file, users);
        
        std::string u1 = user1->name();
        
        //check if the user already is existing in the database
        if (!users["users"].isMember(u1))
        {
            Json::Value followers(Json::arrayValue);
            Json::Value following(Json::arrayValue);
            
            users["users"][u1]["name"] = u1;
            users["users"][u1]["following"] = following;
            users["users"][u1]["followers"] = followers;
            
            std::ofstream op_users_file("users.json");
            op_users_file << std::setw(4) << users << std::endl;
            
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
        }
        
        Json::Reader reader;
        Json::Value users;
        reader.parse(ip_users_file, users);
        
        std::string u1 = request->user1();
        std::string u2 = request->user2();
        
        if (users["users"].isMember(u1) and users["users"].isMember(u2))
          {
              std::cout << "Json contains both in unfollow" << std::endl;
              users["users"][u1]["following"].removeMember(u2);
              users["users"][u2]["followers"].removeMember(u1);
              std::ofstream op_users_file("users.json");
              op_users_file << std::setw(4) << users << std::endl;
              response->set_success_status(0);
          }
          else
          {
              response->set_success_status(1);
          }
          
        return Status::OK;
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
          }
          else
          {
              //set the response variable to indicate failure
              response->set_success_status(1);
          }
          
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
