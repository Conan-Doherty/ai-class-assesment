#include "vec.hpp"
#include "draw-triangle-pro.hpp"
#include "raylib-cpp.hpp"
#include "graph.hpp"
#include "graph-utils.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
std::vector<node_t> astar_pathfind(const Graph& g, node_t start, node_t goal)
{
  std::unordered_map<node_t, node_t> came_from;
  std::unordered_map<node_t, double> cost_so_far;
  a_star_search(g, start, goal, came_from, cost_so_far);
  std::vector<node_t> path = reconstruct_path(start, goal, came_from);
  return path;
}

unsigned int path_cost(const std::vector<node_t>& path)
{
  double dcost = 0;
  if (path.size() >= 2) // then we have some lines to draw
  {
    const int num_edges = path.size() - 1;
    for (int i = 0; i < num_edges; i++)
    {
      dcost = dcost + edge_info[std::pair{path[i], path[i+1]}];
    }
  }

  return static_cast<unsigned int>(dcost);
}

int main()
{
    InitAudioDevice();
    Sound GetTarget = LoadSound("../../deps/raylib-cpp-5.0.1/examples/audio/resources/coin.wav");
    Sound RemoveTarget = LoadSound("../../deps/raylib-cpp-5.0.1/examples/audio/resources/spring.wav");
    //Resets whenever other sound is use, figure out later. Sound BGM = LoadSound("../../deps/raylib-cpp-5.0.1/examples/audio/resources/country.mp3");
  const int w{ 960 }, h{ 540 }, half_w{ w/2 }, half_h{ h/2 }, gap{ w/8 };
  raylib::Window window{ w, h, "Pathfinder" };

  SetTargetFPS(60);

  Graph g;
  add_node(g, 'A', { half_w - gap, half_h });
  add_node(g, 'B', { half_w, half_h });
  add_node(g, 'C', { half_w, half_h - gap });
  add_node(g, 'D', { half_w, half_h + gap });
  add_node(g, 'E', { half_w + gap, half_h + gap });
  add_node(g, 'F', { half_w + gap, half_h });
  add_node(g, 'G', { half_w + (2 * gap), half_h - gap });
  add_double_edge(g, 'A', 'B');
  add_double_edge(g, 'B', 'C');
  add_double_edge(g, 'B', 'D');
  add_double_edge(g, 'C', 'A');
  add_double_edge(g, 'D', 'E');
  add_double_edge(g, 'D', 'A');
  add_double_edge(g, 'E', 'B');
  add_double_edge(g, 'B', 'F');
  add_double_edge(g, 'C', 'F');
  add_double_edge(g, 'C', 'G');
  add_double_edge(g, 'F', 'G');

  int t{60}; // time
  float float_time{ 60 };
  std::vector<node_t> player_path{};
  std::vector<node_t> neighbors{};
  std::vector<node_t> Perfect_Path{};

  srand(time(0));
  node_t start = 'A' + (rand() % 3);


  node_t end = 'D' + (rand() % 4);


  int tokens{2000}, score{}, high_score{}; // try with more/less tokens?



  while (!window.ShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(LIGHTGRAY);

    draw_graph(g);
   
    DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
    DrawText(TextFormat("Tokens: %d", tokens), 10, 40, 20, BLACK);
    DrawText(TextFormat("High Score: %d", high_score), 10, 70, 20, BLACK);
    DrawText(TextFormat("Time: %d", t), 10, 100, 20, BLACK);
    DrawCircleV(node_info[start], node_radius, GREEN);
    DrawCircleV(node_info[end], node_radius, RED);

    if (t > 0) {
        float_time -= GetFrameTime();
        t = float_time;
    }
    else {
        if (high_score < score) {
            high_score = score;
        }
        score = 0;
        t = 60;
        float_time = 60;
        tokens = 2000;
        start = 'A' + (rand() % 3);
        end = 'D' + (rand() % 4);
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        

      if (auto opt = get_nearby_node(GetMousePosition()))
      {
          

          int size = player_path.size();
          int i;
        // *opt is a node_t
          if (player_path.size() == 0) {

              player_path.push_back(start);
              PlaySound(GetTarget);
          }

          else {
              if (*opt == player_path.back()) {
                  node_t t = player_path.back();
                  player_path.pop_back();
                  tokens = tokens + static_cast<unsigned int>(g.cost(player_path.back(), t ));
                  PlaySound(RemoveTarget);
              }
              else
              {
                  neighbors = g.neighbors(player_path.back());
                  for (i = 0; i < neighbors.size(); i++)
                  {
                      if (*opt == neighbors[i]) {
                          tokens = tokens - static_cast<unsigned int>(g.cost(player_path.back(), *opt));
                          player_path.push_back(*opt);
                          PlaySound(GetTarget);
                      }
                      if (*opt == neighbors[i] && *opt == end) {
                          tokens = tokens + 300;
                          Perfect_Path = astar_pathfind(g, start, end);
                          score = score + path_cost(Perfect_Path);

                          player_path.clear();

                          
                          start = 'A' + (rand() % 3);
                          end = 'D' + (rand() % 4);
                      }
                  }
              }
          }
          
          
          

         

      }
    }


    for (size_t i = 0; i < player_path.size(); i++)
    {
        if (i > 0) {
            DrawLineEx(node_info[player_path[i - 1]], node_info[player_path[i]], line_thickness, BLUE);
        }
        DrawCircleV(node_info[player_path[i]], node_radius, BLUE);
    }
    EndDrawing();
  }
  
  return 0;
}

