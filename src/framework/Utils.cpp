/*
 * GL Framework
 * Copyright (c) David Avedissian 2014-2015
 */
#include "Common.h"
#include "Utils.h"

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
        stringstream err;
        err << "Error: Unable to open file '" << file << "'" << endl;
        throw std::runtime_error(err.str());
    }

    return fileData;
}

}
