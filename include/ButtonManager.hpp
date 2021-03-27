#ifndef _BUTTON_MANAGER_HPP_
#define _BUTTON_MANAGER_HPP_

#include <map>
#include <unordered_map>
#include <list>
#include <functional>

#include <SFML/Graphics.hpp>

#include "Button.hpp"
#include "Globals.hpp"
#include "EventListener.hpp"
#include "Event.hpp"

typedef std::function<void (const Button*)> ButtonClickCallback;

enum class CONTROL_KEY_TYPE {UP, RIGHT, DOWN, LEFT, SELECT};

/**
 * Structure which stores which buttons are adjacent to a button; helps with keyboard control of
 * buttons. E.g. if the down arrow key is presesed, then the button manager should follow the
 * "below" link.
 */
typedef struct ButtonAdjacencies {

public:

    ButtonAdjacencies(
        Button* above = nullptr,
        Button* right = nullptr,
        Button* below = nullptr,
        Button* left = nullptr
    ) :
        _buttons({
            {CONTROL_KEY_TYPE::UP,    above},
            {CONTROL_KEY_TYPE::RIGHT, right},
            {CONTROL_KEY_TYPE::DOWN,  below},
            {CONTROL_KEY_TYPE::LEFT,  left}
        })
    {}

    Button* getAdjacent(const CONTROL_KEY_TYPE& type) {
        if (_buttons.find(type) != _buttons.end())
            return _buttons[type];
        return nullptr;
    }

private:

    std::unordered_map<CONTROL_KEY_TYPE, Button*> _buttons;

} ButtonAdjacencies;

/**
 * Manages any number of buttons by determining which one is hovered over, clicked on, etc. The
 * mouse can always interact with the buttons, and optionally the keyboard can as well. When a
 * button is "clicked", a callback function is called instead of triggering an event.
 * 
 * When setting up the ButtonManager, all the buttons must be added before activation.
 */
class ButtonManager {

public:

    ButtonManager();

    ~ButtonManager();

    /**
     * Initializes the ButtonManager.
     * 
     * @param buttonClickedCallback Function to call when a button is clicked
     * @param canUseKeyboard  Whether or not keyboard keys can be used to navigate and select
     *     buttons. If this is true, then there will always be one button that is hovered over. If
     *     this is false, then a button will only be hovered over if the mouse cursor is on top of
     *     it. Defaults to false.
     */
    void init(const ButtonClickCallback& buttonClickCallback, const bool& canUseKeyboard = false);

    /**
     * Activates the button manager such that the buttons can now be interacted with.
     */
    void activate();

    /**
     * Deactivates the button manager such that its buttons can no longer be interacted with.
     */
    void deactivate();

    /**
     * Adds the given button and adjacencies to the button manager. By default, the adjacencies are
     * set up such that the given button has no adjacencies.
     */
    void addButton(Button* button, ButtonAdjacencies adjacencies = ButtonAdjacencies());

private:

    /**
     * Returns the button which contains the given point. If there is no such button, then returns
     * nullptr.
     */
    Button* getButtonContaining(const sf::Vector2f& point);

    // event listener callbacks
    void mouseMoveHandler(const Event& event);
    void mousePressHandler(const Event& event);
    void mouseReleaseHandler(const Event& event);
    void keyPressHandler(const Event& event);
    void keyReleaseHandler(const Event& event);

    bool _initialized;
    bool _activated;

    // whether or not keyboard keys can be used to navigate the buttons
    bool _canUseKeyboard;

    // event listeners
    EventListener _mouseMoveListener;
    EventListener _mousePressListener;
    EventListener _mouseReleaseListener;
    EventListener _keyPressListener;
    EventListener _keyReleaseListener;

    // called when a button is clicked, with the mouse or keyboard
    ButtonClickCallback _buttonClickCallback;

    // maps a button to its adjacent buttons, ordered so that we know the first button added
    std::map<Button*, ButtonAdjacencies> _buttons;

    Button* _buttonHovered; // which button is being hovered over
    bool _buttonIsClicked; // if the hovered button is being clicked or not

    // keys that control navigation, these are WASD, arrow keys, space, and enter
    const std::unordered_map<sf::Keyboard::Key, CONTROL_KEY_TYPE> _CONTROL_KEYS;
};

#endif
