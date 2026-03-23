#include <bits/stdc++.h>
using namespace std;

struct Node{
    char ch;
    int f;
    Node *l,*r;
    Node(char c,int x){
        ch=c; f=x;
        l=r=NULL;
    }
};

// 🔥 Perfect Min-Heap comparator
struct cmp{
    bool operator()(Node* a,Node* b){
        if(a->f == b->f) return a->ch > b->ch;
        return a->f > b->f;
    }
};

string xoren(string s,string key){
    for(int i=0;i<s.size();i++){
        s[i]^=key[i%key.size()];
    }
    return s;
}

// 🔥 Smart RLE: Only compresses 3+ repeating chars to avoid bloat
string rle(string s){
    string res="";
    for(int i=0;i<s.size();i++){
        int cnt=1;
        while(i+1<s.size() && s[i]==s[i+1]){
            cnt++; i++;
        }
        if(cnt >= 3 || s[i] == '~'){
            res += "~" + to_string(cnt) + "|" + s[i];
        } else {
            while(cnt--) res += s[i];
        }
    }
    return res;
}

// 🔥 Smart RLE Decoder
string unrle(string s){
    string res="";
    for(int i=0;i<s.size();i++){
        if(s[i] == '~'){
            i++; 
            string num="";
            while(i<s.size() && isdigit(s[i])){
                num+=s[i]; i++;
            }
            if(i<s.size() && s[i]=='|'){
                i++;
                if(i<s.size()){
                    int cnt=stoi(num);
                    while(cnt--) res+=s[i];
                }
            }
        } else {
            res += s[i];
        }
    }
    return res;
}

Node* build(unordered_map<char,int> &freq){
    priority_queue<Node*,vector<Node*>,cmp> pq;
    for(auto &p:freq){
        pq.push(new Node(p.first,p.second));
    }
    while(pq.size()>1){
        Node* a=pq.top(); pq.pop();
        Node* b=pq.top(); pq.pop();
        Node* t=new Node('\0',a->f+b->f);
        t->l=a; t->r=b;
        pq.push(t);
    }
    return pq.top();
}

void gen(Node* root,string s,unordered_map<char,string> &mp){
    if(!root) return;
    if(!root->l && !root->r){
        mp[root->ch]=s;
    }
    gen(root->l,s+"0",mp);
    gen(root->r,s+"1",mp);
}

vector<unsigned char> pack(string &bits){
    vector<unsigned char> v;
    for(int i=0;i<bits.size();i+=8){
        unsigned char b=0;
        for(int j=0;j<8 && i+j<bits.size();j++){
            if(bits[i+j]=='1'){
                b|=(1<<(7-j));
            }
        }
        v.push_back(b);
    }
    return v;
}

string unpack(vector<unsigned char> &v,int cnt){
    string s="";
    for(auto b:v){
        for(int i=7;i>=0;i--){
            s += ((b>>i)&1)?'1':'0';
        }
    }
    return s.substr(0,cnt);
}

// 🔥 UNIQUE FILE GENERATOR
string getUniqueName(string base){
    int i = 1;
    while(true){
        string name = base + "_" + to_string(i) + ".yzip";
        ifstream f(name);
        if(!f) return name;
        i++;
    }
}

// 🔥 FINAL NAMING LOGIC
string getArchiveName(vector<string> &files){
    if(files.size() > 1){
        return getUniqueName("archived_bundle");
    }

    string base = files[0];

    int pos = base.find_last_of("/\\");
    if(pos != string::npos) base = base.substr(pos+1);

    int dot = base.find_last_of('.');
    if(dot != string::npos) base = base.substr(0, dot);

    return "archived_" + base + ".yzip";
}

void compress(vector<string> files,string outFile,string key){
    ofstream out(outFile,ios::binary);
    int n=files.size();
    out.write((char*)&n,sizeof(n));

    for(auto file:files){
        ifstream in(file,ios::binary);
        if(!in){
            cout<<"File not found: "<<file<<"\n";
            continue;
        }

        string text((istreambuf_iterator<char>(in)),{});
        in.close();

        // 1. Squash repeating characters first
        text = rle(text);

        // 2. Build Huffman frequencies
        unordered_map<char,int> freq;
        for(char c:text) freq[c]++;

        Node* root=build(freq);

        unordered_map<char,string> mp;
        gen(root,"",mp);

        string bits="";
        for(char c:text) bits+=mp[c];

        vector<unsigned char> bytes=pack(bits);

        // 3. ENCRYPT the final compressed bytes before saving
        string byteStr(bytes.begin(), bytes.end());
        byteStr = xoren(byteStr, key);
        bytes.assign(byteStr.begin(), byteStr.end());

        // --- WRITING METADATA ---
        int nameLen=file.size();
        out.write((char*)&nameLen,sizeof(nameLen));
        out.write(file.c_str(),nameLen);

        int codeSize=mp.size();
        out.write((char*)&codeSize,sizeof(codeSize));

        for(auto &p:mp){
            out.write(&p.first,1);
            int len=p.second.size();
            out.write((char*)&len,sizeof(len));
            out.write(p.second.c_str(),len);
        }

        int bc=bits.size();
        out.write((char*)&bc,sizeof(bc));

        int cs=bytes.size();
        out.write((char*)&cs,sizeof(cs));
        
        // --- WRITING PAYLOAD ---
        out.write((char*)bytes.data(),bytes.size());
    }

    out.close();
    cout<<"Saved as "<<outFile<<"\n";
}

void decompress(string inFile,string key){
    ifstream in(inFile,ios::binary);
    if(!in){
        cout<<"Archive not found\n";
        return;
    }

    int n;
    in.read((char*)&n,sizeof(n));

    while(n--){
        int nameLen;
        in.read((char*)&nameLen,sizeof(nameLen));

        string name(nameLen,' ');
        in.read(&name[0],nameLen);

        int codeSize;
        in.read((char*)&codeSize,sizeof(codeSize));

        unordered_map<string,char> rev;

        for(int i=0;i<codeSize;i++){
            char ch;
            in.read(&ch,1);

            int len;
            in.read((char*)&len,sizeof(len));

            string code(len,' ');
            in.read(&code[0],len);

            rev[code]=ch;
        }

        int bc,cs;
        in.read((char*)&bc,sizeof(bc));
        in.read((char*)&cs,sizeof(cs));

        vector<unsigned char> bytes(cs);
        in.read((char*)bytes.data(),cs);

        // 1. DECRYPT the raw payload first
        string byteStr(bytes.begin(), bytes.end());
        byteStr = xoren(byteStr, key);
        bytes.assign(byteStr.begin(), byteStr.end());

        // 2. Unpack bytes back to bits
        string bits=unpack(bytes,bc);

        // 3. Traverse Huffman tree to decode back to RLE text
        string cur="",dec="";
        for(char b:bits){
            cur+=b;
            if(rev.count(cur)){
                dec+=rev[cur];
                cur="";
            }
        }

        // 4. Decode the RLE back to original text
        dec=unrle(dec);

        string newName="decoded_"+name;

        ofstream out(newName,ios::binary);
        out<<dec;
        out.close();

        cout<<"Extracted "<<newName<<"\n";
    }

    in.close();
}

int main(int argc,char* argv[]){
    if (argc < 4) {
        cout << "Usage:\n";
        cout << "Compress: ./zipper 1 <num_files> <file1> ... <key>\n";
        cout << "Decompress: ./zipper 2 <archive.yzip> <key>\n";
        return 1;
    }

    int mode=atoi(argv[1]);

    if(mode==1){
        int n=atoi(argv[2]);
        vector<string> files;
        for(int i=0;i<n;i++){
            files.push_back(argv[3+i]);
        }
        string key=argv[3+n];

        string outFile=getArchiveName(files);
        compress(files,outFile,key);
    }
    else{
        string file=argv[2];
        string key=argv[3];
        decompress(file,key);
    }
}