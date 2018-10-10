#include <SFML/Audio.hpp>

#include <iostream>
#include <fstream>
#include <bitset>
#include <Windows.h>
#include <cmath>

using namespace std;

vector<char> FtoBA(string filename) {
	ifstream inputf;
	inputf.open(filename, ios::binary);
	vector<char> buffer((istreambuf_iterator<char>(inputf)), (istreambuf_iterator<char>()));
	inputf.close();
	return buffer;
}

void BAtoF(string filename, vector<char> buffer) {
	ofstream outputf;
	outputf.open(filename, ios::binary);
	for (std::vector<char>::iterator it = buffer.begin(); it != buffer.end(); ++it) outputf << static_cast<unsigned char>(bitset<8>(*it).to_ulong());
	outputf.close();
}

vector<char> IAtoBA(vector<int> vi) {
	vector<char> vc;
	for (std::vector<int>::iterator it = vi.begin(); it != vi.end(); ++it) {
		string s= to_string(*it - 48);
		++it;
		s += to_string(*it - 48);
		++it;
		s += to_string(*it - 48);
		++it;
		s += to_string(*it - 48);
		++it;
		s += to_string(*it - 48);
		++it;
		s += to_string(*it - 48);
		++it;
		s += to_string(*it - 48);
		++it;
		s += to_string(*it - 48);
		vc.push_back(static_cast<unsigned char>(bitset<8>(s).to_ulong()));
	}
	return vc;
}

vector<int> BAtoIA(vector<char> vc) {
	vector<int> vi;
	for (std::vector<char>::iterator it = vc.begin(); it != vc.end(); ++it) {
		string s = (bitset<8>(*it)).to_string();
		for (int i = 0; i < 8; i++) vi.push_back(s[i]);
	}
	return vi;
}


int main(int, char const**)
{
	if (!sf::SoundBufferRecorder::isAvailable())
	{
	}
	// create the recorder
	sf::SoundBufferRecorder recorder;
	// start the capture
	recorder.start();
	Sleep(500);
	cout << "Speak now" << endl;
	Sleep(2000);
	// stop the capture
	recorder.stop();
	cout << "Stop" << endl;
	// retrieve the buffer that contains the captured audio data
	const sf::SoundBuffer& buffer = recorder.getBuffer();
	sf::Sound sound(buffer);
	Sleep(100);
	sound.play();
	Sleep(3000);

	const unsigned SAMPLES = 44100;
	const unsigned SAMPLE_RATE = 44100;
	const unsigned AMPLITUDE = 30000;

	sf::Int16 raw[SAMPLES];

	const double TWO_PI = 6.28318;
	const double increment = 440. / 44100;
	double x = 0;
	for (int i = 0; i < SAMPLES; i++) {
		raw[i] = AMPLITUDE * sin(x*TWO_PI*12000);
		x += increment;
	}

	sf::SoundBuffer Buffer;
	if (!Buffer.loadFromSamples(raw, SAMPLES, 1, SAMPLE_RATE)) {
		std::cerr << "Loading failed!" << std::endl;
		return 1;
	}

	sf::Sound Sound;
	Sound.setBuffer(Buffer);
	//Sound.setLoop(true);
	Sound.play();

	while (Sound.getStatus() == 2){}

	return EXIT_SUCCESS;
}