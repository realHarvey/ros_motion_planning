#ifndef LPA_STAR_H
#define LPA_STAR_H

#include <ros/ros.h>

#include <map>
#include <algorithm>

#include "global_planner.h"

#define INF 10000       // infinity, a big enough number
#define WINDOW_SIZE 70  // local costmap window size (in grid, 3.5m / 0.05 = 70)

namespace lpa_star_planner
{
class LNode;
typedef LNode* LNodePtr;

/**
 * @brief Class for objects that plan using the LPA* algorithm
 */
class LPAStar : public global_planner::GlobalPlanner
{
public:
  /**
   * @brief Construct a new LPAStar object
   *
   * @param nx          pixel number in costmap x direction
   * @param ny          pixel number in costmap y direction
   * @param resolution  costmap resolution
   */
  LPAStar(int nx, int ny, double resolution);

  /**
   * @brief Init map
   */
  void initMap();

  /**
   * @brief Reset the system
   */
  void reset();

  /**
   * @brief Get heuristics between n1 and n2
   *
   * @param n1  LNode pointer of on LNode
   * @param n2  LNode pointer of the other LNode
   * @return heuristics between n1 and n2
   */
  double getH(LNodePtr n1, LNodePtr n2);

  /**
   * @brief Calculate the key of s
   *
   * @param s LNode pointer
   * @return the key value
   */
  double calculateKey(LNodePtr s);

  /**
   * @brief Check if there is collision between n1 and n2
   *
   * @param n1  DNode pointer of one DNode
   * @param n2  DNode pointer of the other DNode
   * @return true if collision, else false
   */
  bool isCollision(LNodePtr n1, LNodePtr n2);

  /**
   * @brief Get neighbour LNodePtrs of nodePtr
   *
   * @param node_ptr    DNode to expand
   * @param neighbours  neigbour LNodePtrs in vector
   */
  void getNeighbours(LNodePtr u, std::vector<LNodePtr>& neighbours);

  /**
   * @brief Get the cost between n1 and n2, return INF if collision
   *
   * @param n1 LNode pointer of one LNode
   * @param n2 LNode pointer of the other LNode
   * @return cost between n1 and n2
   */
  double getCost(LNodePtr n1, LNodePtr n2);

  /**
   * @brief Update vertex u
   * 
   * @param u LNode pointer to update 
   */
  void updateVertex(LNodePtr u);

  /**
   * @brief Main process of LPA*
   */
  void computeShortestPath();

  /**
   * @brief Extract path for map
   *
   * @param start start node
   * @param goal  goal node
   */
  void extractPath(const Node& start, const Node& goal);

  /**
   * @brief Get the closest Node of the path to current state
   *
   * @param current current state
   * @return the closest Node
   */
  Node getState(const Node& current);

  /**
   * @brief LPA* implementation
   * @param costs   costmap
   * @param start   start node
   * @param goal    goal node
   * @param expand  containing the node been search during the process
   * @return tuple contatining a bool as to whether a path was found, and the path
   */
  std::tuple<bool, std::vector<Node>> plan(const unsigned char* costs, const Node& start, const Node& goal,
                                           std::vector<Node>& expand);

public:
  // global costmap
  unsigned char* curr_global_costmap_;
  unsigned char* last_global_costmap_;
  // grid pointer map
  LNodePtr** map_;
  // open list, ascending order
  std::multimap<double, LNodePtr> open_list_;
  // path
  std::vector<Node> path_;
  // expand
  std::vector<Node> expand_;
  // start and goal
  Node start_, goal_;
  // start and goal ptr
  LNodePtr start_ptr_, goal_ptr_;
};

class LNode : public Node
{
public:
  /**
   * @brief Construct a new LNode object
   * 
   * @param x       X value
   * @param y       Y value
   * @param cost    Cost to get to this node
   * @param h_cost  Heuritic cost of this node
   * @param id      Node's id
   * @param pid     Node's parent's id
   * @param rhs     Node's right hand side
   * @param key     Node's key value
   */
  LNode(const int x = 0, const int y = 0, const double cost = INF, const double h_cost = INF, const int id = 0,
        const int pid = -1, const double rhs = INF, const double key = INF)
    : Node(x, y, cost, h_cost, id, pid), rhs(rhs), key(key)
  {
  }

public:
  // minimum cost moving from start(value)
  double rhs;
  // priority
  double key;
  // iterator
  std::multimap<double, LNodePtr>::iterator open_it;
};
}  // namespace lpa_star_planner

#endif