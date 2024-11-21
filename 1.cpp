//3rd one with file input and output

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <algorithm>
using namespace std;

 vector< set<char> > readTransactions(const  string& filename) {
     vector< set<char> > transactions;
     ifstream file(filename.c_str());
    if (file.is_open()) {
         string line;
        while ( getline(file, line)) {
             istringstream ss(line);
             string transactionId;
            ss >> transactionId; 
             set<char> items;
            char item;
            while (ss >> item) {
                items.insert(item);
            }
            transactions.push_back(items);
        }
        file.close();
    } else {
         cerr << "Unable to open file: " << filename <<  endl;
    }
    return transactions;
}

void generateSubsets(const  set<char>& items,  set< set<char> >& allSubsets,  set<char> currentSubset =  set<char>(),  set<char>::iterator it =  set<char>().end()) {
    if (it == items.end()) {
        if (!currentSubset.empty()) {
            allSubsets.insert(currentSubset);
        }
        return;
    }
     set<char>::iterator nextIt = it;
    ++nextIt;
    generateSubsets(items, allSubsets, currentSubset, nextIt);
    currentSubset.insert(*it);
    generateSubsets(items, allSubsets, currentSubset, nextIt);
}

 map< set<char>, int> countSupport(const  vector< set<char> >& transactions, const  set< set<char> >& candidates) {
     map< set<char>, int> supportCount;
    for ( vector< set<char> >::const_iterator transaction = transactions.begin(); transaction != transactions.end(); ++transaction) {
        for ( set< set<char> >::const_iterator candidate = candidates.begin(); candidate != candidates.end(); ++candidate) {
            if ( includes(transaction->begin(), transaction->end(), candidate->begin(), candidate->end())) {
                supportCount[*candidate]++;
            }
        }
    }
    return supportCount;
}

 vector< set<char> > filterItemsets(const  map< set<char>, int>& supportCount, int minSupport) {
     vector< set<char> > frequentItemsets;
    for ( map< set<char>, int>::const_iterator item = supportCount.begin(); item != supportCount.end(); ++item) {
        if (item->second >= minSupport) {
            frequentItemsets.push_back(item->first);
        }
    }
    return frequentItemsets;
}

int main() {
     vector< set<char> > transactions = readTransactions("3rd_input.txt");
    int minSupport = 2;
    int k = 1;
     set< set<char> > L_prev;

     ofstream outputFile("freq_itemset.txt");
    if (!outputFile.is_open()) {
         cerr << "Unable to open file: freq_itemset.txt" <<  endl;
        return 1;
    }

    while (true) {
         set< set<char> > Ck;
        if (k == 1) {
            for ( vector< set<char> >::const_iterator transaction = transactions.begin(); transaction != transactions.end(); ++transaction) {
                for ( set<char>::const_iterator item = transaction->begin(); item != transaction->end(); ++item) {
                     set<char> itemset;
                    itemset.insert(*item);
                    Ck.insert(itemset);
                }
            }
        } else {
             set< set<char> > prevItemsets = L_prev;
            for ( set< set<char> >::const_iterator it1 = prevItemsets.begin(); it1 != prevItemsets.end(); ++it1) {
                for ( set< set<char> >::const_iterator it2 =  next(it1); it2 != prevItemsets.end(); ++it2) {
                     set<char> candidate;
                     set_union(it1->begin(), it1->end(), it2->begin(), it2->end(),
                                    inserter(candidate, candidate.begin()));
                    if (candidate.size() == static_cast< size_t>(k)) {
                        Ck.insert(candidate);
                    }
                }
            }
        }

         map< set<char>, int> supportCount = countSupport(transactions, Ck);

         vector< set<char> > frequentItemsets = filterItemsets(supportCount, minSupport);

        if (frequentItemsets.empty()) {
            if (k == 1) {
                outputFile << "No itemsets of size " << k <<  endl;
            }
            break;
        }

        outputFile << "Frequent itemsets of size " << k << ":" <<  endl;
        for ( vector< set<char> >::const_iterator itemset = frequentItemsets.begin(); itemset != frequentItemsets.end(); ++itemset) {
            for ( set<char>::const_iterator item = itemset->begin(); item != itemset->end(); ++item) {
                outputFile << *item;
            }
            outputFile <<  endl;
        }

        L_prev.clear();
        for ( vector< set<char> >::const_iterator itemset = frequentItemsets.begin(); itemset != frequentItemsets.end(); ++itemset) {
            L_prev.insert(*itemset);
        }
        ++k;
    }
    cout <<"All the frequent itemsets are stored in the file freq_itemset.txt"<<endl;
    outputFile.close();
    return 0;
}