//
//  test_main.cpp
//  splashkit
//
//  Created by Andrew Cain on 24/06/2016.
//  Copyright © 2016 Andrew Cain. All rights reserved.
//

#include "input.h"

#include "test_main.h"

#include <iostream>
#include <climits>
#include <functional>
#include <utility>
#include <string>
#include <vector>

using namespace std;

vector<pair<string, function<void()>>> tests;

void add_test(const string &name, const function<void()> &f)
{
    tests.push_back({name, f});
}

void setup_tests()
{
    add_test("Animations", run_animation_test);
    add_test("Audio", run_audio_tests);
    add_test("Bundles", run_bundle_test);
    add_test("Camera", run_camera_test);
    add_test("Database", run_database_tests);
    add_test("Geometry", run_geometry_test);
    add_test("Graphics", run_graphics_test);
    add_test("Input", run_input_test);
    add_test("Physics", run_physics_test);
    add_test("Resources", run_resources_tests);
    add_test("Shape drawing", run_shape_drawing_test);
    add_test("Sprite tests", run_sprite_test);
    add_test("Text", run_text_test);
    add_test("Timers", run_timer_test);
    add_test("Web Server", run_web_server_tests);
    add_test("Windows", run_windows_tests);
    add_test("cave-escape", run_cave_escape);
}

int main()
{
    setup_tests();

    int opt;
    do
    {
        cout << "---------------------" << endl;
        cout << " SplashKit Dev Tests " << endl;
        cout << "---------------------" << endl;
        cout << " -1: Quit" << endl;

        for (int i = 0; i < tests.size(); ++i)
        {
            cout << " " << i << ": " << tests[i].first << endl;
        }

        cout << "---------------------" << endl;
        cout << " Select test to run: ";

//        cin.ignore(INT_MAX);
        cin >> opt;
        cin.clear();
        cin.ignore(INT_MAX,'\n');

        if (opt >= 0 && opt < tests.size())
        {
            tests[opt].second();
        }

        reset_quit();
    } while (opt != -1);
    return 0;
}

