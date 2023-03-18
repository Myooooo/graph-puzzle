#include <iostream>
#include <sstream>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

// global variables
int** country;
char** build;
char** destroy;

void initTables(int n) {
    country = new int*[n];
    build = new char*[n];
    destroy = new char*[n];

    for(int i = 0; i < n; i++) {
        country[i] = new int[n];
        build[i] = new char[n];
        destroy[i] = new char[n];
    }
}

void deleteTables(int n) {
    for(int i = 0; i < n; i++) {
        delete[] country[i];
        delete[] build[i];
        delete[] destroy[i];
    }
    
    delete[] country;
    delete[] build;
    delete[] destroy;
}

void printTables(int n) {
    cout << endl;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            cout << country[i][j] << " ";
        }
        cout << endl;
    }

    cout << endl;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            cout << build[i][j] << " ";
        }
        cout << endl;
    }

    cout << endl;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            cout << destroy[i][j] << " ";
        }
        cout << endl;
    }
}

// sort function for pair of pair by cost
bool sortByCost(const pair<pair<int,int>,int> &a, const pair<pair<int,int>,int> &b) {
    return (a.second < b.second);
}

// find the subset root for node n
int findSet(int n, int* parent) {
    //cout << "\tparent of " << n << ": ";
    while(parent[n] != n) n = parent[n];
    //cout << n << endl;
    return n;
}

// join sets between nodes
void joinSet(int n1, int n2, int* parent) {
    int a = findSet(n1,parent);
    int b = findSet(n2,parent);
    parent[a] = b;
}

// returns true if the two nodes form a cycle
bool isCycle(int n1, int n2, int* parent) {
    if (findSet(n1,parent) != findSet(n2,parent)) return false;
    return true;
}

// find the minimum cost path using Kruskal’s MST
int findCost(int n) {

    // record edges
    vector<pair<pair<int,int>,int>> edges;
    int parent[n];
    int min_cost = 0;

    // preprocessing of cost table
    int cost;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            // ignore diagnals and replicates
            if(i < j) {
                if(country[i][j] == 0) {
                    // disconnected, use build cost
                    if(build[i][j] >= 'a') {
                        cost = build[i][j] - 'a' + 26;
                    } else {
                        cost = build[i][j] - 'A';
                    }
                } else {
                    // connected, use destroy cost inversed
                    // so edges with higher destruction cost will be selected first
                    if(destroy[i][j] >= 'a') {
                        cost = 0 - (destroy[i][j] - 'a' + 26);
                    } else {
                        cost = 0 - (destroy[i][j] - 'A');
                    }
                }
                // format edge and append to vector
                edges.push_back(make_pair(make_pair(i,j),cost));
            }
        }

        // initialise parent
        parent[i] = i;
    }

    // sort edges in ascending order by cost
    sort(edges.begin(),edges.end(),sortByCost);

    // pick n-1 edges
    int picked = 0;
    while(picked < n - 1) {
        // pick edge and remove from vector
        pair<pair<int,int>,int> edge = edges[0];
        edges.erase(edges.begin());

        int n1 = edge.first.first;
        int n2 = edge.first.second;

        //cout << "edge: " << n1 << " " << n2 << " " << edge.second << endl;

        // check if the edge forms a cycle
        if(!isCycle(n1,n2,parent)) {
            // join the sets of the two nodes and add cost to result
            joinSet(n1,n2,parent);
            //cout << "\t<added>" << endl;
            
            // add build cost if needed
            if(country[n1][n2] == 0) min_cost += edge.second;
            picked ++;
        } else {
            // add destruction cost if needed
            if(country[n1][n2] == 1) min_cost -= edge.second;
        }

        //cout << "\tcurrent cost: " << min_cost << endl;
        
    }

    // add destruction cost for remaining connected edges
    for(auto &e : edges) {
        int n1 = e.first.first;
        int n2 = e.first.second;
        if(country[n1][n2] == 1) {
            //cout << "destroying edge: " << n1 << " " << n2 << " " << e.second << endl;
            min_cost -= e.second;
        }
    }

    return min_cost;
}

int main() {
    // get input from terminal
    string input;
    getline(cin,input);

    // format input using string stream
    stringstream ss(input);
    string c;
    string b;
    string d;

    // split by space
    ss >> c >> b >> d;

    // count the dimension of 2D arrays
    int n = 1;
    for(auto &ch : c) {
        if(ch == ',') n++;
    }

    // format strings into 2D arrays
    initTables(n);
    int k = 0;

    // iterate through each entry
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(c[k] == ',') {
                // skip comma
                j--;
            } else {
                // store value
                country[i][j] = c[k] - '0';
                build[i][j] = b[k];
                destroy[i][j] = d[k];
            }
            k++;
        }
    }

    //printTables(n);

    cout << findCost(n) << endl;

    deleteTables(n);

    return 0;
}