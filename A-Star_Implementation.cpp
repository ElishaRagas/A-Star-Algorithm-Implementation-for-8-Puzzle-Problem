// RAGAS, Elisha John D.
// BSCS - 3C

// Machine Problem 1 - Implementaion of A* Algortihm to solve 8-Puzzle Problem

// g++ RAGAS_MP1-A-Star.cpp -o RAGAS_MP1-A-Star
// Used list instead of stacks and queues

#include <iostream>     // standard i/o
#include <cmath>        // to get abs() value
#include <list>         // sequenced container for open and close listw

using namespace std;

// const size of board
const int numRows = 3;
const int numCols = 3;
// declared globally, will be used to check for hammingCost
char goal_TileValue[numRows][numCols] =  {{'1', '2', '3'}, {'8', '0', '4'}, {'7', '6', '5'}};
char heuristicUsed;

// attributes are defined during construction of states
class State {
    private:
        // Attributes
        int manhattanCost;
        int hammingCost;
        int gCost;
    public:
        // Attributes
        int fCost;
        char tileValue[numRows][numCols]; 
        int coordinate_zero_x = -1;         // x == row
        int coordinate_zero_y = -1;         // y == column
        State* parent;                      // pointer to parent state

        // Constructor
        State(char tileValue[][numCols], State* parent) {
            // initialize address of parent state
            this->parent = parent;
            // initialize tileValue
            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    this->tileValue[i][j] = tileValue[i][j];
                    if (this->tileValue[i][j] == '0') {
                        coordinate_zero_x = i;
                        coordinate_zero_y = j;
                    } 
                }
            }
            // initialize computed informtion
            // prevents repeated computations
            setManhattan(); // h1
            setHamming();   // h2
            set_gCost();
            set_fCost();
        }
        
        // Setter
        void setManhattan() {
            int h_cost = 0;
            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    switch (this->tileValue[i][j]) {
                        case '0':
                            h_cost += (abs(i - 1)) + (abs(j - 1));
                            break;
                        case '1':
                            h_cost += (abs(i - 0)) + (abs(j - 0));
                            break;
                        case '2':
                            h_cost += (abs(i - 0)) + (abs(j - 1));
                            break;
                        case '3':
                            h_cost += (abs(i - 0)) + (abs(j - 2));
                            break;
                        case '4':
                            h_cost += (abs(i - 1)) + (abs(j - 2));
                            break;
                        case '5':
                            h_cost += (abs(i - 2)) + (abs(j - 2));
                            break;
                        case '6':
                            h_cost += (abs(i - 2)) + (abs(j - 1));
                            break;
                        case '7':
                            h_cost += (abs(i - 2)) + (abs(j - 0));
                            break;
                        case '8':
                            h_cost += (abs(i - 1)) + (abs(j - 0));
                            break;
                    }
                }
            }
            manhattanCost = h_cost;
        }
        void setHamming() {
            int h_cost = 0;
            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    if (this->tileValue[i][j] != goal_TileValue[i][j]) h_cost += 1;
                }
            }
            hammingCost = h_cost;
        }
        void set_gCost() {
            int g_cost = 0;
            if (this->parent == NULL) g_cost = 0;
            else {
                State *temp = this->parent;
                g_cost += 1;
                while (temp->parent != NULL) {
                    g_cost += 1;
                    temp = temp->parent;
                }
            }
            gCost =  g_cost;
        }
        void set_fCost() {
            if (heuristicUsed == 'h') fCost = hammingCost + gCost;
            else fCost = manhattanCost + gCost;
        }

        // Getter
        int getManhattan() {
            return manhattanCost;
        }
        int getHamming() {
            return hammingCost;
        }
        int get_gCost() {
            return gCost;
        }

        // Methods : for utility
        void displayBoard() {
            cout << "+---+---+---+\n";
            for (int i = 0; i < numRows; i++) {
                cout << "| ";
                for (int j = 0; j < numCols; j++) {
                    if (this->tileValue[i][j] == '0') cout << " " << " | ";
                    else cout << this->tileValue[i][j] << " | ";
                }
                cout << "\n+---+---+---+\n";
            }
            // used for testing
            // cout << "fcost: " << fCost() << endl;
            // cout << "gCost: " << get_gCost() << endl; 
            // cout << "hamming: " << getHamming() << endl;
            // cout << "manhattan: " << getManhattan() << endl;
        }
        // used for testing
        void displayLine() {
            for (int i = 0; i < numRows; i++) {
                for (int j = 0; j < numCols; j++) {
                    cout << this->tileValue[i][j] << " ";
                }
            }
            cout << " | ";
        }
};

// create lists
list <State*> open;             // open list: contains unexplored nodes
list <State*> close;            // close list: containse explored nodes
list <State*> finalPath;        // containse the sequence of moves to solve the problem

// F U N C T I O N S

// displays a list: for testing/debugging
void displayLIst(list <State*> l) {
    while (!l.empty()) {
        State *ptr = l.front();
        l.pop_front();
        // ptr->displayBoard();
        ptr->displayLine();
        cout << "F-Cost = " << ptr->fCost << endl;
    }
}

// checks for possible duplicate states
bool check_closedList(char tileVal[][numCols]) {
    list <State*> temp(close);
    while (!temp.empty()) {
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                if (tileVal[i][j] != temp.front()->tileValue[i][j]) {
                    goto nextIndex;
                }
            }
        }
        return true;    
        nextIndex:
        temp.pop_front();
    }
    return false;
}
// creates new states based on the coordinate of the blank tile/0
void createState(State *currentState, int x_value, int y_value) {
    int x = currentState->coordinate_zero_x; // 1 
    int y = currentState->coordinate_zero_y; // 2
    
    char temp_TileValue[numRows][numCols];
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            temp_TileValue[i][j] = currentState->tileValue[i][j];
        }
    }

    temp_TileValue[x][y] = temp_TileValue[x_value][y_value];
    temp_TileValue[x_value][y_value] = '0'; // 0 2
    
    if (check_closedList(temp_TileValue) == false) {
        open.push_front(new State(temp_TileValue, currentState));
    }
}
// creates new state: determines coordinate of blank tile and possible moves
void create_successorState(State *currentState) {
    // coordinate x and y
    int x = currentState->coordinate_zero_x; // 1 
    int y = currentState->coordinate_zero_y; // 2
    // move zero down
    if ((x - 1) >= 0) {
        createState(currentState, x-1, y);
    }
    // move zero up
    if ((x + 1) <= 2) {
        createState(currentState, x+1, y);
    }
    // move zero left
    if ((y - 1) >= 0) {
        createState(currentState, x, y-1);
    }
    // move zero right
    if ((y + 1) <= 2) {
        createState(currentState, x, y+1);
    }
}

// M A I N
int main() {

    // pointers
    State *initState = NULL;        // to keep track of the initial state
    State *currentState = NULL;     // to keep track of current state

    // TEST CASES
    // char init_TileValue[numRows][numCols] =  {{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '0'}};    // easy
    // char init_TileValue[numRows][numCols] =  {{'1', '3', '4'}, {'8', '6', '2'}, {'7', '0', '5'}};    // easy
    // char init_TileValue[numRows][numCols] =  {{'2', '8', '1'}, {'0', '4', '3'}, {'7', '6', '5'}};    // medium
    // char init_TileValue[numRows][numCols] =  {{'2', '8', '1'}, {'4', '6', '3'}, {'7', '5', '0'}};    // hard
    // char init_TileValue[numRows][numCols] =  {{'5', '6', '7'}, {'4', '0', '8'}, {'3', '2', '1'}};    // worst // not solved using hamming
    
    // PRESET HEURISTIC TO BE USED
    // heuristicUsed = 'h';

    // get initial tile value from user
    char init_TileValue[numRows][numCols];
    start:
    reDo:
    cout << "Please input initial state...\n\n";
    for (int i = 0; i < numRows; i++) {
        getRow: // get row
        cout << "Row " << i+1 << ": ";
        for (int j = 0; j < numCols; j++) {
            cin >> init_TileValue[i][j];
        }

        // check for duplicate-valued tiles
        if (i != 0) {
            for (int k = 0; k < 3; k++) {
                for (int l = 0; l < numCols; l++) {
                    if (init_TileValue[i-1][k] == init_TileValue[i][l]) {
                        cout << "ERROR: Duplicate tiles...\n";
                        goto getRow;
                    }
                }
            } 
        }

    }

    cout << "\nStart State:\n";
    cout << "+---+---+---+\n";
    for (int i = 0; i < numRows; i++) {
        cout << "| ";
        for (int j = 0; j < numCols; j++) {
            if (init_TileValue[i][j] == '0') cout << " " << " | ";
            else cout << init_TileValue[i][j] << " | ";
        }
        cout << "\n+---+---+---+\n";
    }
    cout << endl;

    choose_h:
    getH:// get heuristic to be used
    cout << "To use Hamming distance, press 1...\n";
    cout << "To use Manhattan distance, press 2...\n";
    cin >> heuristicUsed;
    if (heuristicUsed == '1') heuristicUsed = 'h';
    else if (heuristicUsed == '2') heuristicUsed = 'm';
    else {
        cout << "ERROR: Please choose either 1 or 2 only...\n";
        goto getH;
    }

    // initialize initState/startState pointer
    initState = new State (init_TileValue, NULL);
    cout << "\nStart State:\n";
    initState->displayBoard();
    cout << "Initial F-Cost: " << initState->fCost << endl;
    cout << endl;

    // a-star implementation from pseudocode   
    open.push_front(initState);
    while (!open.empty()) {
        
        open.sort([](const State* f, const State* s) {return f->fCost < s->fCost;});    // sort open list: used lambda expr to sort by objecty property
        currentState = open.front();                                                    // get lowest fcost in open queue
        open.pop_front();                                                               // remove lowest fcost from open
        close.push_front(currentState);                                                 // place lowest fcost to close
        
        // check if state is goalState
        if ((currentState->getHamming() == 0) || (currentState->getManhattan() == 0)) {
            
            int count = 0;  // path count sequence

            // insert sequence to final path list
            while (currentState->parent != NULL) {
                finalPath.push_front(currentState);
                currentState = currentState->parent;
            }

            // print initial state
            cout << "S.)\n";
            initState->displayBoard();
            cout << "Initial F-Cost: " << initState->fCost << endl;
            cout << "==================\n";
            // print final path/sequence
            while (!finalPath.empty()) {
                count += 1;
                cout << "      |      \n";
                cout << "      V      \n";
                cout << count << ".)\n";
                finalPath.front()->displayBoard();
                finalPath.pop_front();
            }
            
            cout << "     END     \n";
            cout << "=============\n";
            cout << "open list: " << open.size() << endl;
            cout << "close list: " << close.size() << endl;

            // reset/clear data
            open.clear();
            close.clear();
            initState = NULL;
            currentState = NULL;

            // try again/ change heuristic/ exit loop : Success
            int try_again;
            tryAgain:
            cout << "Try a different board, press 1...\n";
            cout << "Try again with other heuristic, press 2...\n";
            cout << "End program, press 3...\n";
            cin >> try_again;
            if (try_again == 1) goto start;
            else if (try_again == 2)goto choose_h;
            else if (try_again == 3) return 0;
            else {
                cout << "ERROR: Please choose from 1, 2 or 3 only...\n";
                goto tryAgain;
            }
        }
        // if not: create successor or childStates and;
        //         place all of them to open list to be sorted
        else create_successorState(currentState);
    }
    // Failure to solve
    cout << "Open is empty...\n";
    return 0;
}