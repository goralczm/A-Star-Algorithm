#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <queue>
#include <algorithm>
#include <iomanip>

using namespace std;

class Point {
public:
    int x;
    int y;
    float heuristic;
    int distanceFromStart;
    Point* parent = nullptr;

    Point(int x, int y) : x(x), y(y), heuristic(0), distanceFromStart(INT_MAX) {}

    void print() {
        cout << "(" << this->x << "," << this->y << ")" << endl;
    }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

float heuristic(int currX, int currY, int goalX, int goalY) {
    return sqrt(pow(currX - goalX, 2) + pow(currY - goalY, 2));
}

vector<string> split(const string& s, const string& delimiter) {
    size_t pos_start = 0, pos_end;
    string token;
    vector<string> res;
    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delimiter.length();
        res.push_back(token);
    }
    res.push_back(s.substr(pos_start));
    return res;
}

int** readGrid(int width, int height) {
    int** grid = new int* [height];
    ifstream file("grid.txt");
    string line;
    int row = 0;
    while (getline(file, line) && row < height) {
        grid[row] = new int[width];
        vector<string> cells = split(line, " ");
        for (int col = 0; col < width; ++col) {
            grid[row][col] = stoi(cells[col]);
        }
        row++;
    }
    file.close();
    return grid;
}

bool isInsideGrid(int width, int height, int x, int y) {
    return x >= 0 && x < height && y >= 0 && y < width;
}

bool isPointAlreadyInList(const vector<Point*>& list, const Point& searching) {
    for (const auto& p : list) {
        if (p->x == searching.x && p->y == searching.y)
            return true;
    }
    return false;
}

Point* findMinHeuristic(vector<Point*>& open) {
    auto minIter = min_element(open.begin(), open.end(), [](Point* a, Point* b) { return a->heuristic < b->heuristic; });
    Point* minPoint = *minIter;
    open.erase(minIter);
    return minPoint;
}

void printGrid(int** grid, int width, int height, const vector<Point*>& open, const vector<Point*>& closed, Point* current, Point* start, Point* goal, const vector<Point*>& path) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (start->x == i && start->y == j)
                cout << "s ";
            else if (goal->x == i && goal->y == j)
                cout << "g ";
            else if (current && current->x == i && current->y == j)
                cout << "C ";
            else if (any_of(open.begin(), open.end(), [i, j](Point* p) { return p->x == i && p->y == j; }))
                cout << "? ";
            else if (any_of(closed.begin(), closed.end(), [i, j](Point* p) { return p->x == i && p->y == j; }))
                cout << "X ";
            else if (any_of(path.begin(), path.end(), [i, j](Point* p) { return p->x == i && p->y == j; }))
                cout << "^ ";
            else
                cout << (grid[i][j] == 5 ? "# " : ". ");
        }
        cout << endl;
    }
    cout << "----------------------" << endl;
}

int main() {
    vector<Point*> path;
    vector<Point*> open;
    vector<Point*> closed;

    int width = 0;
    int height = 0;

    cout << "Podaj wysokosc siatki: ";
    cin >> height;
    cout << endl << "Podaj szerokosc siatki: ";
    cin >> width;

    if (width <= 0 || height <= 0)
    {
        cout << "Niepoprawne wymiary siati";
        return 0;
    }

    height++;
    width++;

    int** grid = readGrid(width, height);

    for (int h = 0; h < height; h++)
    {
        if (h == 0)
        {
            cout << "   ";
            for (int w = 0; w < width; w++)
            {
                cout << w << " ";
            }
            cout << endl;
        }

        if (h < 10)
            cout << h << "  ";
        else
            cout << h << " ";

        for (int w = 0; w < width; w++)
        {
            if (grid[h][w] == 5)
                cout << "# ";
            else
                cout << ". ";
        }
        cout << endl;
    }

    int startX, startY = 0;
    int goalX, goalY = 0;

    cout << "Podaj punkt startowy (szerokosc): ";
    cin >> startX;
    cout << "Podaj punkt startowy (wysokosc): ";
    cin >> startY;
    cout << "Podaj punkt koncowy (szerokosc): ";
    cin >> goalX;
    cout << "Podaj punkt koncowy (wysokosc): ";
    cin >> goalY;

    Point start(startY, startX);
    start.distanceFromStart = 0;
    Point goal(goalY, goalX);

    open.push_back(&start);

    bool foundSolution = false;
    Point* current = nullptr;

    while (!open.empty()) {
        current = findMinHeuristic(open);

        if (*current == goal) {
            foundSolution = true;
            break;
        }

        closed.push_back(current);

        printGrid(grid, width, height, open, closed, current, &start, &goal, path);

        vector<pair<int, int>> dirs = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
        for (auto& dir : dirs) {
            int newX = current->x + dir.first;
            int newY = current->y + dir.second;

            if (isInsideGrid(width, height, newX, newY) && grid[newX][newY] != 5) {
                Point* neighbor = new Point(newX, newY);

                if (isPointAlreadyInList(closed, *neighbor)) {
                    delete neighbor;
                    continue;
                }

                int newDistance = current->distanceFromStart + 1;

                bool isAlreadyInOpenList = isPointAlreadyInList(open, *neighbor);
                if (!isAlreadyInOpenList || newDistance < neighbor->distanceFromStart) {
                    neighbor->distanceFromStart = newDistance;
                    neighbor->heuristic = newDistance + heuristic(newX, newY, goal.x, goal.y);
                    neighbor->parent = current;

                    if (!isAlreadyInOpenList)
                        open.push_back(neighbor);
                }
                else
                    delete neighbor;
            }
        }
    }

    if (foundSolution) {
        cout << "Znaleziono drogę:" << endl;
        while (current != nullptr) {
            current->print();
            path.push_back(current);
            current = current->parent;
        }
        cout << endl;
    }
    else
        cout << "Cel nie może być osiągnięty" << endl;

    open.clear();
    closed.clear();

    printGrid(grid, width, height, open, closed, current, &start, &goal, path);

    for (int h = 0; h < height; h++)
        delete[] grid[h];
    delete[] grid;
    grid = 0;

    return 0;
}
