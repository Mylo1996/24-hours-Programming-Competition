#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <atomic>
#include <cassert>
#include <mutex>
#include <queue>
#include <iostream>
#include <fstream>
#include <bitset>


// Useful to hold onto the memory when converting it into a Chunk.
struct Samples {
	Samples(sf::Int16 const* ss, std::size_t count) {
		samples.reserve(count);
		std::copy_n(ss, count, std::back_inserter(samples));
	}

	Samples() {}

	std::vector<sf::Int16> samples;
};


class PlaybackRecorder : private sf::SoundRecorder, private sf::SoundStream {
public: /** API **/

	// Initialise capturing input & setup output
	void start() {
		sf::SoundRecorder::start();

		sf::SoundStream::initialize(sf::SoundRecorder::getChannelCount(), sf::SoundRecorder::getSampleRate());
		sf::SoundStream::play();
	}

	// Stop both recording & playback
	void stop() {
		sf::SoundRecorder::stop();
		sf::SoundStream::stop();
	}

	bool isRunning() { return isRecording; }


	~PlaybackRecorder() {
		stop();
	}


protected: /** OVERRIDING SoundRecorder **/

	bool onProcessSamples(sf::Int16 const* samples, std::size_t sampleCount) override {
		{
			std::lock_guard<std::mutex> lock(mutex);
			data.emplace(samples, sampleCount);
		}
		cv.notify_one();
		return true; // continue capture
	}

	bool onStart() override {
		isRecording = true;
		return true;
	}

	void onStop() override {
		isRecording = false;
		cv.notify_one();
	}


protected: /** OVERRIDING SoundStream **/

	bool onGetData(Chunk& chunk) override {
		// Wait until either:
		//  a) the recording was stopped
		//  b) new data is available
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [this] { return !isRecording || !data.empty(); });

		// Lock was acquired, examine which case we're into:
		if (!isRecording) return false; // stop playing.
		else {
			assert(!data.empty());

			playingSamples.samples = std::move(data.front().samples);
			data.pop();
			chunk.sampleCount = playingSamples.samples.size();
			chunk.samples = playingSamples.samples.data();
			return true;
		}
	}

	void onSeek(sf::Time) override { /* Not supported, silently does nothing. */ }

private:
	std::atomic<bool> isRecording{ false };
	std::mutex mutex; // protects `data`
	std::condition_variable cv; // notify consumer thread of new samples
	std::queue<Samples> data; // samples come in from the recorder, and popped by the output stream
	Samples playingSamples; // used by the output stream.
};
using namespace cv;
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

int main(int argc, const char** argv)
{

	//Tesztfutás függvényekre

	//BAtoF("kki.txt", IAtoBA(BAtoIA(FtoBA("be.txt"))));
	


	Mat image;
	image = imread("down.png", CV_LOAD_IMAGE_COLOR);
	imshow("Test", image);

int main(int, char const**)
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML PlayBack");

	if (!sf::SoundRecorder::isAvailable()) {
		return EXIT_FAILURE;
	}

	PlaybackRecorder input;
	input.start();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}

			if (event.type == sf::Event::KeyPressed) {
				if (input.isRunning()) input.stop();
				else input.start();
			}
		}

		window.display();
	}

	return EXIT_SUCCESS;
}