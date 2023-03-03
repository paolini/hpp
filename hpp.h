#include <vector>
#include <ctype.h>
#include <iostream>
#include <cstring>

#define NDEBUG
#include <cassert>

using namespace std;

class Buffer {
private:
  const static int MaxStack=16;
  vector<char> buf;
  unsigned long cp;
protected:
  vector<char>::iterator iter(){return buf.begin()+cp;};
  vector<char>::iterator begin(){return buf.begin();};
  vector<char>::iterator end(){return buf.end();};
public:
  Buffer(){cp=0;};
  Buffer(istream &in);
  Buffer(Buffer &b1,int len);
  Buffer(char *s);
  void Insert(char c) {buf.insert(iter(),c);cp++;};
  void Insert(const char *s) {
    int n=strlen(s);
    buf.insert(iter(),s,s+n);
    cp+=n;
  };
  void Insert(Buffer &Buf){
    buf.insert(iter(),Buf.iter(),Buf.end());
    cp+=Buf.end()-Buf.iter();
    Buf.cp=Buf.end()-Buf.begin();
  };
  int Insert(istream &in);
  void Delete(unsigned long from,unsigned long to);
  void Seek(unsigned long pos=0) {cp=pos;};
  unsigned long Tell() {return cp;};
  int End(){assert(cp<=buf.size() && cp>=0);return cp>=buf.size();};
  int Current(){return End()?EOF:buf[cp];};
  int Read(){return End()?EOF:buf[cp++];};
  void SkipSpaces() {while(isspace(Current())) Read();};
  void Out(ostream &out,unsigned int from,unsigned int to);
  char &operator[](int pos){return buf[cp+pos];};
  int Word(char *s); 
  char *Dup(unsigned long from,unsigned long to);
  void Dump(unsigned long position);
};

class Couple { // campo=valore
public:
  char tag; /*0: simple var, 1: tag*/
  char *field;
  char *value;
  Couple(char *Field=0,char *Value=0,int Tag=0){
    field=Field,value=Value;tag=Tag;};
  void Destroy(void) {delete field; delete value;};
};

class Env {
private:
  Env *parent;
  vector<Couple> var;
protected:
  int isname(int c){return isalnum(c) || c=='/' || c=='_' || c=='-';};
  int isstring(int c) {return isgraph(c) && !(isspace(c) || c=='>' || c=='=' || c=='}');};
  int isangle(int c){return c=='>' || c==EOF;};
  int istag(int c){return c=='<' || c==EOF;};
  int isexpandable(int c){return c=='<' || c=='&' || c=='$' || c=='\\';};

  void AddCouple(Couple cop){var.push_back(cop);};
public:
  // riscrivo le funzioni utili di buf:
  Buffer *buf;
  void Insert(const char *s) {buf->Insert(s);};
  void Insert(Buffer &bf) {buf->Insert(bf);};
  void Insert(char c) {buf->Insert(c);};
  void Delete(unsigned long from, unsigned long to){buf->Delete(from,to);};
  void Seek(unsigned long pos=0){buf->Seek(pos);};
  unsigned long Tell(){return buf->Tell();};
  int End(){return buf->End();};
  int Current(){return buf->Current();};
  int Read(){return buf->Read();};
  void SkipSpaces(){
//    if (isexpandable(Current())) Expand(1);
    buf->SkipSpaces();};
  char *Dup(unsigned long from,unsigned long to){return buf->Dup(from,to);};

  Env(){parent=0;};
  ~Env();

  char *ValueOf(char *name);
  void Set(char *name,char *value,int tag=0);
  void Unset(char *name);
  int HasClosure(char *name);
  int IsTag(char *name);
  void InsertEnv(int global); //qui buf ci va veramente

  void Expand(int mode);
  void ExpandAll();
  char *ReadName();
  char *ReadMaybeName();
  char *ReadString();
  Couple ReadCouple();

  Env *SubEnv();

  void ExpandIgnore();
  void ExpandLiteral();
  void ExpandNospace();
  void ExpandSet(int tag);
  void ExpandTag(char *name,int tag=1);
  void ExpandUse();
  void ExpandIf();
  void ExpandForeach();
  void ExpandEnv();
  void ExpandStat();
  void ExpandFile();
  void ExpandProcess();
  void ExpandMessage();
  void ExpandClear();

  int SkipToTag(char *,char *);

  void Options(int argc,char *argv[]);
  void envDump();
  void Dump(char *message,unsigned long position);
  void NEOF(unsigned long pos,char *name);
};

