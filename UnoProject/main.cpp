/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard math, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 600;

//Texture wrapper class
class LTexture
{
public:
    //Initializes variables
    LTexture();

    //Deallocates memory
    ~LTexture();

    //Loads image at specified path
    bool loadFromFile(std::string path);

    //Deallocates texture
    void free();

    //Renders texture at given point
    void render(int x, int y, SDL_Rect* clip = nullptr);
    void renderRotated(int x, int y, double angle, SDL_Rect* clip = nullptr);

    //Gets image dimensions
    int getWidth();
    int getHeight();

private:
    //The actual hardware texture
    SDL_Texture* mTexture;

    //Image dimensions
    int mWidth;
    int mHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = nullptr;

//The window renderer
SDL_Renderer* gRenderer = nullptr;

//Scene sprites
SDL_Rect gSpriteClips[56];
LTexture gSpriteSheetTexture;


LTexture::LTexture()
{
    //Initialize
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    //Deallocate
    free();
}

bool LTexture::loadFromFile(std::string path)
{
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture* newTexture = nullptr;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == nullptr)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    mTexture = newTexture;
    return mTexture != nullptr;
}

void LTexture::free()
{
    //Free texture if it exists
    if (mTexture != nullptr)
    {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y, SDL_Rect* clip)
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    //Set clip rendering dimensions
    if (clip != nullptr)
    {
        renderQuad.w = clip->w*0.6;
        renderQuad.h = clip->h*0.6;
    }
    
    //Render to screen
    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

void LTexture::renderRotated(int x, int y, double angle, SDL_Rect* clip) {
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };

    //Set clip rendering dimensions
    if (clip != nullptr)
    {
        renderQuad.w = clip->w*0.6;
        renderQuad.h = clip->h*0.6;
    }

    //Define the center of the clip
    SDL_Point center;

    center.x = renderQuad.w / 2;
    center.y = renderQuad.h / 2;

    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, &center, SDL_FLIP_NONE);

}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        //Create window
        gWindow = SDL_CreateWindow("Uno!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == nullptr)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == nullptr)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;
    unsigned int x[14] = {48, 188, 328, 468, 608, 748, 888, 1028, 1168, 1308, 1448, 1588, 1728, 1868};
    unsigned int y[4] = {30, 226, 422, 618};
    const int cardWidth = 133, cardHeight = 185;
    int i = 0;

    //Load sprite sheet texture
    if (!gSpriteSheetTexture.loadFromFile("uno.png"))
    {
        printf("Failed to load sprite sheet texture!\n");
        success = false;
    }
    else
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 14; k++)
            {
                std::cout << x[k] << " " << y[j] << " " << i << std::endl;
                gSpriteClips[i].x = x[k];
                gSpriteClips[i].y = y[j];
                gSpriteClips[i].h = cardHeight;
                gSpriteClips[i].w = cardWidth;
                i++;

            }
        }
    }

    return success;
}

void close()
{
    //Free loaded images
    gSpriteSheetTexture.free();

    //Destroy window	
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* args[])
{
    //Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        //Load media
        if (!loadMedia())
        {
            printf("Failed to load media!\n");
        }
        else
        {
            //Main loop flag
            bool quit = false;

            //Event handler
            SDL_Event e;

            //While application is running
            while (!quit)
            {
                //Handle events on queue
                while (SDL_PollEvent(&e) != 0)
                {
                    //User requests quit
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }

                srand(time(NULL));

                //Clear screen
                SDL_SetRenderDrawColor(gRenderer, 6, 136, 148, 255);
                SDL_RenderClear(gRenderer);

                for (int i = 0; i < 50; i++)
                {
                    gSpriteSheetTexture.render((70 - i)*0.6, 200, &gSpriteClips[55]);
                }

                for (int j = 0; j < 5; j++)
                {
                    gSpriteSheetTexture.render(100 * (j + 2), 0, &gSpriteClips[55]);
                }

                for (int k = 0; k < 5; k++)
                {
                    gSpriteSheetTexture.renderRotated((SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) - 50, 360 / (k*k+1), &gSpriteClips[k * k-1]);
                }

                //Render top left sprite
                gSpriteSheetTexture.render((133*2)*0.6, SCREEN_HEIGHT - gSpriteClips[2].h, &gSpriteClips[0]);

                //Render top right sprite
                gSpriteSheetTexture.render((133*3)*0.6, SCREEN_HEIGHT - gSpriteClips[2].h, &gSpriteClips[13]);

                //Render bottom left sprite
                gSpriteSheetTexture.render((133*4)*0.6, SCREEN_HEIGHT - gSpriteClips[2].h, &gSpriteClips[32]);

                //Render bottom right sprite
                gSpriteSheetTexture.render((133*5)*0.6, SCREEN_HEIGHT - gSpriteClips[3].h, &gSpriteClips[27]);

                //Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}