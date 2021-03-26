#ifndef _GAME_HPP_
#define _GAME_HPP_

#include <memory>

#include <SFML/Graphics.hpp>

#include "Activity.hpp"
#include "PlayingActivity.hpp"
#include "MenuActivity.hpp"

#include "Globals.hpp"
#include "EventListener.hpp"
#include "Event.hpp"

/**
 * The Game class serves as the application layer.
 */
class Game {

public:

    /**
     * Default constructor.
     */
    Game();

    /**
     * Destructor. Removes event listeners from the event messenger.
     */
    ~Game();

    /**
     * Initializes the window and the first activity. Also initializes any game-related global
     * structures.
     */
    void init();

    /**
     * Handle the polling of SFML events, then pass the update task on to the current activity.
     * @return true if the game window is still open, false otherwise
     */
    bool update();

    /**
     * Clear, draw, and display the screen.
     */
    void draw() const;

private:

    /**
     * Handles resizing of the window by changing the window's viewport. The viewport will be as
     * large as possible such that the native aspect ratio is maintained. If the window is taller
     * than it should be, then black bars will appear on the top and bottom; if the window is wider,
     * then the bars will be on left and right.
     * @param event should be a WindowResizeEvent
     */
    void windowResizeHandler(const Event& event);

    /**
     * Closes the window when the user exits out of the window.
     * @param event should be a WindowCloseEvent
     */
    void windowCloseHandler(const Event& event);

    void buttonHandler(const Event& event);
    
    bool _initialized;

    // event listeners
    EventListener _windowResizeListener;
    EventListener _windowCloseListener;
    EventListener _buttonPressListener;
    // the render window onto which to draw
    std::shared_ptr<sf::RenderWindow> _window;

    // activities, for now there's only a PlayingActivity
    PlayingActivity _playingActivity;
    MenuActivity _menuActivity;
    Activity* _currentActivity;

    // game clock
    sf::Clock _clock;

    // time difference between the current fame and the previous frame, recalculated every frame
    float _timeDelta;
};

#endif // _GAME_HPP_
