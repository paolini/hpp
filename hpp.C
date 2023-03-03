//makefile:CPP_PROG(MYNAME,BASENAME(MYNAME).h)

//#include <fstream.h>
//#include <pfstream.h>
//#include <strstream.h>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <clocale>
#include <cstring>
#include <sstream>
#include <fstream>
#include "hpp.h"
#include "gif.h"
#include "jpeg.h"

#include "pstream.h"
using namespace redi;
using namespace std;

void Buffer::Dump(unsigned long position) {
  long from,to;
  from=position-50;
  to=position+50;
  if (from<0) from=0;
  cerr<<"Buffer dump:\n";
  for (Seek(from);!End() && Tell()< to;) cerr<<(char)Read();
}

void Buffer::Delete(unsigned long from,unsigned long to) {
  assert (from>=0 && from<=to && to<=buf.size());
  buf.erase(buf.begin()+from,buf.begin()+to);
  if (cp>from && cp<=to) cp=from;
  else if (cp>to) cp-=to-from;
};

char *Buffer::Dup(unsigned long from,unsigned long to) {
  char *s=new char[to-from+1];
  assert(from>=0);
  assert(from<=to);
  assert(to<=buf.size());
  for (int i=0;from+i<to;i++)
    s[i]=buf[from+i];
  //memcpy(s,buf.begin()+from,to-from);
  s[to-from]=0;
  return s;
}

int Buffer::Insert(istream &in) {
  char c;
  static char cache[1024];

  while (in.good() && !in.eof()) {
    in.read(cache,sizeof(cache));
    buf.insert(iter(),cache,cache+in.gcount());
    cp+=in.gcount();
  }
  if (in.bad()) {
    cerr<<"problems reading from input file\n";
  }
  return 1;
}

Buffer::Buffer(char *s) {
  cp=0;
  Buffer();
  if (s) {
    for(;*s;s++)
      Insert(*s);
  }
}

Buffer::Buffer(istream &in) {
  cp=0;
  Buffer();
  Insert(in);
}

void Buffer::Out(ostream &out,unsigned int from,unsigned int to) {
  unsigned long oldpos=Tell();
  Seek(from);
  while (Tell()<to && !End()) out<<(char)Read();
  Seek(oldpos);
} 


Env::~Env() {
  int i;
  for (i=0;i<var.size();i++)
    var[i].Destroy();
}

void Env::envDump() {
  for (int i=0;i<var.size();i++) {
    cerr<<"$"<<var[i].field;
    if (var[i].value) {
      cerr<<"=";
      for (int k=0;var[i].value[k] && k<30;k++)
	cerr<<(char)var[i].value[k];
      cerr<<"\n";
    }
  }
  if (parent) {
    cerr<<"Parent's environment:\n";
    parent->envDump();
  }
}

void Env::Dump(char *message,unsigned long position) {
  if (message) cerr<<"Error: "<<message<<"\n";
  cerr<<"Environment dump:\n";
  envDump();
  buf->Dump(position);
  exit(1);
}

void Env::NEOF(unsigned long pos,char *name) {
  if (Current()==EOF) {
    cerr<<"Error: unexpected EOF in "<<name<<"\n";
    Dump(0,pos);
  }
}

char *Env::ReadMaybeName() {
  unsigned long oldpos=Tell();
  while (isname(Current())) Read();
  if (Tell()>oldpos)
    return Dup(oldpos,Tell());
  else {
    return 0;
  }
}


char *Env::ReadName() {
  unsigned long oldpos=Tell();
  while (1) {
    //    if (isexpandable(Current())) Expand(1);
    if (!isname(Current())) 
      return Dup(oldpos,Tell());
    Read();
  }
}

char *Env::ReadString() {
  int quoted=0;
  unsigned long oldpos=Tell();

  while (1) {
    if (isexpandable(Current())) Expand(quoted==0);
    if (Current()=='"') {
      quoted=!quoted;
      Delete(Tell(),Tell()+1);
    }
    else if (Current()=='\\') {
      Delete(Tell(),Tell()+1);
      Read(); //non interpreta il carattere escapato
    }
    else if (quoted==0 && !isstring(Current())) {
      break;
    }
    else if (Current()==EOF) {
      cerr<<"Error: unterminated string\n";
      Dump(0,oldpos);
    }
    else Read();
  }
  return Dup(oldpos,Tell());
}

Couple Env::ReadCouple() {
  SkipSpaces();
  char *var=0,*value=0;
  if (isangle(Current())) {
    cerr<<"internal error 83274\n";
    exit(1);
  }
  var=ReadString();
  if (!var || !*var) {
    if (!var) {
      var=strdup("");
      cerr<<"warning: assignement to null variable\n";
    }
  }
  SkipSpaces();
  if (Current()=='=') {
    Read();
    SkipSpaces();
    value=ReadString();
    SkipSpaces();
  }
  return Couple(var,value);
}

int Env::SkipToTag(char *tag1,char *tag2=0) {
  unsigned long begin=Tell();
  char *tag[3];
  int count=0;
  tag[0]=tag1;tag[1]=tag2;
  if (tag1[0]=='/') tag[2]=tag1+1;
  else if (tag2[0]=='/') tag[2]=tag2+1;
  else tag[2]=0;
  for (;;) {
    while (!istag(Current())) Read();
    NEOF(begin,tag1);
    begin=Tell();
    Read();
    SkipSpaces();
    char *name=ReadMaybeName();
    for (int i=0; name && i<3;i++) {
      if (!tag[i]) continue;
      if (!strcasecmp(name,tag[i])) {
	while (!isangle(Current())) Read();
	NEOF(begin,tag[0]);
	if (i==2) //e' un sottotag
	  count++;
	else if (count==0) { //trovato!!!
	  Read();
	  buf->Delete(begin,Tell());
	  delete name;
	  return i;
	}
	else if (tag[i][0]=='/') //e' la fine di un sottotag
	  count--;
      }
    }
    delete name;    
  }
}

char *Env::ValueOf(char *name) {
  for (int i=0;i<var.size();i++) {
    if (!strcasecmp(var[i].field,name))
      {
	if (var[i].value)
	  return var[i].value;
	else return "";
      }
  }
  if (parent) return parent->ValueOf(name);
  return 0; /* variabile non definita*/
}

int Env::HasClosure(char *name) {
  for (int i=0;i<var.size();i++) {
    if (var[i].field[0]=='/' && !strcasecmp(var[i].field+1,name))
      return 1;
  }
  if (parent) return parent->HasClosure(name);
  else return 0;
}

int Env::IsTag(char *name) {
  for (int i=0;i<var.size();i++) {
    if (var[i].tag==1 && !strcasecmp(var[i].field,name))
      return 1;
  }
  if (parent) return parent->IsTag(name);
  else return 0;
}

void Env::Set(char *name,char *value,int tag) {
  int i;
  for (i=0;i<var.size() && !!strcasecmp(name,var[i].field);i++);
  if (i<var.size()) {
    delete var[i].value;
    var[i].value=value;
    var[i].tag=tag;
    delete name;
  }
  else
    AddCouple(Couple(name,value,tag));
}

void Env::Unset(char *name) {
  int i;
  for (i=0;i<var.size() && !!strcasecmp(name,var[i].field);i++);
  if (i<var.size()) {
    var[i].Destroy();
    var.erase(var.begin()+i);
  }
  else
    {
      cerr<<"warning: unset: variable "<<name<<" not set\n";
    }
}



void Env::ExpandNospace() {
  unsigned long begin=Tell();
  while (!isangle(Current())) {
    Couple cop=ReadCouple();
    cop.Destroy();
  }
  NEOF(begin,"nospace");
  Read();
  SkipSpaces();
  Delete(begin,Tell());
}

void Env::ExpandIgnore() {
  unsigned long begin=Tell();
  SkipSpaces();
  while (!isangle(Current())) {
    Couple cop=ReadCouple();
    cop.Destroy();
  }
  NEOF(begin,"ignore");
  Read();
  SkipToTag("/ignore");
  Delete(begin,Tell());
}

void Env::ExpandLiteral() {
  unsigned long begin=Tell();
  SkipSpaces();
  while (!isangle(Current())) {
    Couple cop=ReadCouple();
    cop.Destroy();
  }
  NEOF(begin,"literal");
  Read();
  Delete(begin,Tell());
  SkipToTag("/literal");
}

void Env::ExpandSet(int def=0) {
  char *name=0, *value=0;
  unsigned long begin=Tell();
  SkipSpaces();
  int unset=0;

  while (!isangle(Current())) {
    Couple cop=ReadCouple();
    if (!strcasecmp(cop.field,"unset") && cop.value==0) {
      unset=1;
    }
    else {
      if (cop.value==0) {
	if (name) {
	  cerr<<"Error: two undefined names in set\n";
	  Dump(0,begin);
	}
	else
	  name=strdup(cop.field);
      }
      else {
	if (!unset)
	  Set(strdup(cop.field),strdup(cop.value));
	else
	  cerr<<"Warning: assigment in unset\n";
      }
    }
    cop.Destroy();
  }
  NEOF(begin,"set");
  Read();
  Delete(begin,Tell());
  if (name)
    {
      if (!unset) {
	begin=Tell();
	if (def)
	  SkipToTag("/def");
	else
	  SkipToTag("/set");
	value=Dup(begin,Tell());
	Delete(begin,Tell());
	Set(name,value,def);
      }
      else //unset
	{
	  Unset(name);
	  delete name;
	}
    }
}

Env *Env::SubEnv() {
  unsigned long begin=Tell();
  Env *env=new Env();
  env->parent=this;
  while (!isangle(Current())) {
    env->AddCouple(ReadCouple());
  }
  NEOF(begin,"env");
  Read();
  Delete(begin,Tell());
  return env;
}

void Env::ExpandTag(char *name,int tag) {
  unsigned long begin=Tell();
  SkipSpaces();
  Delete(begin,Tell());
  Env *env=SubEnv();
  Buffer *bf=new Buffer(ValueOf(name));
  env->buf=bf;
  if (tag && HasClosure(name)) {
    char *closure=new char[strlen(name)+2];
    closure[0]='/';
    strcpy(closure+1,name);
    begin=Tell();
    SkipToTag(closure);
    bf->Insert(Dup(begin,Tell()));
    Delete(begin,Tell());
    bf->Insert(ValueOf(closure));
    delete closure;
  }

  bf->Seek();
  env->ExpandAll();
  
  begin=Tell();
  bf->Seek();
  Insert(*bf);
  delete bf;
  delete env;
  
}

void Env::ExpandUse() {
  char *name=0;
  
  unsigned long begin=Tell();
  SkipSpaces();
  {
    Couple cop=ReadCouple();
    if (!cop.field) {
      cerr<<"internal error 4852\n";
    }
    if (cop.value) {
      cerr<<"warning: unused value in use\n";
    }
    name=strdup(cop.field);
    SkipSpaces();
    cop.Destroy();
  }
  Delete(begin,Tell());

  ExpandTag(name,0);
  delete name;
}

void Env::ExpandIf() {
  int cond=-1;
  int noot=0;
  unsigned long begin=Tell();
  SkipSpaces();
  while (!isangle(Current())) {
    Couple cop=ReadCouple();
    if (!strcasecmp(cop.field,"not")) {
      if (cop.value) {
	cerr<<"warning: assignment to not\n";
      }
      noot=!noot;
    }
    else if (cop.value==0) {
      if (ValueOf(cop.field))
	cond=noot?(cond==0):(cond!=0);
      else 
	cond=noot?1:0;
    }
    else {
      if (!strcmp(cop.field,cop.value))
	cond=noot?(cond==0):(cond!=0);
      else 
	cond=noot?1:0;
    }
    cop.Destroy();
  }
  NEOF(begin,"tag");
  Read();
  Delete(begin,Tell());
  begin=Tell();
  int tg=SkipToTag("else","/if");
  unsigned long middle;
  if (tg==0) {
    middle=Tell();
    SkipToTag("/if");
  }
  else middle=Tell();
  if (cond)
    Delete(middle,Tell());
  else
    Delete(begin,middle);
  Seek(begin);
}

void Env::ExpandForeach() {
  unsigned long begin=Tell();
  char *name=0;
  vector<char *> value;
  SkipSpaces();
  while (!isangle(Current())) {
    Couple cop(ReadCouple());
    if (cop.value)
      cerr<<"warning: unused assignment in foreach\n";
    else if (!name) 
      name=strdup(cop.field);
    else
      value.push_back(strdup(cop.field));
    cop.Destroy();
  }
  NEOF(begin,"foreach");
  Read();
  Delete(begin,Tell());
  begin=Tell();
  SkipToTag("/foreach");
  char *body=Dup(begin,Tell());
  Delete(begin,Tell());
  for (int i=0;i<value.size();i++) {
    Buffer *bf=new Buffer(body);
    bf->Seek();
    Env *env=new Env;
    env->parent=this;
    env->buf=bf;
    env->Set(strdup(name),value[i]);
    if (i==0) env->Set(strdup("first"),strdup(""));
    if (i==value.size()-1) env->Set(strdup("last"),strdup(""));
    env->ExpandAll();
    delete env;
    bf->Seek();
    Insert(*bf);
    delete bf;
  }
  delete name;
}

void Env::InsertEnv(int global) {
  if (global && parent) parent->InsertEnv(global);
  for (int i=0;i<var.size();i++) {
    if (i) Insert(" ");
    Insert(var[i].field);
    if (var[i].value && strlen(var[i].value)) {
      Insert("=\"");
      Insert(var[i].value);
      Insert("\"");
    }
  }
}

void Env::ExpandEnv() {
  unsigned long begin=Tell();
  int global=0;
  SkipSpaces();
  while (!isangle(Current())) {
    Couple cop(ReadCouple());
    if (!strcasecmp(cop.field,"global")) {
      global=1;
      if (cop.value) {
	cerr<<"warning: unused assignment in env\n";
      }
    }
    cerr<<"warning: ignored options in env\n";
    cop.Destroy();
  }
  NEOF(begin,"env");
  Read();
  Delete(begin,Tell());
  InsertEnv(global);
}

void Env::ExpandMessage() {
  unsigned long oldpos=Tell();
  int exit=0;
  int body=1;
  SkipSpaces();
  while (!isangle(Current())) {
    Couple cop(ReadCouple());
    if (!strcasecmp(cop.field,"err")) {
      if (cop.value) {
	cerr<<"User error:\n";
	cerr<<cop.value<<"\n";
	body=0;
	Dump(0,oldpos);
      }
    }
    else if (!strcasecmp(cop.field,"msg")) {
      if (cop.value) {
	cerr<<cop.value<<"\n";
	body=0;
      }
    } else {
      body=0;
      cerr<<cop.field;
      if (cop.value) {
	cerr<<"="<<cop.value;
      }
      cerr<<"\n";
    }
    cop.Destroy();
  }
  NEOF(oldpos,"message");
  Read();
  Delete(oldpos,Tell());
  if (body) {
    oldpos=Tell();
    SkipToTag("/message");
    buf->Out(cerr,oldpos,Tell());
  }
}

void Env::ExpandFile() {
  int operation=0;//1: input 2:output 3: template
  int literal=0;
  char *command=0;
  unsigned int oldpos=Tell();
  SkipSpaces();
  while (!isangle(Current())) {
    Couple cop(ReadCouple());
    if (!command) {
      if (!strcasecmp(cop.field,"input")){
	operation=1;
	if (cop.value )
	  command=strdup(cop.value);
      }
      else if (!strcasecmp(cop.field,"output")) {
	operation=2;
	if (cop.value)
	  command=strdup(cop.value);
      }
      else if (!strcasecmp(cop.field,"literal")) {
	literal=1;
      }
      else {
	cerr<<"warning: unknown option in file: "<<cop.field<<"\n";
      }
    } else {
      cerr<<"warning: option already defined in file \n";
    }
    cop.Destroy();
  }
  NEOF(oldpos,"file");
  Read();
  Delete(oldpos,Tell());
  oldpos=Tell();
  if (operation==1) { //input 
    if (command) {
      istream * cmd;
      if (command[0]=='|')
	cmd=new ipstream(command+1);
      else
	cmd=new ifstream(command);

      //      if (command[0]=='|') cmd=new ipstream(command);
      //      else cmd=new ifstream(command);
      if (cmd->bad()) {
	cerr<<"Error: opening input file "<<command<<"\n";
	Dump(0,oldpos);
      }
      buf->Insert(*cmd);
      if (!literal)
	Seek(oldpos);
      delete cmd;
    }
    else 
      {
	cerr<<"warning: reading from stdin:\n";
	buf->Insert(cin);
      }
  }
  else if (operation==2) {
    if (command) {
      ostream *cmd;
      if (command[0]=='|')
	cmd=new opstream(command+1);
      else
	cmd=new ofstream(command);
//       if (command[0]=='|') cmd=new opstream(command);
//       else cmd=new ofstream(command);
      if (cmd->bad()) {
	cerr<<"Error: opening output file "<<command<<"\n";
	Dump(0,oldpos);
      }
      buf->Out(*cmd,0,Tell());
      buf->Delete(0,Tell());
      delete cmd;
    }
    else {
      buf->Out(cout,0,Tell());
      buf->Delete(0,Tell());
    }
  }
  else {
    cerr<<"Error: unspecified operation in file\n";
    Dump(0,oldpos);
  }
  delete command;
}

void Env::ExpandStat() {
  unsigned long oldpos=Tell();
  char *filename=0;
  char *format=0;
  int op=0;
  const int path=1;
  const int basename=2;
  const int extension=4;
  const int extmode=8; // 0:maximal extension, 1:minimal extension
  const int date=16;
  const int size=32;
  const int dim=64;

  while (!isangle(Current())) {
    Couple cop(ReadCouple());
    if (!strcasecmp(cop.field,"path")) {
      op|=path;
    }
    else if (!strcasecmp(cop.field,"base")) {
      op|=basename;
    }
    else if (!strcasecmp(cop.field,"ext")) {
      op|=extension;
    }
    else if (!strcasecmp(cop.field,"min")) {
      op |= extmode;
    }
    else if (!strcasecmp(cop.field,"max")) {
      op&=~extmode;
    }
    else if (!strcasecmp(cop.field,"file") && cop.value && !filename) 
      filename=strdup(cop.value);
    else if (!strcasecmp(cop.field,"date")) {
      op|=date;
    }
    else if (!strcasecmp(cop.field,"format") && cop.value && !format) {
      format=strdup(cop.value);
    }
    else if (!strcasecmp(cop.field,"size")) {
      op|=size;
    }
    else if (!strcasecmp(cop.field,"dim")) {
      op|=dim;
    }
    else {
      cerr<<"warning: unused option "<<cop.field<<" in stat\n";
    }
    cop.Destroy();
  }
  NEOF(oldpos,"stat");
  Read();
  Delete(oldpos,Tell());
  
  if (!filename && (op&(path|basename|extension|size|dim))) {
    if (ValueOf("sourcefile"))
      filename =strdup(ValueOf("sourcefile"));
    if (!filename) Dump("no filename specified in stat",oldpos);
    
  }
  
  char *base,*ext=0;
  if (filename) {
    base=filename;
    for (char *p=filename;*p;p++) {
      if (*p=='/' || *p=='\\') base=p+1;
      if (*p=='.' && ((op&extmode)?1:(!ext))) ext=p+1;
    }  
  }
  if (op&path) {
    for (char *p=filename;p<base; p++) Insert(*p);
  }
  if (op&basename) {
    for (char *p=base;p+1<ext;p++) Insert(*p);
  }
  if (op&extension) {
    if (op&basename) Insert('.');
    for (char *p=ext;*p;p++) Insert(*p);
  }
  if (op&date) {
    if (!format) format=strdup("%d.%m.%Y");
    struct tm *T;
    struct stat S;
    char s[80];
    if (filename) {
      stat(filename,&S);
      T=localtime(&(S.st_mtime));
    } else {
      time_t t=time(0);
      T=localtime(&t);
    }
    strftime(s,79,format,T);
    Insert(s);
  }
  if (op&size) {
    struct stat S;
    stat(filename,&S);
    //strstream s; 
    ostringstream s;
    if (op!=size)
      s<<" ";
    s<<S.st_size;
    s<<(char)0;
    Insert(s.str().c_str());
    //    delete s.str();
  }
  if (op&dim) {
    ifstream fp(filename);
    if (!fp) {
      cerr<<"Couldn't open file "<<filename<<"\n";
      exit(1);
    }
    int width=0,height=0;
    if (GifFormatFound(fp)) {
      GifReadDimensions(fp,&height,&width);
    }
    else if (JpegFormatFound(fp)) {
      JpegReadDimensions(fp,&height,&width);
    }
    else
      cerr<<"Warning: cannot find dimensions of image "<<filename<<"\n";
    if (width>0 && height>0) {
      stringstream s;
      s<<" WIDTH="<<width<<" HEIGHT="<<height<<(char)0;
      Insert(s.str().c_str());
      //      delete s.str();
    }
  }
  delete format;
  delete filename;
}

void Env::ExpandClear() {
  unsigned long oldpos=Tell();
  SkipSpaces();
  while (!isangle(Current())) {
    Couple cop(ReadCouple());
    cop.Destroy();
  }
  NEOF(oldpos,"clear");
  Read();
  Delete(oldpos,Tell());
  Delete(0,Tell());
}

void Env::ExpandProcess() {
  unsigned long oldpos=Tell();
  SkipSpaces();
  while (!isangle(Current())) {
    Couple cop(ReadCouple());
    cop.Destroy();
  }
  NEOF(oldpos,"process");
  Read();
  Delete(oldpos,Tell());
  oldpos=Tell();
  SkipToTag("/process");
  char *s=Dup(oldpos,Tell());
  Delete(oldpos,Tell());
  Buffer bf(s);
  delete s;
  Env env;
  env.buf=&bf;
  env.parent=this;
  bf.Seek();
  env.ExpandAll();
  bf.Seek();
  bf.SkipSpaces();
  if (!bf.End()) {
    cerr<<"Warning: non empty buffer destroyed\n";
  }
}

void Env::Expand(int mode=0){ 
  //0: rimane in fondo all'espansione 1:torna all'inizio
  //espande solo i buffer che iniziano per $ o < (isexpandable)
  //puo' succedere che non venga espanso niente o comunque
  //che rimanga un $ o < come primo carattere e che un'espansione
  //successiva non porti ulteriori modifiche => non espandere due
  //volte nella stessa posizione!

  char *name, *value;
  unsigned long oldpos,beginname;
  oldpos=Tell();
  switch(Current()) {
  case '<':
    Read();
    SkipSpaces();
    name=ReadMaybeName();
    if (!name) name=strdup("");
    //    cerr<<"cmd: "<<name<<"\n";
    if (!strcasecmp(name,"ignore")) {
      Delete(oldpos,Tell());
      ExpandIgnore();
    }
    else if (!strcasecmp(name,"literal")) {
      Delete(oldpos,Tell());
      ExpandLiteral();
    }
    else if (!strcasecmp(name,"nospace")) {
      Delete(oldpos,Tell());
      ExpandNospace();
    }
    else if (!strcasecmp(name,"if")) {
      Delete(oldpos,Tell());
      ExpandIf();
    }
    else if (!strcasecmp(name,"foreach")) {
      Delete(oldpos,Tell());
      ExpandForeach();
    }
    else if (!strcasecmp(name,"set")) {
      Delete(oldpos,Tell());
      ExpandSet();
    }
    else if (!strcasecmp(name,"def")) {
      Delete(oldpos,Tell());
      ExpandSet(1);
    }
    else if (!strcasecmp(name,"env")) {
      Delete(oldpos,Tell());
      ExpandEnv();
    }
    else if (!strcasecmp(name,"file")) {
      Delete(oldpos,Tell());
      ExpandFile();
    }
    else if (!strcasecmp(name,"stat")) {
      Delete(oldpos,Tell());
      ExpandStat();
    }    
    else if (!strcasecmp(name,"process")) {
      Delete(oldpos,Tell());
      ExpandProcess();
    }    
    else if (!strcasecmp(name,"message")) {
      Delete(oldpos,Tell());
      ExpandMessage();
    }    
    else if (!strcasecmp(name,"clear")) {
      Delete(oldpos,Tell());
      ExpandClear();
    }
    else if (!strcasecmp(name,"use")) {
      Delete(oldpos,Tell());
      ExpandUse();
    }
    else if (IsTag(name)){
	Delete(oldpos,Tell());
	ExpandTag(name);
    }
    else {
      Seek(oldpos);
      Read(); //il carattere si espande a se stesso
    }
    break;
  case '$':
    Read();
    if (Current()=='{') {
      Read();
      name=ReadName();
      if (Current()!='}')
	Dump("} expected",Tell());
      Read();
    }
    else 
      name=ReadName();
    value=ValueOf(name);
    if (value) {
      Delete(oldpos,Tell());
      Insert(value);
    }
    delete name;
    break;
  case '&':
    Read();
    name=ReadName();
    if (Current()!=';')
      Dump("; expected",Tell());
    Read();
    value=ValueOf(name);
    if (value) {
      Delete(oldpos,Tell());
      Insert(value);
    }
    break;
  case '\\':
    Read();
    if (isexpandable(Current())) {
      Delete(oldpos,Tell());
    }
    break;
  default:
    cerr<<"Expand internal error 84745\n";
    exit(1);
    break;
  }
  if (mode) 
    Seek(oldpos);
}

void Env::ExpandAll() {
  while (!End()) {
    if (isexpandable(Current())) Expand(0);
    else Read();
  }
}

char *InternalRc= 
"<if sourcefilelist>"
"<foreach SourceFile $sourcefilelist>"
"<message \"Processing file $file\">"
"<process>"
"<set outputfile=<stat base file=$sourceFile>.html>"
"<set sourcefile=$sourceFile>"
"<clear><file input=$SourceFile><file output><nospace>"
"</process>"
"</foreach>"
"<else>"
"<message>"
"No files specified.\n"
"try: hpp <hpp-files>\n"
"</message>"
"<output>"
"</if>";


char *strndup(char *s,int n) {
  char *p=new char[n+1];
  strncpy(p,s,n);
  p[n]=0;
  return p;
}

void Env::Options(int argc,char *argv[]) {
  Buffer list;
  int nfiles=0;
  for (int i=1;i<argc;i++) {
    if (strchr(argv[i],'=')) {
      char *p=strchr(argv[i],'=');
      Set(strndup(argv[i],p-argv[i]),strdup(p+1));
    }
    else {//un file
      if (nfiles)
	list.Insert(" ");
      list.Insert(argv[i]);
      nfiles++;
    }
  }
  char *s=list.Dup(0,list.Tell());
  if (s && *s)
    Set(strdup("sourcefilelist"),s);
}

#ifndef DEFAULT_RC
#define DEFAULT_RC 0
#endif

#ifndef USER_RC
#define USER_RC "~/.hpp"
#endif

#ifndef LOCAL_RC
#define LOCAL_RC ".hpp"
#endif

char *fallback[]={LOCAL_RC,USER_RC,DEFAULT_RC,0};


main(int argc,char *argv[]) {
  setlocale(LC_ALL,"");
  
  Env mainenv;
  Buffer mainbuf;
  mainenv.buf=&mainbuf;
  mainenv.Options(argc,argv);  
  {
    char *rc;
    rc=mainenv.ValueOf("sourcefile");
    ifstream *in=0;
    for (int i=0;rc==0 && fallback[i];i++) {
      rc=fallback[i];
      in= new ifstream(rc);
      if (in && in->good()) {
	mainenv.Set(strdup("sourcefile"),strdup(rc));
	mainbuf.Insert(*in);
	delete in;
      }
      else rc=0;
    }
    if (rc==0) {
      mainbuf.Insert(InternalRc);
    }
  }
  mainbuf.Seek();
  mainenv.ExpandAll();
}
