
/*
 * How to build this on Windows 7 in PowerShell
 * 
 * 1) Install MinGW
 * 2) C:\MinGW\bin\x86_64-w64-mingw32-g++.exe -g -m64 -std=c++11 -o main.exe main.cpp
 * 3) ./main.exe
 *
 * How to build on Linux
 *
 * 1) Make sure gcc is installed
 * 2) gcc/v6.3.0/bin/g++ -g -m64 -Wall --std=c++11 -o main.exe main.cpp
 * 3) ./main.exe
 *
 */
#include <iostream>
#include <string>       // to give us the string type
//#include <string.h>
//#include <chrono>
#include <ctime>   
#include <algorithm>    // used to do lowercase of my strings
#include <sys/stat.h>   // let's you create a directory via mkdir()
#include <dirent.h>
#include <fstream>      // to open files and write/read them
#include <cstdlib>      // atoi()
#include "Task.h"

using namespace std;



/*
 * #define constants
 */

#define TASK_DIRNAME_PREFIX "task_"
#define TASK_NUMBER_OFFSET 5
/*
 * Status Values (these are values that can be returned by various function calls)
 */
#define TASK_S_SUCCESS 0
#define TASK_S_ERROR  -1

#define STATUS_RETURN_TYPE  int

/*
 * Classes
 */
typedef struct _configType {
    bool showIntro;             /* print introduction for the program */
    unsigned long taskNumber;
} ConfigType;

/*
 * Global variables (hopefully just one goes here)
 */

ConfigType g_config = {true, 0};   /* set default values */

/**
 * \brief Function that prints the introduction to this program.
 */
void printIntro(void) {
    cout << "Task Master V1.0" << endl;
    return;
}

// trim from end (in place)
static inline void rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](int ch) {
        return !isspace(ch);
    }).base(), s.end());
}

template <class Container>
void split3(const std::string& str, Container& cont,
              char delim = ' ')
{
    std::size_t current, previous = 0;
    current = str.find(delim);
    while (current != std::string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find(delim, previous);
    }
    cont.push_back(str.substr(previous, current - previous));
}


/**
 * \brief Open filename from folder and return the text
 */
string readContents( char *folder,  string filename ) {
    ifstream ifs;
    string text;
    

    ifs.open( ( string(folder)+"/"+filename), ifstream::in);
    char c = ifs.get();
    while(ifs.good()) {
        text += c;
        c = ifs.get();
    }
    ifs.close();

    return text;
}

/**
 *  \brief Function that reads in the configuration file settings.
 */
ConfigType *readconfig(void) {
    unsigned long largest_number=0;

    /*
     * See if config file is present; if so then read it.
     */

    /*
     * Look at all folders starting with "task_" to determine
     * the largest number. This largest number will be our 
     * starting number for any new tasks generated.
     *
     */

    DIR *dp = opendir(".");  // look in current folder
    struct dirent *dirp;

    if (dp) {
        while (( dirp = readdir(dp)) != NULL) {
            if (0 == strncmp(dirp->d_name, TASK_DIRNAME_PREFIX, TASK_NUMBER_OFFSET)) {
                // parse out the number from the filename  task_123
                //                                              ^
                //                                              |
                //                                         012345
                // we always know the number comes 5 bytes into the dirname (see above)
                //
                unsigned long number = atol((const char *)&dirp->d_name[strlen(TASK_DIRNAME_PREFIX)]);
                if ( number >= largest_number) {
                    largest_number = number;
                }

                // in each folder there should be text files with information we can read
                string subject = readContents( dirp->d_name , "subject.txt" );
                string description = readContents(dirp->d_name, "description.txt");
                string startDate = readContents(dirp->d_name, "start.txt" );
                cout << "Read in " << "number: " << number << " subj: " << subject << " desc: " << description << " date: " << startDate << endl;
                rtrim(startDate); 
                cout << "rtrim startDate: " << startDate << "." << endl;
                string dirname(dirp->d_name);
                (void) new Task(number, dirname, subject,description,(char *)startDate.c_str());

            }
        }
        closedir(dp);
    }

    g_config.taskNumber = largest_number;

    return &g_config;
}

/*
 * All the command functions are here
 */


/**
 * \brief Command that lists your tasks
 */
void cmd_list( ConfigType *config ) {

    for ( auto &itask : Task::list ) 
    {
        itask->display();
    }    

    return;
}

/**
 * \brief Function that will save a single task to disk
 */
string
savetask( ConfigType *config, unsigned long taskNumber, string subject, string description, char *startTime) {

    // open up new folder (directory) based on taskNumber

    string folderName = TASK_DIRNAME_PREFIX + to_string(taskNumber);
    mkdir( folderName.c_str());
    
    // save subject and dueDate into the folder

    ofstream myfile;
    myfile.open( folderName + "/subject.txt");
    myfile << subject ;
    myfile.close();

    myfile.open( folderName + "/description.txt");
    myfile << description ;
    myfile.close();

    myfile.open( folderName + "/start.txt");
    myfile << startTime;
    myfile.close();


    return folderName;
}

/**
 * \brief Command that adds a new task
 */
void cmd_add_task(ConfigType *config) {
    string subject;
    string description;

    cout << __func__ << endl;

//    cin.clear();
//    cin.ignore(255, '\n');

    cout << "Subject: > ";
    getline(cin, subject);

    cout << "Description: > ";
    getline(cin, description);

    // Get current date 
    time_t now = time(0);
    tm *gmtm = localtime(&now);

    // Generate a task number for this. Just make it +1 larger
    // than the largest number created so far.

    config->taskNumber++;
    string dirname = savetask( config, config->taskNumber, subject, description, asctime(gmtm));
    (void) new Task(config->taskNumber, dirname, subject, description, asctime(gmtm));

    return;
}

void removeDir(const char *path)
{
    struct dirent *entry = NULL;
    DIR *dir = NULL;
    dir = opendir(path);
    entry = readdir(dir);
    while (entry)
    {   
        DIR *sub_dir = NULL;
        FILE *file = NULL;
        char abs_path[100] = {0};
        if (*(entry->d_name) != '.')
        {   
            sprintf(abs_path, "%s/%s", path, entry->d_name);
            sub_dir = opendir(abs_path);
            if (sub_dir)
            {   
                closedir(sub_dir);
                removeDir(abs_path);
            }   
            else 
            {
                file = fopen(abs_path, "r");
                if (file)
                {
                    fclose(file);
                    remove(abs_path);
                }   
            }   
        }
        entry = readdir(dir);
    }
    remove(path);
    rmdir(path);
}

void cmd_activate_task(ConfigType *config) {
    cout << __func__ << endl;
}

void cmd_deactivate_task(ConfigType *config) {
    cout << __func__ << endl;
}

/**
 * \brief Command that deletes a task
 */
void cmd_delete_task(ConfigType *config, string command) {
    cout << __func__ << endl;

    rtrim(command);

    vector<string> parts;
    split3( command, parts, ' ');

    /*
     * See if we have value(s) supplied with the command string
     * eg.  del 33... 
     */    
    vector<unsigned long> numbers;
    if ( parts.size() < 2 ) 
    {
        cout << "Enter the task number to delete it> " ;
        string value;
        getline(cin, value);
        numbers.push_back( atol(value.c_str()));
    }
    else
    {
        bool isFirst=true;
        for ( auto &s : parts) {
            if ( ! isFirst ) {
                numbers.push_back( atol(s.c_str()));
            } else {
                isFirst = false;
            }
        }
        parts.clear();
    }
    

    for ( auto &itask : Task::list )
    {
        for (auto &inumber : numbers )
        {
            if ( itask->getTaskNumber() == inumber )
            {
                string dirPath = itask->getDirPath();
                // remove the directory
                removeDir(dirPath.c_str());
                // remove the task item 
                itask->remove();
            }
        }
    }

    numbers.clear();

}

/**
 * \brief Command that tells you all available commands
 */
void cmd_help(ConfigType *config) {
    cout << "Known Commands: list, add, (del or delete), help, (exit or quit)" << endl;
}

void coutInvalidCommand( string command ) {
  cout << "Sorry but I don't recognize the command \"" << command << "\"" << endl;
  cout << "Try typing \"help\" to get a list of known commands." << endl;
}

/*
 * The MAIN function goes here
 */

int main(int argc, char *argv[]) {
    auto config = readconfig();

    if ( config->showIntro) {
        printIntro();
    }

    // Look at the arguments passed into argv[]
    // TBD

    // Repeatedly Prompt User for another command until he/she
    // enters "exit" or "quit" 
    string command;
    bool userWantsToExit = false;
    do {
        cout << "Command> " ;
        getline(cin,command) ;
        if (command == "") {
            continue;
        }

        // force the command to all lowercase characters before comparing
        for_each( command.begin(), command.end(), [](char & c) { c = tolower(c); });
        switch ( command[0] ) {
            case 'l': // list
                if (command == "list") {
                    cmd_list( config );
                } else {
                    coutInvalidCommand(command);
                }
                break;
            case 'a': // add task
                if (command == "add") {
                    cmd_add_task(config);
                } else if ( command == "activate") {
                    cmd_activate_task(config);
                } else {
                    coutInvalidCommand(command);
                }
                break;
            case 'd': // delete task
                if (command.substr(0,6) == "delete" || command.substr(0,3) == "del" ) {
                    cmd_delete_task(config, command);
                } else if ( command == "deactivate" ) {
                    cmd_deactivate_task(config);
                } else {
                    coutInvalidCommand(command);
                }
                break;
            case 'h': // help
                if (command == "help") {
                    cmd_help(config);
                } else {
                    coutInvalidCommand(command);
                }
                break;
            case 'e': // exit
            case 'q': // quit
                if (command == "quit" || command == "exit" ) {
                    userWantsToExit = true;
                } else {
                    coutInvalidCommand(command);
                }
                break;
            default: 
                coutInvalidCommand(command);
                break;
        }

    } while( ! userWantsToExit );
}
