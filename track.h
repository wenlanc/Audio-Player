#ifndef TRACK_H
#define TRACK_H

#include <string>

using namespace std;

class Track
{
public:
    Track();

    int getDuration();

    string getName();

    bool getPlayed();

    bool getEnabled();

    string getLocation();

    void setName(string name);

    void setEnabled(bool e);

    void setPlayed(bool e);

    void setLocation(string location);

    void setDuration(int dur);

private:
    string name = "";

    string location = "";

    int dur = 0;

    bool enabled = true;

    bool played = false;
};

#endif // TRACK_H
