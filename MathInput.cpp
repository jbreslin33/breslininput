
#include "MathInput.h"
#include "../breslinmathracer/MathRacer.h"
#include "../questionfactory/AdditionFactory.h"

#include "Ogre.h"

MathInput::MathInput(MathRacer* mathRacer)
{
    mMathRacer    = mathRacer;
}

MathInput::~MathInput()
{
}

// a number is hit what should happen? append to var in question factory and display on screen....
void MathInput::keyNumberHit(const OIS::KeyEvent &arg)
{
    std::string number   = Ogre::StringConverter::toString(arg.key -1);

    //this should update variable
    mMathRacer->getAdditionFactory()->keyNumberHit(number);

    //this should update display
    mMathRacer->keyNumberHit(number);

}

void MathInput::injectKeyDown(const OIS::KeyEvent& evt)
{
    if (evt.key == OIS::KC_M)
    {
        mMathRacer->startGame();
    }
    else if (evt.key == OIS::KC_0)
    {
        keyNumberHit(evt);
    }
    else if (evt.key == OIS::KC_1)
    {
        keyNumberHit(evt);
    }
    else if (evt.key == OIS::KC_2)
    {
        keyNumberHit(evt);
    }
    else if (evt.key == OIS::KC_3)
    {
        keyNumberHit(evt);
    }
    else if (evt.key == OIS::KC_4)
    {
        keyNumberHit(evt);
    }
    else if (evt.key == OIS::KC_5)
    {
        keyNumberHit(evt);
    }
    else if (evt.key == OIS::KC_6)
    {
        keyNumberHit(evt);
    }
    else if (evt.key == OIS::KC_7)
    {
        keyNumberHit(evt);
    }
    else if (evt.key == OIS::KC_8)
    {
        keyNumberHit(evt);
    }
    else if (evt.key == OIS::KC_9)
    {
        keyNumberHit(evt);
    }
    else if (evt.key == OIS::KC_RETURN)
    {
        mMathRacer->processAnswer();
    }
}
