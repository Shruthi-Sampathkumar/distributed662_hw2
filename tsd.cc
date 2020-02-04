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
        Json::value users;
        
        ip_users_file >> users;
        std::string u1 = request->user1().name();
        std::string u2 = request->user2().name();
        //user u1 = request.user1();
        //user u2 = request.user2();
        //std::vector<std::string> current_followers = users[u1]["followers"];
        //current_followers.push_back(u2);
        if (users.contains(u1) and users.contains(u2))
        {
            users["users"][u1]["following"].push_back(u2);
            users["users"][u2]["followers"].push_back(u1);
            std::ofstream op_users_file("users.json");
            op_users_file << std::setw(4) << users << std::endl;
            response.set_success_status(0);
            
        }
        else:
        {
            response.set_success_status(1);
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
        
        Json::value users;
        
        ip_users_file >> users;
        std::string u1 = request.user1().name();
        std::string u2 = request.user2().name();
        //user u1 = request.user1();
        //user u2 = request.user2();
        
        if (users.contains(u1) and users.contains(u2))
          {
              users["users"][u1]["following"].remove(u2);
              users["users"][u2]["followers"].remove(u1);
              std::ofstream op_users_file("users.json");
              op_users_file << std::setw(4) << users << std::endl;
              response.set_success_status(0);
              //return Status::OK;
          }
          else:
          {
              response.set_success_status(1);
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
        
        Json::value users;
        
        ip_users_file >> users;
       std::string u1 = request.user1().name();
        //u1 = request.user1();
        
        if (users.contains(u1))
          {
              std::vector<std::string> current_followers = users[u1]["followers"];
              //for (Json::Value::ArrayIndex i = 0; i != users.size(); i++)
              //{
              Json::Value  v = users["users"];
                  //number _of_users = v.size();
              //the keys (names) in users json file is extracted
              std::vector<std::string> active_users = v.getMemberNames();
              
              //setting the response variable
              response.set_success_status(0);
              for (vector<string>::iterator t=current_followers.begin(); t!=current_followers.end(); ++t)
              {
                  response.mutable_followers().add_followers(*t);
              }
              for (vector<string>::iterator t=active_users.begin(); t!=active_users.end(); ++t)
              {
                  response.mutable_active_users().add_active_users(*t);
              }
                  //for (auto const& key : v.getMemberNames())
                  //{
                      //active_users.push_back(key);
                  //}
                      
              //}
              
              //return Status::OK;
          }
          else:
          {
              response.set_success_status(1);
              //return StatusCode::NOT_FOUND;
          }
          
        return Status::OK;
    }
    
};




void RunServer() {
  std::string server_address("0.0.0.0:50051");
    
  social_network service;
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
