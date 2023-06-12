#pragma once

const int WINDOW_WIDTH = 1200;	// ekrano plotis
const int WINDOW_HEIGHT = 720;	// ekrano aukstis
const int CAMERA_WIDTH = WINDOW_WIDTH / 5 * 4;   //5:3
const int CAMERA_HEIGHT = WINDOW_HEIGHT / 5 * 4;
const float gravity = 0.5f;
const float friction = gravity * 10;

const float round_start = 150;
const float stage = WINDOW_HEIGHT - 40;
enum side { left, right, none };
enum state { neutral, block, hit, attacking };
enum position { ground, air };
