#ifndef LOGFILE_H
#define LOGFILE_H

#include <fstream>
#include <sstream>
#include <unistd.h>

// ================================= FOR USE ====================================================
#define LOG(data)   logFIle(data);
#define LOG_NEWLINE logFIle("\n");

// FOR EDIT
#define FILENAME    "log_Alfarobi"
//------------------------------------------------------------------------------------------------

#define to_string(x) static_cast<std::ostringstream*>( &(std::ostringstream() << x) )->str()

static std::fstream log_file;
static bool logging = false;

void checkFile()
{
    static int a;
    static std::string file_name;

    if(!logging)
    {
        a = 1;
        file_name = FILENAME;
        file_name += "_";
        file_name += to_string(a << ".txt");
        while(access(file_name.c_str(), F_OK) != -1)
        {
            a++;
            file_name = FILENAME;
            file_name += "_";
            file_name += to_string(a << ".txt");
        }
        logging = true;
        log_file.open(file_name.c_str(), std::ios_base::out | std::ios_base::app);
    }
}

void logFIle(const std::string &text)
{
    checkFile();
    log_file << text;
}

void logFIle(const int &value)
{
    checkFile();
    log_file << value << "\t";
}


#endif // LOGFILE_H
