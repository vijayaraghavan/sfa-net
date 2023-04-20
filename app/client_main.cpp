#include <iostream>
#include <string>
#include <enet/enet.h>

#define PORT 11234
#define MAX_CLIENTS 32
#define CHANNEL_ID 0

int main()
{
    if (enet_initialize() != 0) {
        std::cerr << "Could not initialize ENet.\n";
        return 1;
    }

    ENetAddress address;
    ENetHost* client;
    ENetEvent event;

    enet_address_set_host(&address, "localhost");
    address.port = PORT;

    client = enet_host_create(NULL, 1, CHANNEL_ID, 0, 0);
    if (client == NULL) {
        std::cerr << "Could not create client.\n";
        return 1;
    }

    ENetPeer* peer = enet_host_connect(client, &address, CHANNEL_ID, 0);
    if (peer == NULL) {
        std::cerr << "Could not connect to server.\n";
        return 1;
    }

    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);

    bool connected = false;
    std::string message;

    while (!connected) {
        while (enet_host_service(client, &event, 1000) > 0) {
            if (event.type == ENET_EVENT_TYPE_CONNECT) {
                std::cout << "Connected to server.\n";
                connected = true;
                break;
            }
            if (event.type == ENET_EVENT_TYPE_RECEIVE) {
                std::cout << "Received message: " << event.packet->data << "\n";
                enet_packet_destroy(event.packet);
            }
        }

        if (!connected) {
            std::cout << "Connecting to server...\n";
        }
    }

    while (true) {
        std::getline(std::cin, message);
        std::cout << "whiletrue" << std::endl;
        if (message.empty()) {
            continue;
        }

        if (message == "/quit") {
            break;
        }

        std::string full_message = name + ": " + message;
        ENetPacket* packet = enet_packet_create(full_message.c_str(), full_message.length() + 1, ENET_PACKET_FLAG_RELIABLE);
        std::cout << "message sent" << std::endl;
        enet_peer_send(peer, CHANNEL_ID, packet);
    }

    enet_peer_disconnect(peer, 0);
    while (enet_host_service(client, &event, 1000) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            enet_packet_destroy(event.packet);
        }
        else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            std::cout << "Disconnected from server.\n";
            break;
        }
    }

    enet_host_destroy(client);
    enet_deinitialize();

    return 0;
}
