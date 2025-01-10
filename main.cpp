#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <queue>
#include <vector>
#include <cmath>
#include <set>
#include <stack>

#include <cstdlib>
#include <ctime>



using namespace std;

const int cols = 20;
const int rows = 20;
const int TILE_SIZE = 50;
const float WALL_THICKNESS = 3.0f;

struct Cell {

    int x;
    int y;
    bool visited;
    bool topWall;
    bool rightWall;
    bool bottomWall;
    bool leftWall;

    // for A*
    float f = 0; // total cost (g + h]
    float g = 0; // costy from start
    float h = 0; // heuristic
    Cell* parent;

    // for A* visualization
    bool inPath;
    bool explored;

    // default constructor
    Cell()
        : x(0)
        , y(0)
        , visited(false)
        , topWall(true)
        , rightWall(true)
        , bottomWall(true)
        , leftWall(true)
        
        , f(0)
        , g(0)
        , h(0)
        , parent(nullptr)
        , inPath(false)
        , explored(false)
    {
    }

 
    Cell(int xPos, int yPos)
        : x(xPos)
        , y(yPos)
        , visited(false)
        , topWall(true)
        , rightWall(true)
        , bottomWall(true)
        , leftWall(true)

        , f(0)
        , g(0)
        , h(0)
        , parent(nullptr)
        , inPath(false)
        , explored(false)
    {}

    void draw(sf::RenderWindow& window) {
        // make tile black if it has been visited
        if (visited) {
            sf::RectangleShape tile;
            tile.setPosition(x * TILE_SIZE, y * TILE_SIZE);
            tile.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            tile.setFillColor(sf::Color::Black);
            window.draw(tile);
        }

      // for A* search 
        if (explored) {

            sf::RectangleShape tile;
            tile.setPosition(x * TILE_SIZE, y * TILE_SIZE);
            tile.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            tile.setFillColor(sf::Color::Blue);
            window.draw(tile);

        }

        // for A* search
        if (inPath) {
            
            sf::RectangleShape tile;
            tile.setPosition(x * TILE_SIZE, y * TILE_SIZE);
            tile.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            tile.setFillColor(sf::Color::Green);
            window.draw(tile);
        }

       
        sf::RectangleShape wall;
        wall.setFillColor(sf::Color(100, 100, 0));

        if (topWall) {
            wall.setSize(sf::Vector2f(TILE_SIZE + WALL_THICKNESS, WALL_THICKNESS));
            wall.setPosition(x * TILE_SIZE, y * TILE_SIZE);
            window.draw(wall);
        }
        if (rightWall) {
            wall.setSize(sf::Vector2f(WALL_THICKNESS, TILE_SIZE + WALL_THICKNESS));
            wall.setPosition((x + 1) * TILE_SIZE, y * TILE_SIZE);
            window.draw(wall);
        }
        if (bottomWall) {
            wall.setSize(sf::Vector2f(TILE_SIZE + WALL_THICKNESS, WALL_THICKNESS));
            wall.setPosition(x * TILE_SIZE, (y + 1) * TILE_SIZE);
            window.draw(wall);
        }
        if (leftWall) {
            wall.setSize(sf::Vector2f(WALL_THICKNESS, TILE_SIZE + WALL_THICKNESS));
            wall.setPosition(x * TILE_SIZE, y * TILE_SIZE);
            window.draw(wall);
        }
    }

    void drawCurrentCell(sf::RenderWindow& window) {
       
       
        sf::RectangleShape tile;
        tile.setPosition(x * TILE_SIZE, y * TILE_SIZE);
        tile.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        tile.setFillColor(sf::Color::Red);
        window.draw(tile);
        
    }

   bool isValid(int x, int y) {
       
       if (x < 0 or x > cols - 1 or y < 0 or y > rows - 1) {
           return false;
       }
       return true;
       
    }

   vector<Cell*> getUnvisitedNeighbors(vector<vector<Cell>>& grid) {
       vector<Cell*>  neighbors;

       // four directions: top, right, bottom, left
       const int dx[] = { 0, 1, 0, -1 };  // x direction changes
       const int dy[] = { -1, 0, 1, 0 };  // y direction changes

       for (int i = 0; i < 4; i++) {
           int newX = x + dx[i];
           int newY = y + dy[i];

           // check if the neighbor is valid and unvisited
           if (isValid(newX, newY) && !grid[newX][newY].visited) {
               neighbors.push_back(&grid[newX][newY]);
           }
       }

       return neighbors;
   }

   Cell* getRandomUnvisitedNeighbor(vector<vector<Cell>>& grid) {
       vector<Cell*> neighbors = getUnvisitedNeighbors(grid);

       if (neighbors.empty()) {
           return new Cell(-10,-10);  
       }

       // random neighbor
       int randomIndex = rand() % neighbors.size();
       return neighbors[randomIndex];
   }

   // remove walls between current cell and next cell
   void removeWallsBetween(Cell& next) {
       int dx = next.x - x;
       int dy = next.y - y;

       if (dx == 1) {  // next cell is to the right
           rightWall = false;
           next.leftWall = false;
       }
       else if (dx == -1) {  // next cell is to the left
           leftWall = false;
           next.rightWall = false;
       }
       else if (dy == 1) {  // next cell is below
           bottomWall = false;
           next.topWall = false;
       }
       else if (dy == -1) {  // next cell is above
           topWall = false;
           next.bottomWall = false;
       }
   }


   vector<Cell*> getValidNeighborsForPath(vector<vector<Cell>>& grid) {
       vector<Cell*> neighbors;


       if (!topWall && isValid(x, y - 1)) {
           neighbors.push_back(&grid[x][y - 1]);
       }
       if (!rightWall && isValid(x + 1, y)) {
           neighbors.push_back(&grid[x + 1][y]);
       }
       if (!bottomWall && isValid(x, y + 1)) {
           neighbors.push_back(&grid[x][y + 1]);
       }
       if (!leftWall && isValid(x - 1, y)) {
           neighbors.push_back(&grid[x - 1][y]);
       }

       return neighbors;
   }


};

float heuristic(const Cell& a, const Cell& b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}


struct CompareCells {
    bool operator()(Cell* a, Cell* b) const {
        return (a->f < b->f) || (a->f == b->f && a < b);
    }
};


void astarSearch(vector<vector<Cell>>& grid, Cell* start, Cell* goal, sf::RenderWindow& window) {
    

    set<Cell*> closedSet;
    std::set<Cell*, CompareCells> openSet;

    openSet.insert(start);

    while (!openSet.empty()) {
        Cell* current = *openSet.begin();

  

        openSet.erase(openSet.begin());

        if (current == goal) {
            Cell* temp = current;
            while (temp != nullptr) {
                temp->inPath = true;
                temp = temp->parent;
            }
            return;
        }

        closedSet.insert(current);
        current->explored = true;

        vector<Cell*> neighbors = current->getValidNeighborsForPath(grid);
        for (Cell* neighbor : neighbors) {
            float tentative_g = current->g + 1;
            if (closedSet.find(neighbor) != closedSet.end() ||
                (openSet.find(neighbor) != openSet.end() && tentative_g >= neighbor->g)) {
                continue;
            }

            neighbor->parent = current;
            neighbor->g = tentative_g;
            neighbor->h = heuristic(*neighbor, *goal);
            neighbor->f = neighbor->g + neighbor->h;

            openSet.erase(neighbor); 
            openSet.insert(neighbor); 
        }
    

        window.clear(sf::Color::White);

   

        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                grid[i][j].draw(window);
            }
        }

       // current->drawCurrentCell(window);
      

        window.display();
        sf::sleep(sf::milliseconds(10));
    }
}





int main() {

    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(cols * TILE_SIZE, rows * TILE_SIZE), "Maze");

    // 2D vector of cells
    vector<vector<Cell>> grid(cols, vector<Cell>(rows));
    vector<Cell*> stack;
    Cell* current_cell = &grid[0][0];

    bool mazeGenerated = false;
    

    // each cell intialized with its coordinates
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            grid[i][j] = Cell(i, j);
        }
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

    
        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                grid[i][j].draw(window);
            }
        }

        // maze Generation using DFS backtracking algorithm
        //--------------------------------------------------------------------
        while (!mazeGenerated) {
            window.clear(sf::Color::White);

      
            for (int i = 0; i < cols; i++) {
                for (int j = 0; j < rows; j++) {
                    grid[i][j].draw(window);
                }
            }

            current_cell->visited = true;
            current_cell->drawCurrentCell(window);

            sf::sleep(sf::seconds(0.01));

            Cell* next_cell = current_cell->getRandomUnvisitedNeighbor(grid);
            if (next_cell->isValid(next_cell->x, next_cell->y)) {
                next_cell->visited = true;
                stack.push_back(current_cell);
                current_cell->removeWallsBetween(*next_cell);
                current_cell = next_cell;
            }
            else if (!stack.empty()) {
                current_cell = stack.back();
                stack.pop_back();
            }
            else {
                mazeGenerated = true;

            }
            window.display();
        }
        // ----------------------------------------------------------------------------
   
        // start A* pathfinding
        Cell* start = &grid[0][0];
        Cell* goal = &grid[19][19];
        astarSearch(grid, start, goal, window);
      
        
          

        window.display();  
    }


    return 0;
};

