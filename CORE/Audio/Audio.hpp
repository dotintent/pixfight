//
//  Audio.hpp
//  PixFight
//
//  Created by Marcin Małysz on 05/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "fmod.hpp"

class Audio {

public:

    typedef uint32_t SoundID;

    Audio();
    ~Audio() noexcept;

    SoundID loadSound(const std::string & path) const;

    void playSound(const SoundID & sid) const;
    void stopSound(const SoundID & sid) const;

    void loadMusic(const std::string & path) const;
    void playMusic() const;
    void pauseMusic() const;

    void mute() const;
    void unmute() const;

    bool isMuted() { return _muted; }
    void setVolume(float volume);
    float getVolume();

private:

    void unload();
    void checkResult(FMOD_RESULT result) const;

    mutable bool             _muted;
    mutable FMOD::System     *_system;
    mutable FMOD::Channel    *_channel;
    mutable FMOD_RESULT       _result;
    unsigned int      _version;
    mutable int       _numsubsounds;

    mutable FMOD::Channel    *_schannel;
    mutable FMOD::Sound      *_stream, *_stream_to_play;

    FMOD::ChannelGroup *_masterGroup;

    mutable std::vector<FMOD::Sound *> _sounds;
    mutable std::vector<std::string> _paths;
    float _volume;
};

