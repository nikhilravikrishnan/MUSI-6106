#if !defined(__SoundProcessor_hdr__)
#define __SoundProcessor_hdr__

#include <cassert>
#include "ErrorDef.h"
#include "Wavetable.h"

class CSoundProcessor
{
public:
	CSoundProcessor(float fSampleRate);
	virtual ~CSoundProcessor();

	virtual float process() = 0;

	virtual Error_t setSampleRate(float fNewSampleRate);
	float getSampleRate();

	
protected:

	float m_fSampleRateInHz = 0.0f;

};

class CInstrument : public CSoundProcessor
{
public:
	CInstrument(float fGain, float fSampleRate);
	virtual ~CInstrument();

	Error_t setGain(float fNewGain);
	float getGain() const;

protected:

	float m_fGain = 0.0f;
};

class CWavetableOscillator : public CInstrument
{
public:
	CWavetableOscillator(const CWavetable& wavetableToUse, float fFrequency, float fGain, float fSampleRate);
	virtual ~CWavetableOscillator();

	Error_t setFrequency(float fNewFrequency);
	float getFrequency() const;

	Error_t setSampleRate(float fNewSampleRate) override;
	float process() override;

protected:

	float m_fFrequencyInHz = 0.0f;
	float m_fCurrentIndex = 0.0f;
	float m_fTableDelta = 0.0f;
	const CWavetable& m_Wavetable;
	int m_iTableSize;

};

#endif // #if !defined(__SoundProcessor_hdr__)