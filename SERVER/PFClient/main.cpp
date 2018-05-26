//
//  main.cpp
//  PFClient
//
//  Created by Marcin Małysz on 13/05/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#include <iostream>

#include "PassiveSocket.h"
#include "ActiveSocket.h"

int main(int argc, const char * argv[]) {

    CActiveSocket client;

    client.Initialize();
    client.SetNonblocking();

    char c = '\0';

    const char *msg = "abc\0";

    std::cout << "Starting client" << std::endl;

    if (client.Open("127.0.0.1", 5001))
    {
        std::cout << "client connected to server" << std::endl;

        while (true) {

            std::cin >> c;

            if (c == 'q') {

                if (client.Send((uint8 *)msg, strlen(msg)))
                {
                    std::cout << "send bye message" << std::endl;
                    break;
                }
            }
            else {

                if (client.Send((uint8 *)&c, 1))
                {
                    std::cout << "send " << c << std::endl;
                }
            }
        }
    }

    client.Close();

    return 0;
}
