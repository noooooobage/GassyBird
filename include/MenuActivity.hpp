#ifndef _MENU_ACTIVITY_HPP_
#define _MENU_ACTIVITY_HPP_

#include <SFML/Graphics.hpp>

#include "Activity.hpp"
#include "GameLogic.hpp"
#include "HumanView.hpp"
#include "Button.hpp"

class MenuActivity : public Activity {

    public:

        MenuActivity();

        void init();

        void update(const float& timeDelta) override;


        Button getButton() { return _goToPlaying; }

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:

        bool _initialized;

        GameLogic _logic;

        HumanView _humanView;

        Button _goToPlaying;

};

#endif