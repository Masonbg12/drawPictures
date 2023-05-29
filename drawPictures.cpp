// DRAWING PICTURES FROM A SET OF COMMANDS IN FILES USING ASTERICKS
// Mason Gillespie

// preprocessor directives
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// structure holds the character which will be printed in the picture output file and pointers to represent
// the cardinal directions which link the 50X50 "grid" of nodes
struct node
{
    string character;
    node* north;
    node* south;
    node* east;
    node* west;
};

// function prototypes
void createGrid(node*&);
bool inputValidation(string, int&, char&, int&, bool&, bool&);
bool reduceString(string&);
void drawLine(node*&, int, char, int, bool);
bool inBounds(node*, int, char, int);
void printGrid(node*&, fstream&, bool);
void deleteGrid(node*&);


int main()
{
    fstream picture;                // input and output file that holds painted picture
    fstream commands;               // input file full of commands
    string  nextCommand;            // line of commands read from input
    char    direction;              // input direction value
    int     status,                 // input status value
            distance;               // input distance value
    bool    bold = false,           // input bold value
            print = false;          // input print value
    node*   head = nullptr;         // pointer to the first node in the first row and first column
    node*   currentNode = nullptr;  // currentNode where pen is at during drawing

    // opening picture fstream and validating it was open properly
    picture.open("picture.txt", ios :: out | ios :: trunc);
    if (picture.fail())
        cout << "picture did not open" << endl;
    else
    {
        // opening commands and validating it was open properly
        commands.open("commands.txt", ios :: in);
        if (commands.fail())
            cout << "commands.txt was not found" << endl;
        else
        {
            // createGrid is called
            createGrid(head);

            currentNode = head;

            // while loop iterates until there are no more commands
            while (!commands.eof())
            {
                // reading line of command
                getline(commands, nextCommand);

                // calling inputValidation
                if (inputValidation(nextCommand, status, direction, distance, bold, print))
                {
                    // drawLine is called
                    drawLine(currentNode, status, direction, distance, bold);

                    // printGrid is called
                    printGrid(head, picture, print);
                }
            }
        }
    }

    // calling deleteGrid
    deleteGrid(head);

    // setting temporary pointers to nullptr
    head = nullptr;
    currentNode = nullptr;

    // closing files
    commands.close();
    picture.close();

    return 0;
}// end of main function

// createGrid uses struct pointers to construct a two dimensional linked list. The function
// moves though the "grid" and connects the pointers east to west and connects north to south
// after setting temporary pointers. All of the nodes on the edge of the "grid" all have nullptrs pointing to that
// edge to indicate it exists
void createGrid(node*& head)
{
    head = new node;                // creating the first node
    node* northNode = nullptr;      // northNode points to the node above the currentNode
    node* previousRow = head;       // previousRow points to the first node in the row before where currentNodes are being created
    node* previousNode = nullptr;   // previousNode points to the node west of the currentNode
    node* currentNode = head;       // currentNode points to the currentNode being created

    // creating the head node
    head->north = nullptr;
    head->west = nullptr;
    head->character = " ";

    // for loop iterates 49 times for the 49 remaining nodes to be created in the first row
    for (int currNum = 0; currNum < 49; currNum++)
    {
        previousNode = currentNode;         // previousNode points to currentNode
        currentNode = new node;             // a new node is created
        currentNode->character = " ";       // the character is set to a space
        currentNode->west = previousNode;   // west pointer is connected
        previousNode->east = currentNode;   // east pointer is connected
        currentNode->north = nullptr;       // north is always nullptr for the first row
    }

    // when the loop is done currentNode is the last node in the the row so the east pointer is set
    currentNode->east = nullptr;

    // for loop iterates for the 49 remaining rows to be created
    for (int row = 0; row < 49; row++)
    {
        // currentNode is set to nullptr so a connection is not made after each row
        currentNode = nullptr;
        // northNode is set the first node in previous row
        northNode = previousRow;

        // for loop iterates for the 50 nodes to be created in the column
        for (int col = 0; col < 50; col++)
        {
            // setting previousNode to currentNode, creating new node, and setting its character to space
            previousNode = currentNode;
            currentNode = new node;
            currentNode->character = " ";

            // if the loop is on the left hand side then the west pointer is set to nullptr
            // else there is a node to the west of the currentNode and the west and east connections are made
            if (col == 0)
                currentNode->west = nullptr;
            else
            {
                currentNode->west = previousNode;
                previousNode->east = currentNode;
            }

            // there is always a north node because the first row is already created
            currentNode->north = northNode;

            // last row of south pointers are set to nullptr
            if (row == 49)
                currentNode->south = nullptr;

            // south connections are made from northNode
            northNode->south = currentNode;

            // northNode is shifted to the right for next iteration
            northNode = northNode->east;
        }
        // previousRow is shifted down for next iteration
        previousRow = previousRow->south;

        // after col for loop the currentNode is on the last row so the east is set to nullptr
        currentNode->east = nullptr;
    }

}// end of createGrid

// inputValidation will verify that the command from the input file
// follows these guidelines: <status>, <direction>, <distance>, <bold - optional>, <print - optional>
bool inputValidation(string command, int& status, char& direction, int& distance, bool& bold, bool& print)
{
    string stringTemp = "";  // stringTemp is used to hold the substring for bold and print
    int    comma;            // comma stores the index of the comma after the distance
    char   charTemp;         // charTemp is used to check if status and distance is a digit
    bold = false;
    print = false;

    // checking if command length is greater than 0
    if (command.length() > 0)
    {
        // storing first character of command in charTemp to test if it is a digit
        // if true, convert to integer and store in status
        charTemp = command.at(0);

        if (isdigit(charTemp))
            status = stoi(command.substr(0, 1));
    }
    else
        return false;

    // testing if status is valid
    if (status != 1 && status != 2)
        return false;

    // calling reduceString
    if (!reduceString(command))
        return false;
    else
    {
        // validating
        direction = command.at(0);

        if (direction != 'N' && direction != 'S' && direction != 'W' && direction != 'E')
            return false;

        // calling reduceString
        if (!reduceString(command))
            return false;
        else
        {
            // testing if there is no comma after distance
            if(command.find(",") == string :: npos)
            {
                // for loop checks that each character of distance is a digit since distance can
                // be many digits (max of 2 for it to also be valid)
                for (unsigned int currentDigit = 0; currentDigit < command.length(); currentDigit++)
                {
                    charTemp = command.at(currentDigit);
                    if (isdigit(charTemp))
                        continue;
                    else
                        return false;
                }

                distance = stoi(command.substr(0));

                // command is given a length of zero for testing later in the function
                command = "";
            }
            // else, if there is a comma
            else
            {
                // storing the index of the comma in comma
                comma = command.find(",");

                // for loop checks that each character of distance is a digit since distance can
                // be many digits (max of 2 for it to also be valid)
                for (int currentDigit = 0; currentDigit < comma; currentDigit++)
                {
                    charTemp = command.at(currentDigit);
                    if (isdigit(charTemp))
                        continue;
                    else
                        return false;
                }

                // converting distance to an integer
                distance = stoi(command.substr(0, comma));
                // removing distance and comma from command line
                command = command.substr(comma + 1);

                // storing bold character stringTemp then validating that bold can be printed
                stringTemp = command.substr(0, 1);

                // testing if there is a print or bold
                if (stringTemp == "P")
                    print = true;

                else if (stringTemp == "B" && status == 1)
                    return false;

                else if (stringTemp == "B" && status == 2)
                {
                    bold = true;

                    // calling reduceString
                    if (reduceString(command))
                    {
                        // storing last character in stringTemp and checking if print is true
                        stringTemp = command.substr(0, 1);
                        if (stringTemp == "P")
                            print = true;
                    }
                }
            }

            // checking if distance is a positive integer
            if (distance <= 0)
                return false;

            // testing if there are any leftover characters in command
            if (command.length() > 1)
                return false;
        }
    }
    return true;
}// end of inputValidation

// reduceString uses substring to modify command by creating a new string after
// the next instance of a comma
// return value is used to test if there is another input value
bool reduceString(string& command)
{
    if (command.length() >= 3 && command.substr(1, 1) == ",")
    {
        command = command.substr(2);
        return true;
    }
    else
        return false;
}// end of reduceString

// drawLine calls inBounds() to check that the command's movements are within bounds and traverses through the linked list
// to draw the line of character or move the cursor in the indicated position
void drawLine(node*& currentNode, int status, char direction, int distance, bool bold)
{
    string printingCharacter = "*";   // character being printed based on if bold is true/false

    // calling inBounds() to make sure the proposed command is within bounds of the "grid"
    if (inBounds(currentNode, status, direction, distance))
    {
        // pen is simply being moved if the status is 1
        if (status == 1)
        {
            // moving the pen follows the same logic in all directions which involves moving in the indicated direction
            // by accessing the respective pointer for distance amount of times
            switch(direction){
            case 'N':   // north direction
                for (int currNode = 0; currNode < distance; currNode++)
                    currentNode = currentNode->north;
                break;
            case 'E':   //east direction
                for (int currNode = 0; currNode < distance; currNode++)
                    currentNode = currentNode->east;
                break;
            case 'S':   // south direction
                for (int currNode = 0; currNode < distance; currNode++)
                    currentNode = currentNode->south;
                break;
            case 'W':   // west direction
                for (int currNode = 0; currNode < distance; currNode++)
                    currentNode = currentNode->west;
                break;
            }
        }
        else //else, status == 2
        {
            // if bold is true then pounds are going to be printed
            if (bold)
                printingCharacter = "#";

            // switch statement changes the nodes characters to represent the newly drawn line.
            // all directions follow the same logic by first moving to account for not printing in the beginning position
            // and then checking that an asterisk is not printed over a pound before changing the character
            switch(direction){
            case 'N':   // north direction
                for (int currNode = 0; currNode < distance; currNode++)
                {
                    currentNode = currentNode->north;

                    if (currentNode->character == "#" && printingCharacter == "*")
                        continue;
                    else
                        currentNode->character = printingCharacter;
                }
                break;
            case 'E':   //east direction
                for (int currNode = 0; currNode < distance; currNode++)
                {
                    currentNode = currentNode->east;

                    if (currentNode->character == "#" && printingCharacter == "*")
                        continue;
                    else
                        currentNode->character = printingCharacter;
                }
                break;
            case 'S':   // south direction
                for (int currNode = 0; currNode < distance; currNode++)
                {
                    currentNode = currentNode->south;

                    if (currentNode->character == "#" && printingCharacter == "*")
                        continue;
                    else
                        currentNode->character = printingCharacter;
                }
                break;
            case 'W':   // west direction
                for (int currNode = 0; currNode < distance; currNode++)
                {
                    currentNode = currentNode->west;

                    if (currentNode->character == "#" && printingCharacter == "*")
                        continue;
                    else
                        currentNode->character = printingCharacter;
                }
                break;
            }
        }
    }
}// end of drawLine

// inBounds checks that the proposed pen movement or line being drawn does not exceed the borders of the 50X50 linked list
bool inBounds(node* currentNode, int status, char direction, int distance)
{
    // switching the direction options.
    // each direction has the same logic which involves moving through the linked list for distance amount of times
    // and if the currentNode pointer reaches a nullptr (the edge of the "grid") then the command moves outside of bounds
    // and is therefore false
    switch(direction){
    case 'N':   // north direction
        for (int pos = 0; pos < distance; pos++)
        {
            currentNode = currentNode->north;

            if (currentNode == nullptr)
                return false;
        }
        break;
    case 'E':   //east direction
        for (int pos = 0; pos < distance; pos++)
        {
            currentNode = currentNode->east;

            if (currentNode == nullptr)
                return false;
        }
        break;
    case 'S':   //south direction
        for (int pos = 0; pos < distance; pos++)
        {
            currentNode = currentNode->south;

            if (currentNode == nullptr)
                return false;
        }
        break;
    case 'W':   //west direction
        for (int pos = 0; pos < distance; pos++)
        {
            currentNode = currentNode->west;

            if (currentNode == nullptr)
                return false;
        }
        break;
    }

    return true;
}// end of inBounds

// printGrid prints the grid to the output file and to the console of the boolean variable print
// is true
void printGrid(node*& head, fstream& picture, bool print)
{
    node* currNode = head;  // currNode points to the currentNode being printed
    node* currRow = head;   // currRow points to the first node in the row being printed

    // assuring that the printing is done from the beginning of the file
    picture.seekp(0L, ios :: beg);

    for (int row = 0; row < 50; row++)
    {
        for (int col = 0; col < 50; col++)
        {
            // printing the character to the file and moving the pointer east
            picture << currNode->character;
            currNode = currNode->east;
        }
        // one row is done so the file pointer is moved to the next line
        picture << endl;

        // conditional statement prevents the nested for loops nextRow from trying to access a non-existing node
        // in the update statements below
        if (currRow == nullptr)
            break;

        // currNode is set the first node below the row that was just printed and currRow is moved to
        // the same position
        currNode = currRow->south;
        currRow = currRow->south;
    }

    // if print is true then the same logic above is repeated, except the characters are printed to the console
    // and two line buffers are printed
    if (print)
    {
        currNode = head;
        currRow = head;

        for (int row = 0; row < 50; row++)
        {
            for (int col = 0; col < 50; col++)
            {
                cout << currNode->character;
                currNode = currNode->east;
            }
            cout << endl;

            if (currRow == nullptr)
                break;
            currNode = currRow->south;
            currRow = currRow->south;
        }

        cout << endl << endl;
    }

}// end of printGrid

// deleteGrid will run through each node and will delete the dynamic memory allocated for the
// current node being deleted
void deleteGrid(node*& head)
{
    node* currNode = head;          // currNode is the current node being deleted
    node* nextNode = nullptr;       // nextNode points to the next node in the list
    node* nextRow = head->south;    // nextRow points to the first node in the row after the current one being deleted

    // nested for loop iterates through entire linked list
    for (int row = 0; row < 50; row++)
    {
        for (int col = 0; col < 50; col++)
        {
            // nextNode is set to the next node so the connection the list is not lost.
            // currNode is deleted and then set to the next node
            nextNode = currNode->east;
            delete currNode;
            currNode = nextNode;
        }

        // conditional statement prevents the nested for loops nextRow from trying to access a non-existing node
        // in the update statements below
        if (nextRow == nullptr)
            break;

        // after an entire column is deleted currNode is set the first node in the next row and
        // nextRow is moved down one
        currNode = nextRow;
        nextRow = nextRow->south;
    }
}// end of deleteGrid
