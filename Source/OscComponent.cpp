/*
 ==============================================================================
 SpatGRIS: multichannel sound spatialization plug-in.
 
 Copyright (C) 2015  GRIS-UdeM
 
 OscComponent.cpp
 Created: 8 Aug 2014 9:27:08am
 
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

#include "OscComponent.h"
#if USE_TOUCH_OSC
const String kOscPathSourceXY = "/Octo/SourceXY";
const String kOscPathSelectSource = "/Octo/Source";

static String getLocalIPAddress(){
//    Array<IPAddress> addresses;
//    IPAddress::findAllAddresses (addresses);
//    
//    String addressList;
//    
//    for (int i = 0; i < addresses.size(); ++i)
//        addressList << "   " << addresses[i].toString() << newLine;
//    
//    return addressList;
    
    Array<IPAddress> addresses;
    IPAddress::findAllAddresses (addresses);
    return addresses[1].toString();
}


class OscComponent : public HeartbeatComponent, public Button::Listener, public TextEditor::Listener, private OSCReceiver,
	private OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>
{
public:
    OscComponent(SpatGrisAudioProcessor* filter, SpatGrisAudioProcessorEditor *editor)
	:mFilter(filter)
    ,mEditor(editor)
    ,mOscSendIpAddress(NULL)
    ,mNeedToEnd(false)
	{
        
		const int m = 10, dh = 18, cw = 120, iw = 112, pw = 60;
		int x = m, y = m;

		mReceive = new ToggleButton();
		mReceive->setButtonText("Receive on ip : port");
		mReceive->setSize(cw, dh);
		mReceive->setTopLeftPosition(x, y);
		mReceive->addListener(this);
		mReceive->setToggleState(mFilter->getOscReceiveEnabled(), dontSendNotification);
        mReceive->setColour(ToggleButton::textColourId, mGrisFeel.getFontColour());
		addAndMakeVisible(mReceive);
		
        x += cw + m;
        
        mReceiveIp = new TextEditor();
//        mReceiveIp->setColour(TextEditor::textColourId, juce::Colour::greyLevel(.6));
        mReceiveIp->setFont(mGrisFeel.getFont());
        mReceiveIp->setLookAndFeel(&mGrisFeel);
        mReceiveIp->setSize(iw, dh);
        mReceiveIp->setTopLeftPosition(x, y);
        //if local address does not start with 10., put it in the ip receive box. Using 10.x.x.x addresses is problematic at UdeM
        String ipAddress = getLocalIPAddress();
        if (!ipAddress.startsWith("10.")){
            mReceiveIp->setText(ipAddress);
            mReceiveIp->setReadOnly(true);
            mReceiveIp->setCaretVisible(false);
        }
        addAndMakeVisible(mReceiveIp);
    
        x += iw + m;
		
		mReceivePort = new TextEditor();
        mReceivePort->setFont(mGrisFeel.getFont());
        mReceivePort->setLookAndFeel(&mGrisFeel);
		mReceivePort->setText(String(mFilter->getOscReceivePort()));
		mReceivePort->setSize(pw, dh);
		mReceivePort->setTopLeftPosition(x, y);
		mReceivePort->addListener(this);
		addAndMakeVisible(mReceivePort);
		
		x = m; y += dh + m;
		
		mSend = new ToggleButton();
		mSend->setButtonText("Send on ip : port");
		mSend->setSize(cw, dh);
		mSend->setTopLeftPosition(x, y);
		mSend->addListener(this);
		mSend->setToggleState(mFilter->getOscSendEnabled(), dontSendNotification);
        mSend->setColour(ToggleButton::textColourId, mGrisFeel.getFontColour());
		addAndMakeVisible(mSend);
		
		x += cw + m;
		
		mSendIp = new TextEditor();
        mSendIp->setFont(mGrisFeel.getFont());
        mSendIp->setLookAndFeel(&mGrisFeel);
		mSendIp->setText(mFilter->getOscSendIp());
		mSendIp->setSize(iw, dh);
        mSendIp->addListener(this);
		mSendIp->setTopLeftPosition(x, y);
		addAndMakeVisible(mSendIp);
		
		x += iw + m;
		
		mSendPort = new TextEditor();
        mSendPort->setFont(mGrisFeel.getFont());
        mSendPort->setLookAndFeel(&mGrisFeel);
		mSendPort->setText(String(mFilter->getOscSendPort()));
		mSendPort->setSize(pw, dh);
		mSendPort->setTopLeftPosition(x, y);
		mSendPort->addListener(this);
		addAndMakeVisible(mSendPort);
		
		if (mReceive->getToggleState()) buttonClicked(mReceive);
		if (mSend->getToggleState()) buttonClicked(mSend);
	}
    
    void updateInfo(){
        mReceive->setToggleState(mFilter->getOscReceiveEnabled(), dontSendNotification);
        mReceiveIp->setText(getLocalIPAddress());
        mReceivePort->setText(String(mFilter->getOscReceivePort()));
        mSend->setToggleState(mFilter->getOscSendEnabled(), dontSendNotification);
        mSendIp->setText(mFilter->getOscSendIp());
        mSendPort->setText(String(mFilter->getOscSendPort()));
        
        if (mReceive->getToggleState()) buttonClicked(mReceive);
        if (mSend->getToggleState()) buttonClicked(mSend);
    };
	
	~OscComponent() {
        disconnect();
        mEditor->getMover()->end(kOsc);
    }
	
    void textEditorFocusLost (TextEditor &textEditor)override
    {
        if (&textEditor == mReceivePort) {
            mFilter->setOscReceivePort(mReceivePort->getText().getIntValue());
        } else if (&textEditor == mSendPort) {
            mFilter->setOscSendPort(mSendPort->getText().getIntValue());
        } else if (&textEditor == mSendIp){
            mFilter->setOscSendIp(mSendIp->getText());
        }
    }
    void textEditorTextChanged (TextEditor &textEditor) override{
        if (&textEditor == mReceivePort) {
            mFilter->setOscReceivePort(mReceivePort->getText().getIntValue());
        } else if (&textEditor == mSendPort) {
            mFilter->setOscSendPort(mSendPort->getText().getIntValue());
        } else if (&textEditor == mSendIp){
            mFilter->setOscSendIp(mSendIp->getText());
        }
        textEditor.grabKeyboardFocus();
        //grabKeyboardFocus
    }
    void returnPressed(TextEditor &textEditor) {
        if (&textEditor == mReceivePort) {
            mFilter->setOscReceivePort(mReceivePort->getText().getIntValue());
        } else if (&textEditor == mSendPort) {
            mFilter->setOscSendPort(mSendPort->getText().getIntValue());
        } else if (&textEditor == mSendIp){
            mFilter->setOscSendIp(mSendIp->getText());
        }
        textEditor.grabKeyboardFocus();

    }
    
	
	
    void buttonClicked (Button *button) override{
        try {
            if (button == mReceive){
                //try to connect to receiving port on one of the local ip addresses
                if (mReceive->getToggleState()) {
                    int p = mReceivePort->getText().getIntValue();
                    if (!connect(p)) {
                        DBG("Error: could not connect to UDP port.");
                    } else {
                        addListener(this, "/Octo/SourceXY");
                        addListener(this, "/Octo/Source1");
                        addListener(this, "/Octo/Source2");
                        addListener(this, "/Octo/Source3");
                        addListener(this, "/Octo/Source4");
                        addListener(this, "/Octo/Source5");
                        addListener(this, "/Octo/Source6");
                        addListener(this, "/Octo/Source7");
                        addListener(this, "/Octo/Source8");
                        changeStatusComp(mReceiveIp, false);
                        changeStatusComp(mReceivePort, false);
                        
                    }
                //trying to disconnect
                } else {
                    if (disconnect()) {
                        changeStatusComp(mReceiveIp);
                        changeStatusComp(mReceivePort);
                    } else {
                        DBG("lo_server_thread_new failed (port in use ?)");
                    }
                }
                mFilter->setOscReceiveEnabled(mReceive->getToggleState());
            } else if (button == mSend) {

                if (mSend->getToggleState()) {
                    mOscSendIpAddress = mSendIp->getText();
                    int iSendPort = mSendPort->getText().getIntValue();
                    if(!mOscSender.connect(mOscSendIpAddress, iSendPort)){
                        DBG("OSC cannot connect");
                        mSend->setToggleState(false, dontSendNotification);
                        changeStatusComp(mSendIp);
                        changeStatusComp(mSendPort);

                    } else {
                        mSourceXY = FPoint(-1, -1);
                        mSource = -1;
                        changeStatusComp(mSendIp, false);
                        changeStatusComp(mSendPort, false);
                    }

                } else {
                    changeStatusComp(mSendIp);
                    changeStatusComp(mSendPort);
                }
                mFilter->setOscSendEnabled(mSend->getToggleState());
            } else {
                printf("unknown button clicked...\n");
            }
        } catch (exception& e) {
            DBG(e.what());
        }
    }
	void oscMessageReceived(const OSCMessage& message) override {
		string address = message.getAddressPattern().toString().toStdString();
		//set position for current source
		if (address == kOscPathSourceXY && message.size() == 2 && message[0].isFloat32() && message[1].isFloat32()){
			float y = message[0].getFloat32();
			float x = message[1].getFloat32();

			mEditor->getMover()->begin(mEditor->getOscLeapSource(), kOsc);
			mEditor->getMover()->move(FPoint(x, y), kOsc);
			mEditor->fieldChanged();

			mNeedToEnd = true;
			mLastXYTime = Time::getCurrentTime(); 
		//set current source
		} else if (address.substr(0, address.size()-1) == kOscPathSelectSource && address.size() == kOscPathSelectSource.length() + 1
			&& message.size() == 1 && message[0].isFloat32() && message[0].getFloat32() < .5) {
			string src_str = address.substr(address.size() - 1);
			String src_jstr(src_str);
			int iSrc = src_jstr.getIntValue()-1;
			mEditor->setOscLeapSource(iSrc);
		}
	}
	
	void heartbeat() override{
		if (mNeedToEnd) {
			Time now = Time::getCurrentTime();
			RelativeTime dt = (now - mLastXYTime);
			if (dt.inMilliseconds() > 200) {
				mEditor->getMover()->end(kOsc);
				mNeedToEnd = false;
			}
		}
        if (!mSend->getToggleState()){
            return;
        }
		int src = mEditor->getOscLeapSource();
		if (src != mSource) {
			String s = "Source "; 
			s << (src+1);
			if (!mOscSender.send(String(kOscPathSelectSource), s)) {
				DBG("Error: could not send OSC message.");
			}
			mSource = src;
		}
		FPoint p = mFilter->getSourceXY01(src);
		if (mSourceXY != p) {
            OSCAddressPattern oscPattern(kOscPathSourceXY);
			OSCMessage message(oscPattern);
			message.addFloat32(p.y);
			message.addFloat32(p.x);
			if (!mOscSender.send(message)) {
				DBG("Error: could not send OSC message.");
			}
			mSourceXY = p;
		}
	}

private:
    
    void changeStatusComp(TextEditor *comp, bool enable = true){
        if(enable){
            comp->setColour (TextEditor::textColourId, juce::Colours::black);
            comp->applyFontToAllText (mGrisFeel.getFont());
            comp->setReadOnly(false);
            comp->setEnabled(true);

        }else{
            comp->setColour (TextEditor::textColourId, juce::Colour::greyLevel(.35));
            comp->applyFontToAllText (mGrisFeel.getFont());
            comp->setReadOnly(true);
            comp->setEnabled(false);
        }
    }
	SpatGrisAudioProcessor *mFilter;
	SpatGrisAudioProcessorEditor *mEditor;
	
	ScopedPointer<ToggleButton> mReceive;
    ScopedPointer<TextEditor>   mReceiveIp;
	ScopedPointer<TextEditor>   mReceivePort;
	
	ScopedPointer<ToggleButton> mSend;
	ScopedPointer<TextEditor> mSendIp;
	ScopedPointer<TextEditor> mSendPort;
	OSCSender mOscSender;
	String mOscSendIpAddress;
	
	bool mNeedToEnd;
	Time mLastXYTime;
	
	FPoint mSourceXY;
	int mSource;
    
    GrisLookAndFeel mGrisFeel;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscComponent)
};
HeartbeatComponent * CreateOscComponent(SpatGrisAudioProcessor *filter, SpatGrisAudioProcessorEditor *editor){
	return new OscComponent(filter, editor);
}
void updateOscComponent(HeartbeatComponent* oscComponent){
    dynamic_cast<OscComponent*>(oscComponent)->updateInfo();
}
#endif
