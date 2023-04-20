#include <enet/enet.h>
#include <iostream>
#include <string>

int main()
{
    ENetAddress address;
    ENetHost* server;
    ENetEvent event;
    int eventStatus;

    // Initialize ENet
    if (enet_initialize() != 0) {
        std::cerr << "Error initializing ENet" << std::endl;
        return 1;
    }

    // Set the address for the server to bind to
    address.host = ENET_HOST_ANY;
    address.port = 11234;

    // Create a server host with a maximum of 32 clients and 2 channels
    server = enet_host_create(&address, 32, 2, 0, 0);

    if (server == nullptr) {
        std::cerr << "Error creating ENet server host" << std::endl;
        enet_deinitialize();
        return 1;
    }

    std::cout << "Server started, waiting for connections..." << std::endl;
    std::cout << "address.host: " << address.host << std::endl;
    std::cout << "address.port: " << address.port << std::endl;

    while (true) {
        // Check for any incoming events on the server
        eventStatus = enet_host_service(server, &event, 1);

        if (eventStatus > 0) {
            switch (event.type) {
                // A client has connected
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "Client connected from " << event.peer->address.host << ":" << event.peer->address.port << std::endl;

                    // Send a welcome message to the client
                    {
                        std::string welcomeMessage = "Welcome to the chat room!";
                        ENetPacket* packet = enet_packet_create(welcomeMessage.c_str(), welcomeMessage.length() + 1, ENET_PACKET_FLAG_RELIABLE);
                        enet_peer_send(event.peer, 0, packet);
                    }

                    // Tell all other clients that a new client has connected
                    {
                        std::string newClientMessage = "A new client has joined the chat room!";
                        ENetPacket* packet = enet_packet_create(newClientMessage.c_str(), newClientMessage.length() + 1, ENET_PACKET_FLAG_RELIABLE);
                        enet_host_broadcast(server, 0, packet);
                    }

                    break;

                // A client has disconnected
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Client disconnected" << std::endl;

                    // Tell all other clients that this client has disconnected
                    {
                        std::string clientDisconnectedMessage = "A client has left the chat room.";
                        ENetPacket* packet = enet_packet_create(clientDisconnectedMessage.c_str(), clientDisconnectedMessage.length() + 1, ENET_PACKET_FLAG_RELIABLE);
                        enet_host_broadcast(server, 0, packet);
                    }

                    break;

                // A packet has been received from a client
                case ENET_EVENT_TYPE_RECEIVE:
                    std::cout << "Message received: " << event.packet->data << std::endl;

                    // Send the message to all other clients
                    enet_host_broadcast(server, 0, event.packet);

                    // Clean up the packet
                    enet_packet_destroy(event.packet);

                    break;
            }
        }
        else if (eventStatus < 0) {
            std::cerr << "Error servicing ENet server: " << eventStatus << std::endl;
            break;
        }
    }

    // Clean up ENet
    enet_host_destroy(server);
    enet_deinitialize();

    return 0;
}
