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
#include "third_party/jsoncpp/json.h"
#include "jsoncpp.cpp"
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
    
    //if the command was "FOLLOW"
    Status addTo(ServerContext* context, const follow_request* request,
                    follow_response* response)
    {
        //reading the json file
        std::ifstream ip_users_file("users.json", std::ifstream::binary);
        if (!ip_users_file.is_open())
        {
          std::cout << "Failed to open users.json " << std::endl;
          return Status::OK;
        }
        
        //nlohmann::json users;
        //grpc_json users;
        Json::Reader reader;
        Json::Value users;
        reader.parse(ip_users_file, users);
        
        //ip_users_file >> users;
        std::string u1 = request->user1().name();
        std::string u2 = request->user2().name();
        //user u1 = request.user1();
        //user u2 = request.user2();
        //std::vector<std::string> current_followers = users[u1]["followers"];
        //current_followers.push_back(u2);
        if (users.isMember(u1) and users.isMember(u2))
        {
            users["users"][u1]["following"].append(u2);
            users["users"][u2]["followers"].append(u1);
            std::ofstream op_users_file("users.json");
            op_users_file << std::setw(4) << users << std::endl;
            response->set_success_status(0);
            
        }
        else
        {
            response->set_success_status(1);
            //return StatusCode::NOT_FOUND;
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
        
        //nlohmann::json users;
        
        //grpc_json users;
        Json::Reader reader;
        Json::Value users;
        reader.parse(ip_users_file, users);
        
        
        //ip_users_file >> users;
        std::string u1 = request->user1().name();
        std::string u2 = request->user2().name();
        //user u1 = request.user1();
        //user u2 = request.user2();
        
        if (users.isMember(u1) and users.isMember(u2))
          {
              //Json::Value following = users["users"][u1]["following"];
              //for( Json::ValueIterator itr = following.begin() ; itr != following.end() ; itr++ )
              //{
                  
              //}
              users["users"][u1]["following"].removeMember(u2);
              users["users"][u2]["followers"].removeMember(u1);
              std::ofstream op_users_file("users.json");
              op_users_file << std::setw(4) << users << std::endl;
              response->set_success_status(0);
              //return Status::OK;
          }
          else
          {
              response->set_success_status(1);
              //return StatusCode::NOT_FOUND;
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
        
        //nlohmann::json users;
        
        //grpc_json users;
        Json::Reader reader;
        Json::Value users;
        reader.parse(ip_users_file, users);
        
        //ip_users_file >> users;
        std::string u1 = request->user1().name();
        //u1 = request.user1();
        
        if (users.isMember(u1))
          {
              response->set_success_status(0);
              
              //users[u1]["followers"]
              Json::Value current_followers = users[u1]["followers"];
              for (const auto& element : users[u1]["followers"])
              {
                  
                  //std::string value = element.get<std::string>();
                  std::string value = element.asString();
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
