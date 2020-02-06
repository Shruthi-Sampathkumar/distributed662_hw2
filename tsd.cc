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
 //explicit timelineImpl(const std::string& db) {
   //timeline::ParseDb(db, &feature_list_);
//}
    
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
        
        Json::Value followers(Json::arrayValue);
        Json::Value following(Json::arrayValue);
        std::string u1 = user1->name();
        
        users["users"][u1]["name"] = u1;
        users["users"][u1]["following"] = following;
        users["users"][u1]["followers"] = followers;
        
        std::ofstream op_users_file("users.json");
        op_users_file << std::setw(4) << users << std::endl;
        
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
        else
        {
            std::cout << "The json file is open " << std::endl;
        }
        
        Json::Reader reader;
        Json::Value users;
        reader.parse(ip_users_file, users);
        
        //std::cout << "The json file is parsed " << std::endl;
        
        std::string u1 = request->user1().name();
        std::string u2 = request->user2().name();
        
        if (users["users"].isMember(u1) and users["users"].isMember(u2))
        {
            //std::cout << "The jon file contains user1 and user2 " << std::endl;
            users["users"][u1]["following"].append(u2);
            users["users"][u2]["followers"].append(u1);
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
        
        
        std::string u1 = request->user1().name();
        std::string u2 = request->user2().name();
        
        if (users.isMember(u1) and users.isMember(u2))
          {
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
        
        std::string u1 = request->user1().name();
        
        if (users.isMember(u1))
          {
              response->set_success_status(0);
              
              Json::Value current_followers = users[u1]["followers"];
              std::cout << "The followers are : " << std::endl;
              
              for (const auto& element : users[u1]["followers"])
              {
                  
                  //std::string value = element.get<std::string>();
                  std::string value = element.asString();
                  std::cout << value << std::endl;
                  response->add_followers(value);
                  
              }
              //for (Json::Value::iterator t=current_followers.begin(); t!=current_followers.end(); ++t)
              //{
                  //Json::FastWriter fastWriter;
                  //std::string output = t.asString();
                  //response->add_followers(output);
              //}
              
              //for (Json::Value::ArrayIndex i = 0; i != users.size(); i++)
              //{
              Json::Value  v = users["users"];
                  //number _of_users = v.size();
              //the keys (names) in users json file is extracted
              Json::Value::Members active_users = v.getMemberNames();
              //setting the response variable
              for (const auto& element : active_users)
              {
                  
                  //std::string value = element.get<std::string>();
                  std::string value = element;
                  response->add_active_users(value);
                  
              }
              //for (Json::Value::iterator t=active_users.begin(); t!=active_users.end(); ++t)
              //{
                  //Json::FastWriter fastWriter;
                  //std::string output = fastWriter.write(t);
                  //response->add_active_users(output);
              //}
                  //for (auto const& key : v.getMemberNames())
                  //{
                      //active_users.push_back(key);
                  //}
                      
              //}
              
              //return Status::OK;
          }
          else
          {
              response->set_success_status(1);
              //return StatusCode::NOT_FOUND;
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
