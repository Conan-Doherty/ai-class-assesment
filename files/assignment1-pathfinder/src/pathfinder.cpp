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
  std::vector<node_t> player_path{};
  std::vector<node_t> neighbors{};
  srand(time(0));
  node_t start = 'A' + (rand() % 5);


  node_t end = 'D' + (rand() % 3);

  
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
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
       
      if (auto opt = get_nearby_node(GetMousePosition()))
      {
          int size = player_path.size();
          int i;
        // *opt is a node_t
          if (player_path.size() == 0) {

              player_path.push_back(start);
          }


          neighbors = g.neighbors(player_path.back());
          for (i = 0; i < neighbors.size(); i++)
          {
              if (*opt == neighbors[i]) {
                  player_path.push_back(*opt);
              }
          }

          /*for (const node_t& i : player_path) {
              if (opt = i) {
                  
              }
              else {
                  player_path.push_back(*opt);
                  
              }
         }
          */
          

         

      }
    }


    for (size_t i = 1; i < player_path.size(); i++)
    {
        
        DrawLineEx(node_info[player_path[i - 1]], node_info[player_path[i]], line_thickness, BLUE);
        DrawCircleV(node_info[player_path[i-1]], node_radius, BLUE);
    }
    EndDrawing();
  }
  
  return 0;
}

