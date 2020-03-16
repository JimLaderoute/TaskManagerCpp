#include "Task.h"
#include <iostream>
using namespace std;

vector<Task *> Task::list;

Task::Task(unsigned long taskNumber, string dirpath, string subject, string description, char * created)
{
    this->subject = subject;
    this->description = description;
    this->taskNumber = taskNumber;
    // remove trailing \n in created 
    char *lfeed = strchr(created, '\n');
    if ( NULL != lfeed) {
        *lfeed = '\0';
    }
    this->created = strdup(created);
    this->hidden = false;
    this->closed = false;
    this->seconds = 0;
    this->dirPath = dirpath;

    list.push_back(this);
}

Task::~Task()
{
    free(created);
}

void Task::display(void)
{
    cout << taskNumber << " : " << created << " : " << subject << " : " << description << endl; 
}

void Task::setHidden(bool value)
{
    hidden = value;
}

void Task::setClosed(bool value)
{
    closed = value;
}

void Task::remove()
{
    for (vector<Task *>::iterator it = list.begin(); it != list.end(); ++it)
    {
        if ( *it == this)
        {
            list.erase(it);
            break;
        }
    }
}

string Task::getDirPath()
{
    return dirPath;
}

string Task::getSubject()
{
    return subject;
}

unsigned long Task::getTaskNumber()
{
    return taskNumber;
}

unsigned long Task::getSeconds()
{
    return seconds;
}

char * Task::getCreated()
{
    return created;
}

string Task::getDescription()
{
    return description;
}

unsigned long Task::addSeconds(unsigned int nseconds)
{
    seconds += nseconds;
    return seconds;
}