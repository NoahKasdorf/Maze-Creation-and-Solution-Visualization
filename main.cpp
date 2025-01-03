#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <vector>
#include <cmath>

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

    // default constructor
    Cell()
        : x(0)
        , y(0)
        , visited(false)
        , topWall(true)
        , rightWall(true)
        , bottomWall(true)
        , leftWall(true)
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
    
   // Check if a cell position is valid within the grid
   bool isValid(int x, int y) {
       
       if (x < 0 or x > cols - 1 or y < 0 or y > rows - 1) {
           return false;
       }
       return true;
       
    }

   // available unvisited neighbors
   vector<Cell*> getUnvisitedNeighbors(vector<vector<Cell>>& grid) {
       vector<Cell*>  neighbors;

       // four directions: top, right, bottom, left
       const int dx[] = { 0, 1, 0, -1 };  // x direction changes
       const int dy[] = { -1, 0, 1, 0 };  // y direction changes

       for (int i = 0; i < 4; i++) {
           int newX = x + dx[i];
           int newY = y + dy[i];

           // Check if the neighbor is valid and unvisited
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


};






int main() {

    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(cols * TILE_SIZE, rows * TILE_SIZE), "Maze");

    // 2D vector of cells
    vector<vector<Cell>> grid(cols, vector<Cell>(rows));
    vector<Cell*> stack;
    Cell* current_cell = &grid[0][0];
    

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

        //Maze Generation using DFS backtracking algorithm
        // -----------------------------------------------------------------------
        // Draw all cells
        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                grid[i][j].draw(window);
            }
        }
        current_cell->visited = true;
        current_cell->drawCurrentCell(window);

        Cell* next_cell = current_cell->getRandomUnvisitedNeighbor(grid);
        if (next_cell->isValid(next_cell->x, next_cell->y)) {
            next_cell->visited = true;
            stack.push_back(current_cell);
            current_cell->removeWallsBetween(*next_cell);
            current_cell = next_cell;
        }
        else if(!stack.empty()){
            current_cell = stack.back();
            stack.pop_back();
        }

        // ----------------------------------------------------------------------------

        window.display();  // display the drawn frame
    }

    return 0;
};

