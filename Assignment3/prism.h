#pragma once
#include <math.h>
#include "arffParser.h"
//All other includes are rolled into arffParser.h

using namespace std;

struct attributeValuePair{ //kinda a silly class, its just a shell around attribute
    attribute *a;
    attributeValuePair(string name, string value, vector<string> * possibleValues){
        a = new attribute(name, value, possibleValues);
    }
    inline bool operator == (const attributeValuePair &rhs){//TODO: inline???
        return this->a == rhs.a; //reuse the operator written for attribute
    }
    ~attributeValuePair(){ //since I made attribute a with new
        delete a;
    }
};
//TODO: I'll add the prism.cpp function declarations here, but after I'm done with them
//double relativeFreq(classification c, dataSet &d);
