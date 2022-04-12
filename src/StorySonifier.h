#pragma once

#include "Wavetable.h"
#include "MainProcessor.h"
#include "GameState.h"

class StorySonifier : public juce::ChangeListener, public juce::ActionListener
{
public:
	StorySonifier();
	~StorySonifier();

	void process(float** ppfOutputBuffer, int iNumChannels, int iNumFrames);
	void prepareToPlay(int iExpectedBlockSize, int iSamplesRate);
	void releaseResources();

	Error_t onMove(Chess::Board& board);

	void setEnabled(bool shouldEnable);
	void setGain(float fGain);

private:

	void sonifyPiece(Chess::Square const& square, Chess::Piece const& piece);

	void changeListenerCallback(juce::ChangeBroadcaster* source) override;
	void actionListenerCallback(const juce::String& message) override;

	CMainProcessor mMainProcessor;
	CSineWavetable mSine;
	CSawWavetable mSaw;
	CSqrWavetable mSquare;

	float mSampleRate;
	float mBlockSize;

};