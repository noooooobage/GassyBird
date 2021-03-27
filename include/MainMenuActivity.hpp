#ifndef _MAIN_MENU_ACTIVITY_HPP_
#define _MAIN_MENU_ACTIVITY_HPP_

#include <functional>
#include <list>

#include <SFML/Graphics.hpp>

#include "Activity.hpp"
#include "Button.hpp"
#include "ButtonManager.hpp"

/**
 * Subactivity of PlayingActivity. Displays and controls the main menu user interface.
 */
class MainMenuActivity : public Activity {

public:

    MainMenuActivity();

    void init();

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
     * To be called by the button manager when one of the buttons is clicked.
     */
    void buttonClickCallback(const Button* button);

    bool _initialized;
    bool _activated;

    // buttons
    Button _playButton;
    Button _exitButton;
    std::list<Button*> _buttons;

    // button manager stuff
    ButtonClickCallback _buttonClickCallback;
    ButtonManager _buttonManager;
};

#endif
