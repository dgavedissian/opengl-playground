/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "common.h"
#include "utils.h"

namespace utils
{

string readFile(const string& file)
{
    string fileData;

    // Open the file
    ifstream fileStream(file, std::ios::in);
    if (fileStream.is_open())
    {
        string line;
        while (getline(fileStream, line))
            fileData += line + "\n";
        fileStream.close();
    }
    else
    {
        cout << "Error: Unable to open file '" << file << "'" << endl;
    }

    return fileData;
}

}
