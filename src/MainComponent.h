#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "ChessboardGUI.h"
#include "DebugSonifier.h"
#include "ThreatsSonifier.h"
#include "StorySonifier.h"
#include "GameState.h"
#include "BroadcastManager.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent, public juce::ChangeListener
{
public:

    enum GameMode {
        PVP,
        PVC,
        PGN
    };

    enum SonifierMode {
        Debug,
        Threats,
        Story
    };

    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    //==============================================================================
    // Your private member variables go here...
    GameMode m_GameMode = PVP;
    SonifierMode mSonifierMode = Debug;
    SonifierBase* mCurrentSonifier = &m_DebugSonifier;
    SonifierBase* mNextSonifier = nullptr;

    BroadcastManager m_BroadcastManager;
    DebugSonifier m_DebugSonifier;
    ThreatsSonifier m_ThreatsSonifier;
    StorySonifier m_StorySonifier;
    GUI::ChessBoard m_ChessboardGUI;


    juce::TextButton buttonPreset1;
    juce::TextButton buttonPreset2;
    juce::TextButton buttonPreset3;
    juce::TextButton buttonPreset4;
    juce::TextButton buttonPreset5;
    juce::TextButton buttonReset;
    juce::TextButton buttonUndo;
    juce::TextButton buttonRedo;
    juce::ComboBox m_SonifierSelector;
    juce::ComboBox m_GameModeSelector;
    juce::Slider m_VolumeSlider;

    juce::Label m_TitleText;
    juce::Label m_TurnText;
    juce::TextButton m_pgnButton;
    juce::TextButton m_NextButton;
    juce::TextButton m_PrevButton;

    void onPgnButtonClicked();
    std::unique_ptr<juce::FileChooser> m_FileChooser;
    juce::String m_PgnString;

    void onGameModeChange(MainComponent::GameMode nextGameMode);
    void onSonifierChange(MainComponent::SonifierMode nextSonifier);
    std::stack<Chess::Move> mUndoHistory;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
