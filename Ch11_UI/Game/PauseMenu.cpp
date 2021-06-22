#include "PauseMenu.h"
#include "Game.h"
#include <SDL/SDL.h>

PauseMenu::PauseMenu(Game* game)
    :UIScreen(game)
{
    mGame->SetState(Game::EPaused);
    SetRelativeMouseMode(false);
    SetTitle("PAUSED");
    AddButton("ResumeButton", [this]() {
        this->Close();
    });
    AddButton("Quit", [this]() {
        mGame->SetState(Game::EQuit);
    });
}
PauseMenu::~PauseMenu()
{
    SetRelativeMouseMode(true);
    mGame->SetState(Game::EGameplay);
}
void PauseMenu::HandleKeyPress(int key)
{
    UIScreen::HandleKeyPress(key);

    if (key == SDLK_ESCAPE)
    {
        Close();
    }
}