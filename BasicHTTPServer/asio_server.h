//
//  asio_server.h
//  BasicHTTPServer
//
//  Created by samuel.
//  Copyright (c) 2014 SDS. All rights reserved.
//

#ifndef BasicHTTPServer_asio_server_h
#define BasicHTTPServer_asio_server_h

#define ASIO_STANDALONE
#include "asio.hpp"

#include <ctime>
#include <iostream>
#include <string>
#include <thread>

const char header[] =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8"
"\r\n\r\n"
"<!DOCTYPE html><html><body><h1>Hello, World!</h1></body></html>"
"\r\n";


void handle_write(const asio::error_code& /*error*/,
                  size_t /*bytes_transferred*/)
{ }

bool ASIO_init_http_server()
{
    try
    {
        // Any program that uses asio need to have at least one io_service object
        asio::io_service io_service;
        
        // acceptor object needs to be created to listen for new connections
        asio::ip::tcp::acceptor acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8080));
        
        std::string message = header;
        
        asio::error_code write_error;
        
        while (true)
        {
            // creates a socket
            asio::ip::tcp::socket socket(io_service);
            
            // wait and listen
            acceptor.accept(socket);
            
            std::cout << "---------------------------------------------" << std::endl;
            std::cout << "Peer IP: " << socket.remote_endpoint().address().to_string() << std::endl;
            std::cout << "---------------------------------------------" << std::endl;
            
            // We ignore the HTTP request.
            // If we do not read the request we get the error:
            // Safari can’t open the page “‎localhost:8080” because the server
            // unexpectedly dropped the connection. This sometimes occurs when
            // the server is busy. Wait for a few minutes, and then try again
            {
                // Dummies variables used to read the HTTP request
                std::stringstream message_stream;
                asio::streambuf read_buffer;
                asio::error_code read_error;
                read_until(socket, read_buffer, "\r\n\r\n", read_error);
                if(read_error) {
                    std::cerr << read_error.message() << std::endl;
                }
            }
        
            // prepare message to send back to client
            
            asio::write(socket, asio::buffer(message), write_error);
            // Alternatives
            // asio::write(socket, asio::buffer(message), asio::transfer_all(), ignored_error);
            // asio::async_write(socket, asio::buffer(message), &handle_write);
            // socket.send(asio::buffer(message));
            // socket.async_write_some(asio::buffer(message), &handle_write);
            if(write_error) {
                std::cerr << write_error.message() << std::endl;
            }
            // Necessary?
            // socket.close();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    return true;
}

#endif
