#include "data_types.h"
#include<cassert>

using namespace std;

string with_suggestions(string name,string value,vector<string> const& suggestions){
	stringstream ss;
	ss<<"<br>"<<name<<":<input name=\""<<name<<"\" list=\""<<name<<"\" value=\""<<value<<"\">";
	ss<<"<datalist id=\""<<name<<"\">";
	for(auto a:suggestions){
		ss<<"<option value=\""<<a<<"\">";
	}
	ss<<"</datalist>";
	return ss.str();
}

string show_input(DB,string name,string value){
	return "<br>"+name+":<input type=\"text\" name=\""+name+"\" value=\""+value+"\">";
}

string show_input(DB db,string name,int value){
	return show_input(db,name,as_string(value))+" (integer)";
}

string show_input(DB db,string name,double value){
	return show_input(db,name,as_string(value))+" (fraction)";
}

string show_input(DB db,string name,unsigned value){
	return show_input(db,name,as_string(value))+" (non-negative integer)";
}

string show_input(DB db,string name,bool value){
	return "<br>"+name+":<input type=\"checkbox\" name=\""+name+"\" "+
		[=](){ if(value) return "checked=on"; return ""; }()+"\">";
}

string escape(string s){
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
string drop_down(string name,Value current,vector<pair<Value,Display>> const& v){
	stringstream ss;
	ss<<"<br>"<<name<<":";
	ss<<"<select name=\""<<name<<"\">";
	for(auto elem:v){
		ss<<"<option value=\""<<elem.first<<"\"";
		if(elem.first==current) ss<<"selected=selected";
		ss<<">"<<elem.second<<"</option><br>";
	}
	ss<<"</select>";
	return ss.str();
}

template<typename T>
string drop_down(string name,T current,vector<T> const& options){
	return drop_down(name,current,mapf([](auto x){ return make_pair(x,x); },options));
}

string to_db_type(const User*){ return "varchar(11)"; }

string to_db_type(const Datetime*){ return "datetime"; }

bool operator==(Date a,Date b){
	return a.s==b.s;
}

bool operator!=(Date a,Date b){
	return !(a==b);
}

std::ostream& operator<<(std::ostream& o,Date a){
	return o<<a.s;
}

Date rand(const Date*){ return {rand((string*)nullptr)}; }

Date parse(const Date*,string s){
	return {s};
}

string show_input(DB db,string name,Date current){
	return "<br>"+name+":<input type=\"date\" name=\""+name+"\" value=\""+as_string(current)+"\">";
}

string escape(Date a){ return escape(a.s); }

string to_db_type(const Date*){ return "date"; }

bool operator==(URL a,URL b){
	return a.s==b.s;
}

bool operator!=(URL a,URL b){
	return !(a==b);
}

std::ostream& operator<<(std::ostream& o,URL const& a){
	return o<<a.s;
}

URL rand(const URL*){
	return {rand((string*)nullptr)};
}

URL parse(const URL*,string const& s){
	return {s};
}

string to_db_type(const URL*){
	return "varchar(100)";
}

string escape(URL const& a){
	return escape(a.s);
}

string show_input(DB db,string name,URL const& value){
	if(prefix("http",value.s)){
		return "<br><a href=\""+value.s+"\">"+name+":</a><input type=\"text\" name=\""+name+"\" value=\""+value.s+"\">";
	}
	return show_input(db,name,value.s);
}

#define E_PRINT(A) if(a==T::A) return o<<""#A;
#define E_LIST(A) v|=T::A;
#define E_PARSE(A) if(s==""#A) return T::A;
#define E_ESCAPED(A) values|=string{"'"#A "'"};

#define ENUM_DEFS(NAME,OPTIONS)\
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
		throw std::invalid_argument{""#NAME};\
	}\
	string show_input(DB db,string name,T value){\
		return drop_down(name,value,options(&value));\
	}\
	string escape(T a){ return "'"+as_string(a)+"'"; }\
	\
	string to_db_type(const T*){\
		vector<string> values;\
		OPTIONS(E_ESCAPED)\
		stringstream ss;\
		ss<<"enum(";\
		ss<<join(",",values);\
		ss<<")";\
		return ss.str();\
	}

#define T Part_state
ENUM_DEFS(Part_state,PART_STATES)
#undef T

#define T Machine
ENUM_DEFS(Machine,MACHINES)
#undef T

#define T Bend_type
ENUM_DEFS(Bend_type,BEND_TYPES)
#undef T

using Decimal=decimal::decimal32;

string to_db_type(const Decimal*){
	return "decimal(8,2)";
}

std::ostream& operator<<(std::ostream& o,Decimal const& a){
	return o<<decimal32_to_double(a);
}

Decimal rand(const Decimal*){ return (rand()%1000)/100; }

Decimal parse(const Decimal*,string s){
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

string show_input(DB db,string name,Decimal value){
	return show_input(db,name,as_string(value))+" Max 2 decimal places";
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

Id rand(const Id*){ return rand()%100; }
string to_db_type(const Id*){ return "int(11)"; }

string to_db_type(const Subsystem_id*){ return "int(11)"; }

bool operator==(Subsystem_id const& a,Subsystem_id const& b){
	return a.id==b.id;
}

bool operator!=(Subsystem_id const& a,Subsystem_id const& b){
	return !(a==b);
}

std::ostream& operator<<(std::ostream& o,Subsystem_id const& a){
	//return o<<"Subsystem_id("<<a.id<<")";
	return o<<a.id;
}

Subsystem_id rand(const Subsystem_id*){
	return {rand((int*)0)};
}

int parse(const int*,string s){ return stoi(s); }

Subsystem_id parse(const Subsystem_id*,string s){
	return {parse((int*)0,s)};
}

string show_input(DB db,string name,Subsystem_id const& current){
	/*auto q=query(
		db,
		"SELECT a.subsystem_id,a.name,a.valid FROM \n"
		"(SELECT subsystem_id,valid,name,max(edit_date) FROM subsystem_info GROUP BY subsystem_id) as a\n"
		"WHERE a.valid"
	);*/

	auto q=query(
		db,
		//"SELECT * FROM subsystem_info WHERE (subsystem_id,edit_date) IN (SELECT subsystem_id,MAX(edit_date) FROM subsystem_info GROUP BY subsystem_id)"
		"SELECT subsystem_id,name FROM subsystem_info WHERE (subsystem_id,edit_date) IN (SELECT subsystem_id,MAX(edit_date) FROM subsystem_info GROUP BY subsystem_id) AND valid"
	);

	//PRINT(q);
	auto m=mapf(
		[](auto row){
			assert(row.size()==2);
			return make_pair(*row[0],*row[1]);
		},
		q
	);

	//show the current one that it's part of, color coded
	//show the other current ones in a drop-down
	return drop_down(name,as_string(current),m);
}

string escape(Subsystem_id const& a){
	return escape(a.id);
}

