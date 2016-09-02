//
//  cave_escape10.cpp
//  splashkit
//
//  Created by Sylvie Trewhella on 2/09/2016.
//  Copyright © 2016 Andrew Cain. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "graphics.h"
#include "keyboard_input.h"
#include "text.h"
#include "bundles.h"
#include "music.h"
#include "input.h"




using namespace std;


#define GRAVITY 0.08
#define MAX_SPEED 5
#define JUMP_RECOVERY_BOOST 2
#define FOREGROUND_FOREROOF_POLE_SCROLL_SPEED -2
#define BACKGROUND_SCROLL_SPEED -1
#define NUM_POLES 4

typedef struct pole_data
{
    bool score_limiter;
    sprite up_pole, down_pole;
} pole_data;

typedef struct pole_data poles[NUM_POLES];

typedef struct background_data
{
    sprite foreroof, foreground, background;
} background_data;

enum player_state{MENU, PLAY};

typedef struct player
{
    sprite sprite;
    int score;
    bool is_dead;
    player_state state;
} player;

typedef struct game_data
{
    player player;
    background_data scene;
    poles poles;
} game_data;

player get_new_player()
{
    player result;
    
    result.sprite = create_sprite(bitmap_named("Player"), animation_script_named("PlayerAnimations"));
    sprite_set_x(result.sprite, screen_width() / 2 - sprite_width(result.sprite));
    sprite_set_y(result.sprite, screen_height() / 2);
    sprite_start_animation(result.sprite, "Fly");
    result.score = 0;
    result.is_dead = false;
    result.state = MENU;
    
    return result;
}

pole_data get_random_poles()
{
    pole_data result;
    
    result.up_pole = create_sprite(bitmap_named("UpPole"));
    result.down_pole = create_sprite(bitmap_named("DownPole"));
    srand(time(NULL));
    sprite_set_x(result.up_pole, screen_width() + rand() % 1201);
    sprite_set_y(result.up_pole, screen_height() - sprite_height(result.up_pole) -  rand() % bitmap_height(bitmap_named("Foreground")) + 1);
    sprite_set_x(result.down_pole, sprite_x(result.up_pole));
    sprite_set_y(result.down_pole, rand() % bitmap_height(bitmap_named("Foreroof")) + 1);
    sprite_set_dx(result.up_pole, FOREGROUND_FOREROOF_POLE_SCROLL_SPEED);
    sprite_set_dx(result.down_pole, FOREGROUND_FOREROOF_POLE_SCROLL_SPEED);
    result.score_limiter = true;
    
    return result;
}

background_data get_new_background()
{
    background_data result;
    
    result.background = create_sprite(bitmap_named("Background"));
    sprite_set_x(result.background, 0);
    sprite_set_y(result.background, 0);
    sprite_set_dx(result.background, BACKGROUND_SCROLL_SPEED);
    
    result.foreground = create_sprite(bitmap_named("Foreground"), animation_script_named("ForegroundAminations"));
    sprite_set_x(result.foreground, 0);
    sprite_set_y(result.foreground, screen_height() - sprite_height(result.foreground));
    sprite_set_dx(result.foreground, FOREGROUND_FOREROOF_POLE_SCROLL_SPEED);
    sprite_start_animation(result.foreground, "Fire");
    
    result.foreroof = create_sprite(bitmap_named("Foreroof"));
    sprite_set_x(result.foreroof, 0);
    sprite_set_y(result.foreroof, 0);
    sprite_set_dx(result.foreroof, FOREGROUND_FOREROOF_POLE_SCROLL_SPEED);
    
    return result;
}

void handle_input(player *player)
{
    if (key_typed(SPACE_KEY) && (player->state = PLAY))
    {
        sprite_set_dy(player->sprite, sprite_dy(player->sprite) - JUMP_RECOVERY_BOOST);
    }
    else if (key_typed(SPACE_KEY))
    {
        player->state = PLAY;
    }
}

void check_for_collisions(game_data *game)
{
    int i;
    
    if (sprite_collision(game->player.sprite, game->scene.foreground) || sprite_collision(game->player.sprite, game->scene.foreroof))
    {
        game->player.is_dead = true;
        return;
    }
    
    for (i = 0; i < NUM_POLES; i++)
    {
        if (sprite_collision(game->player.sprite, game->poles[i].up_pole) || sprite_collision(game->player.sprite, game->poles[i].down_pole))
        {
            game->player.is_dead = true;
            return;
        }
    }
}

void reset_pole_data(pole_data *poles)
{
    free_sprite(poles->up_pole);
    free_sprite(poles->down_pole);
    *poles = get_random_poles();
}

void reset_player(player *player)
{
    free_sprite(player->sprite);
    *player = get_new_player();
}

void reset_game(game_data *game)
{
    int i;
    
    reset_player(&game->player);
    for (i = 0; i < NUM_POLES; i++)
    {
        reset_pole_data(&game->poles[i]);
    }
}

void update_velocity(sprite player)
{
    sprite_set_dy(player, sprite_dy(player) + GRAVITY);
    
    if (sprite_dy(player) > MAX_SPEED)
    {
        sprite_set_dy(player, MAX_SPEED);
    }
    else if (sprite_dy(player) < -(MAX_SPEED))
    {
        sprite_set_dy(player, -(MAX_SPEED));
    }
}

void update_poles(pole_data *poles, player *player)
{
    update_sprite(poles->up_pole);
    update_sprite(poles->down_pole);
    
    if ((sprite_x(poles->up_pole) < sprite_x(player->sprite)) && (poles->score_limiter == true))
    {
        poles->score_limiter = false;
        player->score++;
    }
    
    if ((sprite_x(poles->up_pole) + sprite_width(poles->up_pole) < 0) && (sprite_x(poles->down_pole) + sprite_width(poles->down_pole) < 0))
    {
        reset_pole_data(poles);
    }
}

void update_poles_array(poles poles_array, player *player)
{
    int i;
    
    for (i = 0; i < NUM_POLES; i++)
    {
        update_poles(&poles_array[i], player);
    }
}

void update_background(background_data scene)
{
    update_sprite(scene.foreground);
    update_sprite(scene.foreroof);
    update_sprite(scene.background);
    
    if (sprite_x(scene.foreground) <= -(sprite_width(scene.foreground) / 2))
    {
        sprite_set_x(scene.foreground, 0);
        sprite_set_x(scene.foreroof, 0);
    }
    if (sprite_x(scene.background) <= -(sprite_width(scene.background) / 2))
    {
        sprite_set_x(scene.background, 0);
    }
}

void update_player(player player)
{
    if (player.state == PLAY)
    {
        update_velocity(player.sprite);
    }
    update_sprite(player.sprite);
}

void update_game(game_data *game)
{
    if (!game->player.is_dead)
    {
        check_for_collisions(game);
        handle_input(&game->player);
        update_background(game->scene);
        update_player(game->player);
        if (game->player.state == PLAY)
        {
            update_poles_array(game->poles, &game->player);
        }
    }
    else
    {
        reset_game(game);
    }
}

void draw_poles(pole_data poles)
{
    draw_sprite(poles.up_pole);
    draw_sprite(poles.down_pole);
}

void draw_poles_array(poles poles_array)
{
    int i;
    
    for (i = 0; i < NUM_POLES; i++)
    {
        draw_poles(poles_array[i]);
    }
}

void draw_game(game_data *game)
{
    char str[15];
    
    sprintf(str, "%d", game->player.score);
    
    draw_sprite(game->scene.background);
    draw_poles_array(game->poles);
    draw_sprite(game->scene.foreroof);
    draw_sprite(game->scene.foreground);
    draw_sprite(game->player.sprite);
    if (game->player.state == PLAY)
    {
        draw_text(str, COLOR_WHITE, "GameFont", 10, 0);
    }
    else if (game->player.state == MENU)
    {
        draw_bitmap(bitmap_named("Logo"), 0, 40);
        draw_text("PRESS SPACE!",
                  COLOR_WHITE,
                  "GameFont",
                  180, 680);
    }
}

void set_up_game(game_data *game)
{
    int i;
    
    load_resource_bundle("CaveEscape", "CaveEscape.txt");
    for (i = 0; i < NUM_POLES; i++)
    {
        game->poles[i] = get_random_poles();
    }
    game->player = get_new_player();
    game->scene = get_new_background();
    fade_music_in("GameMusic", -1, 15000);
}

int run_cave_escape()
{
    game_data game;
    
    open_window("Cave Escape", 432, 768);
    set_up_game(&game);
    
    do
    {
        process_events();
        clear_screen(COLOR_WHITE);
        update_game(&game);
        draw_game(&game);
        refresh_screen();
        
    } while(!window_close_requested("Cave Escape"));
    
    return 0;
}
