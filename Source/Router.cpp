/*
 SpatGRIS: multichannel sound spatialization plug-in.
 
 Copyright (C) 2015  GRIS-UdeM
 
 Router.cpp
 
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

#include "PluginProcessor.h"
#include "Router.h"
#include "DefaultParam.h"

Router & Router::instance()
{
	static Router r;
	return r;
}

Router::Router()
:
mOutputBuffers(MaxSpeakers, MaxBufferSize)
{
}

Router::~Router()
{
}

void Router::accumulate(int channels, int frames, const AudioSampleBuffer &buffer)
{
	SpinLock::ScopedLockType guard(mLock);
    //copy content of each channel into the outputBuffers. Seems like it should be fast, but maybe worth checking
    for (int c = 0; c < channels; c++){
		mOutputBuffers.addFrom(c, 0, buffer, c, 0, frames);
    }
}

float ** Router::outputBuffers(int frames) {
	if (frames > MaxBufferSize) {
		printf("unexpected frames size: %d\n", frames);
		jassertfalse;
		return NULL;
	}
	return mOutputBuffers.getArrayOfWritePointers();
}

