#include "data_types.h"
#include<cassert>
#include<strings.h>
#include "queries.h"

using namespace std;

string to_db_type(const int*){
	return "int(11)";
}

std::string to_db_type(const Part_number*){
	return "varchar(100)";
}

Input show_input(DB db,std::string const& name,Part_number const& current){
	auto r=show_input(db,name,current.data);
	r.notes+="If left blank, will be automatically generated.";
	return r;
}

int rand(const int*){ return rand()%100; }

int parse(const int*,string const& s){ return stoi(s); }

Input with_suggestions(string const& name,string const& value,vector<string> const& suggestions){
	stringstream ss;
	ss<<"<input name=\""<<name<<"\" list=\""<<name<<"\" value=\""<<value<<"\">";
	ss<<"<datalist id=\""<<name<<"\">";
	for(auto a:suggestions){
		ss<<"<option value=\""<<a<<"\">";
	}
	ss<<"</datalist>";
	return Input{name,ss.str(),""};
}

Input show_input(DB,string const& name,string const& value){
	//return "<br>"+name+":<input type=\"text\" name=\""+name+"\" value=\""+value+"\">";
	return Input{name,"<input type=\"text\" name=\""+name+"\" value=\""+value+"\">",""};
}

Input show_input(DB db,string const& name,int value){
	auto x=show_input(db,name,as_string(value));
	x.notes="(integer)";
	return x;
}

Input show_input(DB db,string const& name,double value){
	auto x=show_input(db,name,as_string(value));
	x.notes="(fraction)";
	return x;
}

Input show_input(DB db,string const& name,unsigned value){
	auto x=show_input(db,name,as_string(value));
	x.notes="(non-negative integer)";
	return x;
}

Input show_input(DB db,string const& name,bool value){
	//return "<br>"+name+":<input type=\"checkbox\" name=\""+name+"\" "+
	//	[=](){ if(value) return "checked=on"; return ""; }()+"\">";
	return Input{
		name,
		"<input type=\"checkbox\" name=\""+name+"\" "+
		[=](){ if(value) return "checked=on"; return ""; }()+"\">"
		""
	};
}

string escape(string const& s){
	stringstream ss;
	ss<<"\"";
	for(auto c:s){
		if(c=='"'){
			nyi
		}else{
			ss<<c;
		}
	}
	ss<<"\"";
	return ss.str();
}

string escape(int i){ return as_string(i); }

template<typename Value,typename Display>
Input drop_down(string const& name,Value const& current,vector<pair<Value,Display>> const& v){
	stringstream ss;
	ss<<"<select name=\""<<name<<"\">";
	for(auto elem:v){
		ss<<"<option value=\""<<elem.first<<"\"";
		if(elem.first==current) ss<<"selected=selected";
		ss<<">"<<elem.second<<"</option><br>";
	}
	ss<<"</select>";
	return Input{name,ss.str(),""};
}

template<typename T>
auto drop_down(string const& name,T const& current,vector<T> const& options){
	return drop_down(name,current,mapf([](auto x){ return make_pair(x,x); },options));
}

string to_db_type(const User*){ return "varchar(11)"; }

string to_db_type(const Datetime*){ return "datetime"; }

unsigned short rand(unsigned short const*){
	return rand();
}

Date parse(Date const*,std::string const& s){
	auto sp=split('-',s);
	if(sp.size()!=3) throw "Invalid date:"+s;
	return Date{
		(unsigned short)stoi(sp[0]),
		(unsigned short)stoi(sp[1]),
		(unsigned short)stoi(sp[2])
	};
	/*Date r;
	if(s=="0000-00-00"){

	}
	bzero(&r,sizeof(r));
	char *res=strptime(s.c_str(),"%Y-%m-%d",&r);
	if(!res){
		PRINT(s);
	}
	assert(res);
	assert(res[0]=='\0');
	return r;*/
}

bool operator<(Date const& a,Date const& b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	DATE_ITEMS(X)
	#undef X
	return 0;
}

bool operator==(Date const& a,Date const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	DATE_ITEMS(X)
	#undef X
	return 1;
}

bool operator!=(Date const& a,Date const& b){
	return !(a==b);
}

std::ostream& operator<<(std::ostream& o,Date const& a){
	//Try to display as something like "Thursday, March 5"
	stringstream ss;
	ss<<a.year<<"-"<<a.month<<"-"<<a.day;
	struct tm tm1;
	bzero(&tm1,sizeof tm1);
	char *res=strptime(ss.str().c_str(),"%Y-%m-%d",&tm1);
	if(!res || res[0]) return o<<ss.str();
	static const size_t LEN=26;
	char s[LEN];
	strftime(s,LEN,"%a, %b %d",&tm1);
	return o<<s;
}

Date rand(Date const*){
	Date r;
	#define X(A,B) r.B=rand((A*)0);
	DATE_ITEMS(X)
	#undef X
	return r;
}

string escape(Date const& a){
	static const size_t LEN=26;
	char s[LEN];
	sprintf(s,"%04d-%02d-%02d",a.year,a.month,a.day);
	return s;
}

Input show_input(DB db,string const& name,Date const& current){
	return Input{
		name,
		"<input type=\"date\" name=\""+name+"\" value=\""+escape(current)+"\">",
		""
	};
}

string to_db_type(const Date*){ return "date"; }

Input show_input(DB db,string const& name,URL const& value){
	auto notes="Must start with something like \"http://\"";
	if(prefix("http",value.data)){
		return Input{
			"<a href=\""+value.data+"\">"+name+"</a>",
			"<input type=\"text\" name=\""+name+"\" value=\""+value.data+"\">",
			notes
		};
	}
	auto r=show_input(db,name,value.data);
	r.notes+=notes;
	return r;
}

#define E_PRINT(A) if(a==T::A) return o<<""#A;
#define E_LIST(A) v|=T::A;
#define E_PARSE(A) if(s==""#A) return T::A;
#define E_ESCAPED(A) values|=string{"'"#A "'"};

#define ENUM_INPUT(NAME,OPTIONS)\
	Input show_input(DB db,string const& name,T const& value){\
		return drop_down(name,value,options(&value));\
	}\

#define ENUM_DEFS_MAIN(NAME,OPTIONS)\
	std::ostream& operator<<(std::ostream& o,NAME const& a){\
		OPTIONS(E_PRINT)\
		assert(0);\
	}\
	vector<NAME> options(const NAME*){\
		vector<NAME> v;\
		OPTIONS(E_LIST)\
		return v;\
	}\
	NAME rand(const NAME* a){\
		return choose(options(a));\
	}\
	NAME parse(const NAME*,string const& s){\
		OPTIONS(E_PARSE)\
		throw std::invalid_argument{""#NAME+string()+": "+s};\
	}\
	string escape(T const& a){ return "'"+as_string(a)+"'"; }\
	\
	string to_db_type(const T*){\
		vector<string> values;\
		OPTIONS(E_ESCAPED)\
		stringstream ss;\
		ss<<"enum(";\
		ss<<join(",",values);\
		ss<<")";\
		return ss.str();\
	}\
	std::optional<T>& operator+=(std::optional<T>& a,std::optional<T> b){\
		a={};\
		return a;\
	}

#define ENUM_DEFS(NAME,OPTIONS)\
	ENUM_DEFS_MAIN(NAME,OPTIONS)\
	ENUM_INPUT(NAME,OPTIONS)

#define T Part_state
ENUM_DEFS_MAIN(Part_state,PART_STATES)
#undef T

template<typename Value,typename Display>
Input drop_down_submit(string const& name,Value const& current,vector<pair<Value,Display>> const& v){
	stringstream ss;
	ss<<"<select name=\""<<name<<"\" onchange=\"this.form.submit()\">";
	for(auto elem:v){
		ss<<"<option value=\""<<elem.first<<"\"";
		if(elem.first==current) ss<<"selected=selected";
		ss<<">"<<elem.second<<"</option><br>";
	}
	ss<<"</select>";
	return Input{name,ss.str(),""};
}

Input show_input(DB db,std::string const& name,Part_state const& current){
	return drop_down_submit(
		name,current,
		mapf(
			[](auto x){ return make_pair(x,x); },
			options(&current)
		)
	);
}

#define T Machine
ENUM_DEFS(Machine,MACHINES)
#undef T

#define T Bend_type
ENUM_DEFS(Bend_type,BEND_TYPES)
#undef T

#define T Export_item
ENUM_DEFS(Export_item,EXPORT_ITEMS)
#undef T

#define T Assembly_state
ENUM_DEFS(Assembly_state,ASSEMBLY_STATES)
#undef T

#define T Bom_exemption
ENUM_DEFS(Bom_exemption,BOM_EXEMPTION_OPTIONS)
#undef T

#define T Bom_category
ENUM_DEFS(Bom_category,BOM_CATEGORY_OPTIONS)
#undef T

using Decimal=decimal::decimal32;

string to_db_type(const Decimal*){
	return "decimal(8,2)";
}

std::ostream& operator<<(std::ostream& o,Decimal const& a){
	return o<<decimal32_to_double(a);
}

Decimal rand(const Decimal*){ return (rand()%1000)/100; }

Decimal parse(const Decimal*,string const& s){
	auto sp=split('.',s);
	if(sp.size()==1){
		return std::decimal::make_decimal32((long long)stoi(sp[0]),0);
	}
	if(sp.size()==2){
		unsigned long long whole=stoi(sp[0]);
		int frac;
		if(sp[1].size()==1){
			frac=stoi(sp[1])*10;
		}else if(sp[1].size()==2){
			frac=stoi(sp[1]);
		}else{
			/*PRINT(s);
			PRINT(sp[1]);
			nyi*/
			throw "Max of 2 decimal places";
		}
		return std::decimal::make_decimal32(whole*100+frac,-2);
	}
	throw "Not a decimal";
}

Input show_input(DB db,string const& name,Decimal value){
	auto x=show_input(db,name,as_string(value));
	x.notes="Max 2 decimal places";
	return x;
}

string escape(Decimal a){ return as_string(a); }

vector<string> Material::suggestions()const{
	return {
		".090 Aluminum Plate",
		"1/8 Aluminum 6061",
		"1/8 Aluminum 5052",
		"Steel",
		"1/4 Aluminun Plate",
		"1x1 1/16 Tube",
		"1x2 1/16 Tube",
		"1x1 1/8 Tube",
		"1x2 1/8 Tube",
		"1/2 Hex",
		"3/8 Hex",
		"80/20",
		"Delrin",
	};
}

vector<string> Supplier::suggestions()const{
	return {"Vex","AndyMark","WCP","McMaster"};
}

string escape(Material const& s){
	return escape(s.s);
}

Input show_input(DB db,string const& name,Subsystem_id const& current){
	auto q=query(
		db,
		"SELECT subsystem_id,name FROM subsystem_info WHERE id IN (SELECT MAX(id) FROM subsystem_info GROUP BY subsystem_id) AND valid"
	);

	auto m=mapf(
		[](auto row){
			assert(row.size()==2);
			assert(row[0]);
			//assert(row[1]);
			return make_pair(*row[0],row[1]?*row[1]:"Untitled");
		},
		q
	);

	//show the current one that it's part of, color coded
	//show the other current ones in a drop-down
	Subsystem_editor page;
	page.id=current;
	auto x=drop_down(name,as_string(current),m);
	x.name=link(page,x.name);
	return x;
}

Input show_input(DB db,string const& name,std::optional<Subsystem_id> const& current){
	auto q=query(
		db,
		"SELECT subsystem_id,name FROM subsystem_info "
		"WHERE "
			"id IN (SELECT MAX(id) FROM subsystem_info GROUP BY subsystem_id) "
			" AND valid"
	);

	vector<pair<string,string>> m;
	m|=pair<string,string>("None","NULL");
	m|=mapf(
		[](auto row){
			assert(row.size()==2);
			assert(row[0]);
			//assert(row[1]);
			return make_pair(*row[0],row[1]?*row[1]:"Untitled");
		},
		q
	);

	Input out=drop_down(name,as_string(current),m);
	out.notes="Name of subsystem which this should be a part of.  If this is intended to be a top-level project, choose \"NULL\"";
	if(current){
		Subsystem_editor page;
		page.id=*current;
		out.name=link(page,out.name);
	}
	return out;
}

Dummy parse(const Dummy*,std::string const&){
	return Dummy{};
}

std::ostream& operator<<(std::ostream& o,Dummy){
	return o;
}

std::string to_db_type(Subsystem_prefix const*){
	return "varchar(2)";
}

Subsystem_prefix::Subsystem_prefix():a('A'),b('A'){}

Subsystem_prefix::Subsystem_prefix(char a1,char b1):a(a1),b(b1){
}

std::string Subsystem_prefix::get()const{
	return string()+a+b;
}

std::ostream& operator<<(std::ostream& o,Subsystem_prefix const& a){
	return o<<a.get();
}

bool operator<(Subsystem_prefix const& a,Subsystem_prefix const& b){
	return a.get()<b.get();
}

bool operator>(Subsystem_prefix const& a,Subsystem_prefix const& b){
	return a.get()<b.get();
}

bool operator==(Subsystem_prefix const& a,Subsystem_prefix const& b){
	return a.get()==b.get();
}

bool operator!=(Subsystem_prefix const& a,Subsystem_prefix const& b){
	return a.get()!=b.get();
}

Subsystem_prefix rand(Subsystem_prefix const*){
	return Subsystem_prefix(
		'A'+(rand()%26),
		'A'+(rand()%26)
	);
}

std::string escape(Subsystem_prefix const& a){
	return escape(a.get());
}

Subsystem_prefix parse(Subsystem_prefix const*,std::string const& s){
	if(s.size()!=2) throw "Bad prefix length";
	if(s[0]<'A' || s[1]>'Z') throw "Invalid char1";
	if(s[1]<'A' || s[1]>'Z') throw "Invalid char2";
	return Subsystem_prefix{s[0],s[1]};
}

Input show_input(DB db,string const& name,Subsystem_prefix const& a){
	auto x=show_input(db,name,as_string(a));
	x.notes=" Two upper case characters";
	return x;
}

Part_number_local::Part_number_local(std::string const& a){
	//XX000-1425-2020
	//15 chars
	if(a.size()!=15) throw "Part_number_local: wrong length ("+as_string(a.size())+" , expected 15)";
	subsystem_prefix=Subsystem_prefix(a[0],a[1]);

	auto n=a.substr(2,3);
	for(auto c:n){
		if(!isdigit(c)){
			throw "Part_number_local: expected digit";
		}
	}
	num=stoi(n);

	auto suffix=a.substr(5,100);
	if(suffix!="-1425-2020"){
		throw "Part_number_local: wrong suffix";
	}
}

Part_number_local::Part_number_local(Part_number const& a):
	Part_number_local(a.data)
{}

Part_number_local::Part_number_local(Subsystem_prefix a,Three_digit b):
	subsystem_prefix(a),num(b)
{}

std::string Part_number_local::get()const{
	stringstream ss;
	ss<<subsystem_prefix<<num<<"-1425-2020";
	return ss.str();
}

std::ostream& operator<<(std::ostream& o,Part_number_local const& a){
	return o<<a.get();
}

bool operator<(Part_number_local const& a,Part_number_local const& b){
	#define X(A) if(a.A<b.A) return 1; if(a.A>b.A) return 0;
	X(subsystem_prefix)
	X(num)
	#undef X
	return 0;
}

std::string escape(Part_number_local const& a){
	return "'"+a.get()+"'";
}

Part_number_local next(Part_number_local a){
	a.num=a.num+1;
	return a;
}

Three_digit::Three_digit():value(0){}

Three_digit::Three_digit(int x):value(x){
	assert(x>=0 && x<=999);
}

Three_digit& Three_digit::operator=(int i){
	if(i<0 || i>999) throw "Invalid Three_digit";
	value=i;
	return *this;
}

Three_digit::operator int()const{
	return value;
}

ostream& operator<<(std::ostream& o,Three_digit a){
	char s[10];
	sprintf(s,"%03d",int(a));
	return o<<s;
}

#define NO_ADD_IMPL(X) \
	std::optional<X>& operator+=(std::optional<X>& a,std::optional<X> const&){\
		a={};\
		return a;\
	}
NO_ADD_IMPL(Subsystem_id)
NO_ADD_IMPL(Part_id)

Input show_input(DB,std::string const& name,Part_checkbox const& a){
	//return "<br><input type=\"checkbox\" name=\""+name+":"+as_string(a)+"\">";
	return Input{name,"<input type=\"checkbox\" name=\""+name+":"+as_string(a)+"\">",""};
}

Valid& Valid::operator=(bool b){
	data=b;
	return *this;
}

Input show_input(DB db,std::string const& name,Valid const& a){
	auto r=show_input(db,name,a.data);
	r.notes+="In other words, this part should continue to exist.";
	return r;
}

Input show_input(DB db,std::string const& name,DNI const& a){
	auto r=show_input(db,name,a.data);
	r.notes+="\"Do not install\".  For items that should not be physically included in their enclosing subsystem.";
	return r;
}

Input show_input(DB db,std::string const& name,Hours const& current){
	auto r=show_input(db,name,current.data);
	r.notes="Hours; "+r.notes;
	return r;
}

Input show_input(DB db,std::string const& name,Priority const& current){
	auto r=show_input(db,name,current.data);
	r.notes="Higher number=higher priority "+r.notes;
	return r;
}

std::ostream& operator<<(std::ostream& o,Part_id const& a){
	return o<<"Part_id("<<a.data<<")";
}

std::ostream& operator<<(std::ostream& o,Meeting_id const& a){
	return o<<"Meeting_id("<<a.data<<")";
}

std::ostream& operator<<(std::ostream& o,Subsystem_id const& a){
	return o<<"Subsystem_id("<<a.data<<")";
}
