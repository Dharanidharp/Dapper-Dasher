#include "raylib.h"

struct AnimData
{
    Rectangle rect;
    Vector2 position;
    int frame;
    float update_time;
    float running_time;
};

bool isOnGround(AnimData data, int height)
{
    return data.position.y >= height - data.rect.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // update running time
    data.running_time += deltaTime;

    if(data.running_time >= data.update_time)
    {
        data.running_time = 0.0;
        // update animation frame
        data.rect.x = data.frame * data.rect.width;
        data.frame++;

        if(data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }

    return data;
}

int main()
{
    // Game window
    const int window_dimensions[2] {720, 480}; // width and height

    // Initialise game window
    InitWindow(window_dimensions[0], window_dimensions[1], "Dapper Dasher");

    // Acceleration due to gravity - (pixels/sec)/sec
    const int gravity{1000};

    // Jump variables
    bool isInAir{}; // rectangle in air
    const int jumpVelocity{-600}; // pixels/sec

    // background
    Texture2D background = LoadTexture("textures/far-buildings.png");

    // midground
    Texture2D midground = LoadTexture("textures/back-buildings.png");

    // foreground
    Texture2D foreground = LoadTexture("textures/foreground.png");

    // obstacle variables
    Texture2D obstacle = LoadTexture("textures/12_nebula_spritesheet.png");

    const int sizeOfObstacles{10};
    AnimData nebulae[sizeOfObstacles] {};

    for(int i = 0; i < sizeOfObstacles; i++)
    {
        nebulae[i].rect.x = 0.0;
        nebulae[i].rect.y = 0.0;
        nebulae[i].rect.width = obstacle.width/8;
        nebulae[i].rect.height = obstacle.height/8;
        nebulae[i].position.y = window_dimensions[1] - obstacle.height/8;
        nebulae[i].frame = 0;
        nebulae[i].running_time = 0.0;
        nebulae[i].update_time = 0.0;
        nebulae[i].position.x = window_dimensions[0] + i * 300;
    }

    // obstacle X velocity (pixels/sec)
    int obstacle_velocity{-200};

    float finish_line { nebulae[sizeOfObstacles - 1].position.x };

    // character variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfy_data;
    scarfy_data.rect.width = scarfy.width/6;
    scarfy_data.rect.height = scarfy.height;
    scarfy_data.rect.x = 0.0;
    scarfy_data.rect.y = 0.0;
    scarfy_data.position.x = window_dimensions[0]/2 - scarfy_data.rect.width/2;
    scarfy_data.position.y = window_dimensions[1] - scarfy_data.rect.height;
    scarfy_data.frame = 0;
    scarfy_data.update_time = 1.0/12.0;
    scarfy_data.running_time = 0.0;

    int velocity{0}; // change in position over time - m/s, km/s, pixels/frame

    SetTargetFPS(60);

    bool collision{};
    
    float bgX{};
    float mgX{};
    float fgX{};

    // Game window loop
    while(!WindowShouldClose())
    {
        // Delta time - time since last frame
        const float delta_time{GetFrameTime()};

        BeginDrawing();

        ClearBackground(WHITE);

        bgX -= 20 * delta_time; // Move BG left

        if(bgX <= -background.width*2)
        {
            bgX = 0.0;
        }

        mgX -= 40 * delta_time; // Move MG left

        if(mgX <= -midground.width*2)
        {
            mgX = 0.0;
        }

                
        fgX -= 80 * delta_time; // Move FG left

        if(fgX <= -foreground.width*2)
        {
            fgX = 0.0;
        }
        
        // Draw BG
        Vector2 BG1Pos{bgX, 0.0};
        DrawTextureEx(background, BG1Pos, 0.0, 2.5, WHITE);
        Vector2 BG2Pos{bgX + background.width*2, 0.0};
        DrawTextureEx(background, BG2Pos, 0.0, 2.5, WHITE);

        // Draw MG
        Vector2 MG1Pos{mgX, 0.0};
        DrawTextureEx(midground, MG1Pos, 0.0, 2.5, WHITE);
        Vector2 MG2Pos{mgX + midground.width*2, 0.0};
        DrawTextureEx(midground, MG2Pos, 0.0, 2.5, WHITE);

        // Draw FG
        Vector2 FG1Pos{fgX, 0.0};
        DrawTextureEx(foreground, FG1Pos, 0.0, 2.5, WHITE);
        Vector2 FG2Pos{fgX + foreground.width*2, 0.0};
        DrawTextureEx(foreground, FG2Pos, 0.0, 2.5, WHITE);

        if(isOnGround(scarfy_data, window_dimensions[1]))
        {
            // set velocity when on ground
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // apply gravity when in air
            velocity += gravity * delta_time;
            isInAir = true;
        }

        // Rectangle jump
        if(IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVelocity;
        }

        // update obstacles position
        for(int i = 0; i < sizeOfObstacles; i++)
        {
            nebulae[i].position.x += obstacle_velocity * delta_time;
        }

        finish_line += obstacle_velocity * delta_time;

        // update scarfy position
        scarfy_data.position.y += velocity * delta_time;
        
        // sprite animation - scarfy
        if(!isInAir)
        {
            scarfy_data = updateAnimData(scarfy_data, delta_time, 5);
        }

        // sprite animation - obstacles
        for(int i = 0; i < sizeOfObstacles; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], delta_time, 7);
        }

        // Collision
        for(AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle neb_rect
            {
                nebula.position.x + pad,
                nebula.position.y + pad,
                nebula.rect.width - 2*pad,
                nebula.rect.height - 2*pad
            };
            Rectangle scarfy_rect
            {
                scarfy_data.position.x,
                scarfy_data.position.y,
                scarfy_data.rect.width,
                scarfy_data.rect.height
            };

            if(CheckCollisionRecs(neb_rect, scarfy_rect))
            {
                collision = true;
            }
        }

        if(collision)
        {
            // lose game
            DrawText("Game Over!", window_dimensions[0]/3, window_dimensions[1]/2, 50, RED);
        }
        else if(scarfy_data.position.x >= finish_line)
        {
            DrawText("You win!", window_dimensions[0]/3, window_dimensions[1]/2, 50, GREEN);
        }
        else
        {
            // Draw obstacle
            for(int i = 0; i < sizeOfObstacles; i++)
            {
                DrawTextureRec(obstacle, nebulae[i].rect, nebulae[i].position, WHITE);
            }
            
            // Draw character
            DrawTextureRec(scarfy, scarfy_data.rect, scarfy_data.position, WHITE);
        }

        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(obstacle);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);

    CloseWindow();
}