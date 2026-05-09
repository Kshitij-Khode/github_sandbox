#include <iostream>

// Target Interface - New interface that client code expects
class MediaPlayer {
public:
    virtual ~MediaPlayer() {}
    virtual void play(const char* filename) = 0;
};

// Adaptee - Old interface with incompatible implementation
class AudioPlayer {
public:
    void playAudio(const char* filename) {
        std::cout << "Playing audio: " << filename << std::endl;
    }
};

class VideoPlayer {
public:
    void playVideo(const char* filename) {
        std::cout << "Playing video: " << filename << std::endl;
    }
};

// Adapter - Adapts the old interfaces to the new MediaPlayer interface
class AudioAdapter : public MediaPlayer {
private:
    AudioPlayer* audioPlayer;

public:
    AudioAdapter() : audioPlayer(new AudioPlayer()) {}
    
    ~AudioAdapter() {
        delete audioPlayer;
    }

    void play(const char* filename) override {
        audioPlayer->playAudio(filename);
    }
};

class VideoAdapter : public MediaPlayer {
private:
    VideoPlayer* videoPlayer;

public:
    VideoAdapter() : videoPlayer(new VideoPlayer()) {}
    
    ~VideoAdapter() {
        delete videoPlayer;
    }

    void play(const char* filename) override {
        videoPlayer->playVideo(filename);
    }
};

int main() {
    MediaPlayer* audioMedia = new AudioAdapter();
    audioMedia->play("song.mp3");

    MediaPlayer* videoMedia = new VideoAdapter();
    videoMedia->play("movie.mp4");

    delete audioMedia;
    delete videoMedia;

    return 0;
}
