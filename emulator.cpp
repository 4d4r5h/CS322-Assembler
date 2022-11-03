// /*****************************************************************************
// TITLE: emulator.cpp
// AUTHOR: Adarsh Kumar (2001CS02)
// Declaration of Authorship
// This file is part of the assignment of CS321 at the
// department of Computer Science and Engineering, IIT Patna.
// *****************************************************************************/

#include <bits/stdc++.h>
using namespace std;

int mainMemory[1 << 24];
int total = 0;
string file_name;
fstream _file;
int LIMIT = 100000;
int stackLimit = 1 << 23;
int PC = 0, SP = 0, regA = 0, regB = 0;
vector<int> objectFile;
vector<string> mnemonics;

string to_hex(int value, int len)
{
    stringstream obj;
    obj << hex << value;
    string result(obj.str());
    reverse(result.begin(), result.end());
    while ((int)result.size() > len)
    {
        result.pop_back();
    }
    while ((int)result.size() < len)
    {
        result.push_back('0');
    }
    reverse(result.begin(), result.end());
    return result;
}

void initialise()
{
    mnemonics = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"};
    _file.open(file_name + ".txt", ios::out);
}

void executeOpcode(int opcode, int operand)
{
    switch (opcode)
    {
    case 0:
        regB = regA;
        regA = operand;
        break;
    case 1:
        regA += operand;
        break;
    case 2:
        regB = regA;
        regA = mainMemory[SP + operand];
        break;
    case 3:
        mainMemory[SP + operand] = regA;
        regA = regB;
        break;
    case 4:
        regA = mainMemory[regA + operand];
        break;
    case 5:
        mainMemory[regA + operand] = regB;
        break;
    case 6:
        regA = regB + regA;
        break;
    case 7:
        regA = regB - regA;
        break;
    case 8:
        regA = regB << regA;
        break;
    case 9:
        regA = regB >> regA;
        break;
    case 10:
        SP = SP + operand;
        break;
    case 11:
        SP = regA;
        regA = regB;
        break;
    case 12:
        regB = regA;
        regA = SP;
        break;
    case 13:
        regB = regA;
        regA = PC;
        PC = operand - 1;
        break;
    case 14:
        PC = regA;
        regA = regB;
        break;
    case 15:
        if (regA == 0)
        {
            PC = PC + operand;
        }
        break;
    case 16:
        if (regA < 0)
        {
            PC = PC + operand;
        }
        break;
    case 17:
        PC = PC + operand;
        break;
    default:
        cout << "Invalid opcode. Incorrect machine code. Aborting.";
        exit(0);
    }
}

void SegmentationFault()
{
    if (PC >= (int)objectFile.size())
    {
        cout << "Segmentation fault. Aborting.";
        exit(0);
    }
}

void StackOverflow()
{
    if (SP > stackLimit)
    {
        cout << "Stack overflow. Aborting.";
        exit(0);
    }
}

void InfiniteLoop()
{
    if (total > LIMIT)
    {
        cout << "Infinite loop detected. Aborting.";
        exit(0);
    }
}

int argumentrun()
{
    SegmentationFault();

    int opcode = 0xFF & objectFile[PC]; // last 8 bits
    int operand = objectFile[PC] >> 8;  // first 24 bits

    if (opcode == 18) // HALT
    {
        total++;
        return 0;
    }

    cout << mnemonics[opcode] << "\t";
    printf("%08X\n", operand);

    executeOpcode(opcode, operand);

    printf("A = %08X, B = %08X, PC = %08X, SP = %08X\n", regA, regB, PC, SP);
    _file << "A = " << to_hex(regA, 8) << ", B = " << to_hex(regB, 8) << ", PC = " << to_hex(PC, 8) << ", SP = " << to_hex(SP, 8) << endl;

    total++;
    PC++;

    InfiniteLoop();

    StackOverflow();

    return 1;
}

pair<long long, bool> read_operand(string &operand)
{
    int len = (int)operand.size();

    if (len == 0)
    {
        return {0, 0};
    }

    char *str = (char *)malloc(len * sizeof(char));

    for (int i = 0; i < len; i++)
    {
        str[i] = operand[i];
    }
    for (int i = len; i < strlen(str); i++)
    {
        str[i] = '\0';
    }

    char *end;
    long long num;

    num = strtol(str, &end, 10); // conversion from decimal to decimal
    if (!*end)
    {
        return {num, 1};
    }

    num = strtol(str, &end, 16); // conversion from hexadecimal to decimal
    if (!*end)
    {
        return {num, 1};
    }

    return {-1, 0};
}

void dump()
{
    string operand, offset;

    cout << "Base address: ";
    cin >> operand;

    int baseAddress = read_operand(operand).first;

    cout << "No. of values: ";
    cin >> offset;

    auto value = read_operand(offset);

    int i = baseAddress;
    while (i < baseAddress + value.first && i < (1 << 24))
    {
        printf("%08X \t %08x\n", i, mainMemory[i]);
        i++;
    }
}

int advance()
{
    cout << "Emulator input: ";
    string temp;
    cin >> temp;

    for (int i = 0; i < temp.size(); i++)
        temp[i] = tolower(temp[i]);

    if (temp == "-t")
    {
        if (argumentrun() == 0)
            cout << "HALT instruction! No furthur tracing possible.\n";
    }
    else if (temp == "-all")
    {
        if (argumentrun() == 0)
            cout << "HALT instruction! No furthur tracing possible.\n";
        else
        {
            while (argumentrun() == 1)
            {
            }
        }
    }
    else if (temp == "-dump")
    {
        dump();
    }
    else if (temp == "-exit")
    {
        return 0;
    }
    else
    {
        cout << "Invalid emulator input." << endl;
    }

    return 1;
}

void help()
{
    printf("Trace current instruction only: -t\n");
    printf("Trace all instructions at once: -all\n");
    printf("See memory: -dump\n");
    printf("Exit: -exit\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Error: Input valid file name" << endl;
        return 0;
    }

    int temp;

    string __file = argv[1];

    ifstream file(__file, ios::in | ios::binary);
    while (file.read((char *)&temp, sizeof(int)))
    {
        objectFile.push_back(temp);
    }

    for (int i = 0; i < __file.length(); i++)
    {
        if (__file[i] == '.')
            break;
        file_name.push_back(__file[i]);
    }

    initialise();

    int i = 0;
    for (auto x : objectFile)
    {
        mainMemory[i] = x;
        i++;
    }

    help();

    while (true)
    {
        if (advance() == 1)
        {
            continue;
        }
        break;
    }

    _file.close();

    cout << "Total instructions executed: " << total << endl;
    return 0;
}
