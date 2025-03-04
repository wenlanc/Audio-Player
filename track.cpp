#include "track.h"

Track::Track()
{
    dur = 0;
}

string Track::getName()
{
    return name;
}
string Track::getLocation()
{
    return location;
}

void Track::setName(string name)
{
    this->name = name;
}

void Track::setEnabled(bool e)
{
    this->enabled = e;
}

bool Track::getPlayed()
{
    return played;
}

bool Track::getEnabled()
{
    return enabled;
}

void Track::setPlayed(bool e)
{
    this->played = e;
}

int Track::getDuration()
{
    if (enabled)
        return dur;
    else
        return 0;
}

void Track::setDuration(int dur)
{
    this->dur = dur;
}

void Track::setLocation(string location)
{
    this->location = location;
}
