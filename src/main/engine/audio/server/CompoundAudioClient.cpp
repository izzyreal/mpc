#include <engine/audio/server/CompoundAudioClient.hpp>

using namespace ctoot::audio::server;
using namespace std;

void CompoundAudioClient::work(int nFrames)
{
	for (auto& client : clients) {
		client->work(nFrames);
	}
}

void CompoundAudioClient::add(AudioClient* client)
{
    if(client == nullptr) return;
    clients.push_back(client);
}

void CompoundAudioClient::remove(AudioClient* client)
{
	if (client == nullptr) return;
	for (int i = 0; i < clients.size(); i++) {
		if (clients[i] == client) {
			clients.erase(clients.begin() + i);
			break;
		}
	}
}
