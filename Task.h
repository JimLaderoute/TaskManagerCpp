#if !defined(TASK_H)
#define TASK_H

#include <string>       // to give us the string type
#include <string.h>
#include <ctime>
#include <vector>

using namespace std;

class Task {
public:
    static vector<Task *> list;
private:
    unsigned long taskNumber;
    string subject;             // Short description of the task
    string description;         // longer description of the task 
    char * created;             // The date this task was created by the user
    string dirPath;             // The directory path that holds the task info
    bool hidden;                // should this task be hidden from normal viewing
    bool closed;                // has this task been closed (aka completed)
    unsigned long seconds;      // how many seconds this task has been active today
public:
    // constructor 
    Task(unsigned long taskNumber, string dirpath, string subject, string description, char * created);
    // de-structor
    ~Task();
    // get info
    string getSubject();
    string getDescription();
    void setHidden(bool);       // set the hidden field
    void setClosed(bool);       // set the closed field
    char * getCreated();            // get the time when this task was first created
    bool getHidden();
    bool getClosed();
    string getDirPath();
    unsigned long getSeconds();
    void display();
    unsigned long getTaskNumber();
    void remove();

    unsigned long addSeconds(unsigned int seconds);
};

#endif
