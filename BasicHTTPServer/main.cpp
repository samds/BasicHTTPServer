//
//  main.cpp
//  BasicHTTPServer
//
//  Created by samuel.
//  Copyright (c) 2014 SDS. All rights reserved.
//

#include <iostream>
#include <string>

#include "posix_server.h"
#include "asio_server.h"

const std::string posix_arg = "posix";
const std::string asio_arg = "asio";

int main(int argc, const char * argv[]) {

    if (argc>1) {
        if(argv[1]==posix_arg) {
            std::cout << "HTTP Client (POSIX APIs)" << std::endl;
            POSIX_init_http_server();
        }
        else if (argv[1]==asio_arg) {
            std::cout << "HTTP Client (ASIO APIs)" << std::endl;
            ASIO_init_http_server();
        }
        else {
            return EXIT_FAILURE;
        }
    }
    else {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
