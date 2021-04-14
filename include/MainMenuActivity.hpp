#ifndef _MAIN_MENU_ACTIVITY_HPP_
#define _MAIN_MENU_ACTIVITY_HPP_

#include <functional>
#include <list>

#include <SFML/Graphics.hpp>

#include "Activity.hpp"
#include "Button.hpp"
#include "ButtonManager.hpp"
#include "Event.hpp"
#include "EventListener.hpp"

/**
 * Subactivity of PlayingActivity. Displays and controls the main menu user interface.
 */
class MainMenuActivity : public Activity {

public:

    MainMenuActivity();

    ~MainMenuActivity();

    /**
     * Initializes with the PlayingActivity of which this activity is a part.
     */
    void init(class PlayingActivity& playingActivity);

    /**
     * Activates the activity such that buttons are able to be interacted with.
     */
    void activate() override;

    /**
     * Deactivates the activity. Buttons are not able to be interacted with, and no other methods
     * may be called before the activity is activated again.
     */
    void deactivate() override;

    void update(const float& timeDelta) override;

    void draw(sf::RenderTarget& target) override;

private:

    /**
     * Handles a ButtonClickEvent.
     */
    void buttonClickHandler(const Event& event);

    bool _initialized;
    bool _activated;

    // event listeners
    EventListener _buttonClickListener;

    // super activity
    class PlayingActivity* _playingActivity;

    // buttons
    Button _playButton;
    Button _quitButton;
    std::list<Button*> _buttons;

    // button manager
    ButtonManager _buttonManager;
};

#endif
