/*
 ==============================================================================
 SpatGRIS: multichannel sound spatialization plug-in.
 
 Copyright (C) 2015  GRIS-UdeM
 
 Router.h
 
 Developers: Antoine Missout, Vincent Berthiaume
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ==============================================================================
 */

#ifndef ROUTING_H_INCLUDED
#define ROUTING_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class Router
{
public:
	static Router & instance();
	
	void accumulate(int channels, int frames, const AudioSampleBuffer &buffer);
	void reset() { mOutputBuffers.clear(); }
	void clear(int channel) { mOutputBuffers.clear(channel, 0, mOutputBuffers.getNumSamples()); }
	
	float ** outputBuffers(int frames);
    
private:
    Router();
    ~Router();
    
    AudioBuffer<float> mOutputBuffers;
    SpinLock mLock;
};

#endif  // ROUTING_H_INCLUDED
