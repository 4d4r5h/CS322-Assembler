// /*****************************************************************************
// TITLE: assembler.cpp
// AUTHOR: Adarsh Kumar (2001CS02)
// Declaration of Authorship
// This file is part of the assignment of CS321 at the
// department of Computer Science and Engineering, IIT Patna.
// *****************************************************************************/

#include <bits/stdc++.h>
using namespace std;

map<string, int> label_address; // mappes each label to its address
vector<int> line_number;        // stores line number for all the valid lines
vector<string> lines;           // stores all the valid lines
vector<string> errors;          // stores all the errors
vector<string> warnings;        // stores all the warnings
vector<string> mnemonics;       // stores all the mnemonics in order of their increasing opcode
map<int, int> memory;           // mappes program counter to data memory
set<int> error_lines;           // has all the line numbers containing any type of error in the first pass
vector<string> listing_file;
vector<string> object_file;

bool compare(string &s1, string &s2)
{
    string num1, num2;

    int i = s1.length() - 1;
    while (s1[i] >= '0' && s1[i] != ' ')
    {
        num1.push_back(s1[i]);
        i--;
    }
    reverse(num1.begin(), num1.end());

    i = s2.length() - 1;
    while (s2[i] >= '0' && s2[i] != ' ')
    {
        num2.push_back(s2[i]);
        i--;
    }
    reverse(num2.begin(), num2.end());

    return stoi(num1) < stoi(num2);
}

int get_opcode(string &label)
{
    for (int i = 0; i < mnemonics.size(); i++)
    {
        if (mnemonics[i] == label)
            return i;
    }
    return -1;
}

bool is_valid_label(string label)
{
    label.pop_back();
    for (int i = 0; i < label.size(); i++)
        label[i] = tolower(label[i]);
    if (!(label[0] >= 'a' && label[0] <= 'z'))
        return false;
    for (int i = 1; i < label.size(); i++)
    {
        if (!((label[i] >= 'a' && label[i] <= 'z') || (label[i] >= '0' && label[i] <= '9') || label[i]=='_'))
            return false;
    }
    return true;
}

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

bool is_decimal(string &operand)
{
    if (operand == "0")
        return true;
    if (!(operand[0] == '-' || operand[0] == '+' || (operand[0] >= '1' && operand[0] <= '9')))
        return false;
    for (int i = 1; i < operand.size(); i++)
    {
        if (!(operand[i] >= '0' && operand[i] <= '9'))
            return false;
    }
    return true;
}

bool is_octal(string &operand)
{
    if (operand[0] != '0')
        return false;
    for (int i = 1; i < operand.size(); i++)
    {
        if (operand[i] < '0' || operand[i] > '7')
            return false;
    }
    return true;
}

bool is_hex(string &operand)
{
    if ((int)operand.size() <= 2 || operand[0] != '0' || operand[1] != 'x')
        return false;
    for (int i = 2; i < operand.size(); i++)
    {
        if ((operand[i] >= 'A' && operand[i] <= 'F') || (operand[i] >= 'a' && operand[i] <= 'f') || (operand[i] >= '0' && operand[i] <= '9'))
            continue;
        else
            return false;
    }
    return true;
}

pair<long long, bool> read_operand(string &operand)
{
    int base;
    if (is_decimal(operand))
        base = 10;
    else if (is_hex(operand))
        base = 16;
    else if (is_octal(operand))
        base = 8;
    else
        return {-1, false};
    long long result = stoll(operand, nullptr, base);
    return {result, true};
}

void map_label_address()
{
    int program_counter = 0;
    for (int i = 0; i < lines.size(); i++)
    {
        int count_colon = 0, initial_size = errors.size();
        for (char ch : lines[i])
        {
            if (ch == ':')
                count_colon += 1;
        }
        if (count_colon == 0)
        {
            program_counter++;
        }
        else if (count_colon == 1)
        {
            string label;
            stringstream obj(lines[i]);
            obj >> label;
            if (label.length() > 1 && label.back() == ':' && is_valid_label(label))
            {
                label.pop_back();

                if (label_address.find(label) != label_address.end())
                {
                    string error = "Duplicate label found at line ";
                    error += to_string(line_number[i]);
                    errors.push_back(error);
                }
                else
                {
                    if (get_opcode(label) != -1)
                    {
                        string error = "Label name is a keyword at line ";
                        error += to_string(line_number[i]);
                        errors.push_back(error);
                    }
                    else
                    {
                        label_address[label] = program_counter;

                        string mnemonic;
                        if (obj >> mnemonic)
                        {
                            string operand1;
                            obj >> operand1;
                            if (mnemonic == "SET")
                            {
                                string operand2;
                                if (operand1.empty())
                                {
                                    string error = "Operand not found at line ";
                                    error += to_string(line_number[i]);
                                    errors.push_back(error);
                                }
                                else if (read_operand(operand1).second == false)
                                {
                                    string error = "Invalid operand format at line ";
                                    error += to_string(line_number[i]);
                                    errors.push_back(error);
                                }
                                else if (obj >> operand2)
                                {
                                    string error = "More operands than expected at line ";
                                    error += to_string(line_number[i]);
                                    errors.push_back(error);
                                }
                                else
                                {
                                    label_address[label] = read_operand(operand1).first;
                                }
                            }
                            else if (mnemonic == "data")
                            {
                                string operand2;
                                if (operand1.empty())
                                {
                                    string error = "Operand not found at line ";
                                    error += to_string(line_number[i]);
                                    errors.push_back(error);
                                }
                                else if (read_operand(operand1).second == false)
                                {
                                    string error = "Invalid operand format at line ";
                                    error += to_string(line_number[i]);
                                    errors.push_back(error);
                                }
                                else if (obj >> operand2)
                                {
                                    string error = "More operands than expected at line ";
                                    error += to_string(line_number[i]);
                                    errors.push_back(error);
                                }
                                else
                                {
                                    memory[program_counter] = read_operand(operand1).first;
                                }
                            }
                            program_counter++;
                        }
                    }
                }
            }
            else
            {
                string error = "Invalid label format at line ";
                error += to_string(line_number[i]);
                errors.push_back(error);
            }
        }
        else
        {
            string error = "More than one colon detected at line ";
            error += to_string(line_number[i]);
            errors.push_back(error);
        }
        if (errors.size() > initial_size)
        {
            error_lines.insert(line_number[i]);
        }
    }
}

void remove_comments(string &line)
{
    string s;
    for (int i = 0; i < line.length(); i++)
    {
        if (line[i] == ';')
            break;
        s.push_back(line[i]);
        if (line[i] == ':')
            s.push_back(' ');
    }
    line = s;
}

void remove_spaces(string &line)
{
    string temp, s = "";
    stringstream obj(line);
    while (obj >> temp)
    {
        s += temp + " ";
    }
    if (!s.empty())
    {
        s.pop_back();
    }
    line = s;
}

void initialise()
{
    mnemonics = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"};
}

int main(int argc, char *argv[])
{
    initialise();

    if (argc < 2)
    {
        cout << "Error: Input valid file name" << endl;
        return 0;
    }

    string _file = argv[1];
    fstream f_ptr(_file, ios::in);

    if (!f_ptr.is_open())
    {
        cout << "Error: File not found" << endl;
        return 0;
    }

    string line;
    int current_line = 1;

    while (getline(f_ptr, line))
    {
        remove_comments(line);
        remove_spaces(line);
        if (line.length() > 0)
        {
            line_number.push_back(current_line);
            lines.push_back(line);
        }
        current_line++;
    }

    map_label_address();

    // Pass 1 completed

    int program_counter = 0;

    for (int i = 0; i < lines.size(); i++)
    {
        vector<string> line;
        stringstream obj(lines[i]);
        string temp;
        while (obj >> temp)
        {
            line.push_back(temp);
        }
        if (line[0].back() == ':')
        {
            if (line.size() == 1)
            {
                continue;
            }

            if (line[1] == "SET" || line[1] == "data")
            {
                if (error_lines.find(i) == error_lines.end())
                {
                    // line is free from error
                    auto operand = read_operand(line[2]);
                    if (operand.first < INT_MIN || operand.first > INT_MAX)
                    {
                        string warning = "Integer overflow at line ";
                        warning += to_string(line_number[i]);
                        warnings.push_back(warning);
                    }
                    string code = to_hex(program_counter, 4) + " " + to_hex(operand.first, 8);
                    listing_file.push_back(code + " " + lines[i]);
                    if(line[1]!="SET")
                        object_file.push_back(code);
                }
                program_counter++;
                continue;
            }
            for (int j = 1; j < line.size(); j++)
            {
                line[j - 1] = line[j];
            }
            line.pop_back();
        }

        if (line.size() > 2)
        {
            string error = "More operands than expected at line ";
            error += to_string(line_number[i]);
            errors.push_back(error);
        }
        else if (get_opcode(line[0]) == -1 && line[0] != "data")
        {
            string error = "Invalid mnemonic found at line ";
            error += to_string(line_number[i]);
            errors.push_back(error);
        }
        else if (line[0] == "brz" || line[0] == "brlz" || line[0] == "br" || line[0] == "call" || line[0] == "data")
        {
            if (line.size() < 2)
            {
                string error = "Operand not found at line ";
                error += to_string(line_number[i]);
                errors.push_back(error);
            }
            else if (line[0] == "data")
            {
                auto operand = read_operand(line[1]);
                if (operand.second == false)
                {
                    string error = "Invalid operand format at line ";
                    error += to_string(line_number[i]);
                    errors.push_back(error);
                }
                else
                {
                    if (operand.first < INT_MIN || operand.first > INT_MAX)
                    {
                        string warning = "Integer overflow at line ";
                        warning += to_string(line_number[i]);
                        warnings.push_back(warning);
                    }
                    memory[program_counter] = operand.first;
                    string code = to_hex(program_counter, 4) + " " + to_hex(operand.first, 8);
                    listing_file.push_back(code + " " + lines[i]);
                    object_file.push_back(code);
                    program_counter++;
                }
            }
            else if (label_address.find(line[1]) == label_address.end())
            {
                string error = "Label not found at line ";
                error += to_string(line_number[i]);
                errors.push_back(error);
            }
            else
            {
                int temp = -program_counter - 1;
                if (line[0] == "call")
                {
                    temp = 0;
                }
                string code = to_hex(program_counter, 4) + " " + to_hex(label_address[line[1]] + temp, 6) + to_hex(get_opcode(line[0]), 2);
                listing_file.push_back(code + " " + lines[i]);
                object_file.push_back(code);
                program_counter++;
            }
        }
        else if (line[0] == "add" || line[0] == "sub" || line[0] == "shl" || line[0] == "shr" || line[0] == "a2sp" || line[0] == "sp2a" || line[0] == "return" || line[0] == "HALT")
        {
            if (line.size() > 1)
            {
                string error = "Extra operand found at line ";
                error += to_string(line_number[i]);
                errors.push_back(error);
            }
            else
            {
                string code = to_hex(program_counter, 4) + " " + to_hex(get_opcode(line[0]), 8);
                listing_file.push_back(code + " " + lines[i]);
                object_file.push_back(code);
                program_counter++;
            }
        }
        else
        {
            if (line.size() < 2)
            {
                string error = "Operand not found at line ";
                error += to_string(line_number[i]);
                errors.push_back(error);
            }
            else if (tolower(line[1][0]) >= 'a' && tolower(line[1][0]) <= 'z')
            {
                if (label_address.find(line[1]) == label_address.end())
                {
                    string error = "Label not found at line ";
                    error += to_string(line_number[i]);
                    errors.push_back(error);
                }
                else
                {
                    string code = to_hex(program_counter, 4) + " " + to_hex(label_address[line[1]], 6) + to_hex(get_opcode(line[0]), 2);
                    listing_file.push_back(code + " " + lines[i]);
                    object_file.push_back(code);
                    program_counter++;
                }
            }
            else
            {
                auto operand = read_operand(line[1]);
                if (operand.second == false)
                {
                    string error = "Invalid operand format at line ";
                    error += to_string(line_number[i]);
                    errors.push_back(error);
                }
                else
                {
                    string code = to_hex(program_counter, 4) + " " + to_hex(operand.first, 6) + to_hex(get_opcode(line[0]), 2);
                    listing_file.push_back(code + " " + lines[i]);
                    object_file.push_back(code);
                    program_counter++;
                }
            }
        }
    }

    f_ptr.close();

    sort(errors.begin(), errors.end(), compare);

    string file_name;

    for (int i = 0; i < _file.length(); i++)
    {
        if (_file[i] == '.')
            break;
        file_name.push_back(_file[i]);
    }

    fstream log_file(file_name + ".log", ios::out);

    log_file << "ERRORS:\n";
    for (string error : errors)
    {
        log_file << error << "\n";
    }
    log_file << "WARNINGS:\n";
    for (string warning : warnings)
    {
        log_file << warning << "\n";
    }

    log_file.close();

    if (errors.empty() == false)
    {
        return 0;
    }

    fstream list_file(file_name + ".l", ios::out);

    for (string _list : listing_file)
    {
        list_file << _list << "\n";
    }

    list_file.close();

    fstream obj_file(file_name + ".o", ios::out | ios::binary);

    for (auto object : object_file)
    {
        stringstream obj(object);
        string s1, s2;
        obj >> s1 >> s2;
        int num = 0;
        reverse(s2.begin(), s2.end());
        for (int i = 0; i < 8; i++)
        {
            int val = 0;
            if (s2[i] >= 'a' and s2[i] <= 'f')
            {
                val = (s2[i] - 'a' + 10);
            }
            else
            {
                val = s2[i] - '0';
            }
            num += (val * (int)pow(16, i));
        }
        static_cast<int>(num);
        obj_file.write((const char *)&num, sizeof(int));
    }

    obj_file.close();

    return 0;
}