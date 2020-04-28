#include "prism.h"

/*
 This is a collaboration between Allen Harper and Stephen Marshall 
 in an attempt to construct an all-in-one .arff file parser, classifier
 structure system, and rule-building application.

 This program relies on no libraries outside the C++ Standard
 Template Library and as a result contains implementations of much 
 of the backing data structures that one would already find available 
 in a tool like Weka.


 Current Status:

    Reads nearly all .arff files presented with a few minor
        inconsistencies("@DATA" must be caps for example)

    Builds an object-oriented model of the data split into
        Instances, attributes, classifications, and datasets

    Every object has a suite of methods to facilitate seamless
        usage of member values and capabilities, as well as full
        support to print out every type in a human-readable format 
        through constistent toString() implementations.

    Features a nearly-complete implementation of J. Crendrowska's
        PRISM rule-building algorithm. As of now, a logical error 
        prevents the algorithm from self-terminating.

    The logical split of functions, declarations, and implementations
        is not currently finalized between prism.cpp and prism.h and
        as a result, prism.h functions more as a secondary code file
        than as a true header file.
 */


void buildRules(dataSet &d, vector<rule> ruleList){
    //for every class in dataset d
    for(string &c : d.classList){
        //cout <<"class :" <<c<< endl;//debug
        //vector<rule> ruleList;
        dataSet copy1(d);
        
        //while copy1 has any of class c in it
        //cout<<"copy1 has "<<copy1.numClass(c)<< " of " << c<<endl;//debug
        while(copy1.numClass(c) > 1){ 
            
            //make a rule to fill up over time
            rule r;
            dataSet copy2(copy1);
            //while copy 2 still has multiple classes (run until its only class c left)
           
            while(copy2.classList.size() > 0){
                //cout << "Loop2|";
                map<string, vector<string>>::iterator it;
                classification cl(c);
                //cl.value = c;
                vector<string> * sVecPtr;
                attribute temp("temp","temp",sVecPtr);
                //ok so max needs to start at some basic value that is valid, so the first one we find must become max
                attributeValuePair max(temp);
                double maxProb = -1;
                bool maxAssigned = false; //this flag will change after we've done that
                
                for(it = d.attributeList.begin(); it != d.attributeList.end(); ++it) {//for every attribute
                    
                    string attributeName = it->first;
                    //cout <<attributeName<<"|";
                    for(string &a : it->second){//for every value of that attribute
                        string attributeValue = a;
                        attributeValuePair avp(attributeName, attributeValue, &d.attributeList.at(attributeName));
                        //cout << avp.toString() << endl;
                        double avpProb = probability(c,avp,d);
                        //cout << " |prob:"<<avpProb;
                        if(!maxAssigned){//if we havent assigned max yet, just make this one max, no questions asked
                            max = avp;
                            maxProb = avpProb;
                            maxAssigned = true;
                        } else { //normal case
                            if (avpProb > maxProb){
                                //cout << maxProb <<" < " << avpProb << endl; //debug
                                max = avp;
                                maxProb = avpProb;
                                
                            } else if (avpProb = maxProb){
                                //TODO: resolve this conflict
                                //currently, do nothing, keep previous max
                            }
                            
                        }
                    }
                }
                //find pair with max probability (this is 'max')
                //copy2.remove instances w/out pair
                //add pair & class to r
                if(maxAssigned){
                    cout <<copy2.instances.size() << "\t";
                    copy2.removeElementsNotMatching(max);
                    cout << "removedElements\t"<< copy2.instances.size() << endl;
                    cout << copy2.toString(true) << "\n\n\n\n";
                    r.addAttribute(max);
                    r.addClass(c);
                } else {
                    break;
                }
            }
            cout << "end of inner while loop";//<--end of inner while loop
            //add r to ruleList
            bool pass = true;
            for(auto &cond : r.conditions){
                if(cond.name=="temp"){
                    cout <<"temp escaped"<<endl;
                    pass=false;
                }
            }
            if(pass){ruleList.push_back(r);}
            cout <<"just added rule:" <<r.toString() << endl;
            //copy1.remove all instances matching rule
            copy1.removeElementsMatching(r);
        };
    }

    
}

int main(int argc, char *argv[]){
    string fileName;
    cout << "Enter filename: ";
    cin >> fileName;
    //fileName = "soybean.arff";//im tired of typing it in
    //fileName ="weather.nominal.arff"; //this will do
    cout << endl;

    dataSet data1;
    fileInput(fileName, data1, true);
    cout << data1.toString(true) << endl;
    cout << "Size of dataSet: " << sizeof(data1) << " bytes\n\n";
    cout << "Would you like to build a ruleset?\n"
         << "(It probably wont work, prepare to force close program)\n";
    vector<rule> rules;
    buildRules(data1, rules);
    for(rule &r : rules){
        cout << r.toString() << endl;
    }


}