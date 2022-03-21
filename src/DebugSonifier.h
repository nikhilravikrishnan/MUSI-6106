#ifndef MUSI_6106_DEBUGSONIFIER_H
#define MUSI_6106_DEBUGSONIFIER_H

#include <cassert>
#include "ErrorDef.h"
#include "GameState.h"
#include "Wavetable.h"
#include "MainProcessor.h"
#include <vector>
#include <list>


class DebugSonifier : public juce::ChangeListener {
public:
        DebugSonifier();

        virtual ~DebugSonifier();

        void process(float **ppfOutBuffer, int iNumChannels, int iNumFrames);

        void prepareToPlay(int iExpectedBlockSize, double fSampleRate);

        void releaseResources();

        Error_t onMove(Chess::Board & board);

		void changeListenerCallback(juce::ChangeBroadcaster* source) override
		{
             onMove(AppState::getInstance().getGame());
		}

        void enable() { m_mainProcessor.noteOn(); onMove(AppState::getInstance().getGame()); };
        void disable() { m_mainProcessor.noteOff(); };

        void setGain(float fNewGain) { m_mainProcessor.setGain(fNewGain); };

protected:

    void sonifyPiece(Chess::Square const& square, Chess::Piece const& piece);
    
    std::list<std::shared_ptr<CInstrument>> oscillatorPtrs;

    CSineWavetable sine;

    CMainProcessor m_mainProcessor;

    float m_fSampleRate = 0;

    int m_fExpectedBlockSize = 0;


};


#endif //MUSI_6106_DEBUGSONIFIER_H
