#ifndef _BUTTON_HPP_
#define _BUTTON_HPP_
#include <SFML/Graphics.hpp>

class Button {
    public:

        Button();

        Button(int minX, int maxX, int minY, int maxY);

        Button(const Button& b);

        void onClick();

        void draw(sf::RenderTarget& target) const;

        bool isInsideBounds(int x, int y);

    private:
        int minXClickWindow, maxXClickWindow;
        int minYClickWindow, maxYClickWindow;
        int centerX, centerY;
};

#endif
