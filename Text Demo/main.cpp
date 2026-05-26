#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return -1;

    if (TTF_Init() < 0)
        return -1;

    SDL_Window* window = SDL_CreateWindow(
        "SDL3 Text Example",
        800, 600,
        SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    // Load font (make sure font.ttf exists)
    TTF_Font* font = TTF_OpenFont("RobotoMono-Light.ttf", 48);
    if (!font)
        return -1;

    SDL_Color color = { 255, 255, 255, 255 };

    SDL_Surface* textSurface =
        TTF_RenderText_Blended(font, "Hello SDL3!", 0, color);

    SDL_Texture* textTexture =
        SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_FRect dstRect = { 200, 250,
        (float)textSurface->w,
        (float)textSurface->h };

    SDL_DestroySurface(textSurface);

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderTexture(renderer, textTexture, NULL, &dstRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}